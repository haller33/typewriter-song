// Server side C program to use Socket for play music song on user space
// programming
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
// for fork stuff
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <signal.h>
//// threads
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details. and pthread gettid
#include <pthread.h>

#include "../lib/tinycthread.h"

#define _unused(x) ((void)(x))

#define PORT 4433
#define BUFFER_SIZE 32

#define FAIL_ON_NOT_CONNECT false
#define SEND_RESPONSE false
#define GTHREAD_ID_SHOW false
#define PLAY_SONG true
#define SHOW_OUTPUT false
#define NOT_SILENT true

typedef struct incoming_t Incoming;
typedef struct send_thread_data_t Send_gTData;

struct incoming_t {
  bool flag_loop;
  bool exit_process;
  int n_socket;
};

struct send_thread_data_t {
  struct sockaddr_in *address;
  char *buffer;

  int server_fd;
  int new_socket;
};

int
work_ent (void) {

#if GTHREAD_ID_SHOW
  pid_t main_thread = gettid();

  printf("from enter_clink tid :: %d\n", main_thread);
#endif
  
#if PLAY_SONG
  system("pw-play --volume=0,1 -p ../resources/clink.wav > /dev/null 2>&1");
#endif
  return 0;
}

int
work_run (void) {


#if GTHREAD_ID_SHOW
  pid_t main_thread = gettid();

  printf("from click tid :: %d\n", main_thread);
#endif

#if PLAY_SONG
  system("pw-play --volume=0,1 -p ../resources/click.wav > /dev/null 2>&1");
#endif
  
  return 0;
}

int
ack_response (void *data_vt) {

  _unused ( data_vt );
#if GTHREAD_ID_SHOW
  pid_t main_thread = gettid();

  printf("from send tid :: %d\n", main_thread);
#endif

#if SEND_RESPONSE
  Send_gTData *data = (struct send_thread_data_t *)data_vt;
  
  sendto(data->server_fd, data->buffer, strlen(data->buffer), MSG_CONFIRM,
	 (struct sockaddr *)data->address, sizeof(data->address));

#endif
  return 0;
}

Incoming
incoming_conection_fork (size_t server_fd,
				  size_t new_socket,
				  struct sockaddr_in *address,
				  size_t *addrlen,
				  char *buffer) {

  Incoming retr;
  Send_gTData data_fsend;

  struct sockaddr_in *cli_address;
  char* ack_msg = "ACK/RUN\n";
  thrd_t work_gthread_id, send_gthread_id;
  size_t client_fd_val = 0, ret = 0;

  // because of macros
  _unused ( data_fsend );
  _unused ( ack_msg );
  _unused ( address );
  _unused ( ret );

    
  memset (&cli_address, 0, sizeof(cli_address));
  //  memset (&cli_addrlen, 0, sizeof(*cli_addrlen));
  memset (&work_gthread_id, 0, sizeof(thrd_t));
  memset (&send_gthread_id, 0, sizeof(thrd_t));

  retr.exit_process = false;
  retr.n_socket = new_socket;
  retr.flag_loop = true;

#if NOT_SILENT
  printf("lisening up udp unix/socket %d\n", PORT);
#endif
  while (retr.flag_loop) {

#if GTHREAD_ID_SHOW
    pid_t main_thread = gettid();

    printf("from main tid :: %d | %d\n ", main_thread, server_fd);
#endif
    
    client_fd_val = recvfrom(server_fd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL,// MSG_DONTWAIT,
			     (struct sockaddr *)cli_address, (socklen_t *)addrlen);
    if (client_fd_val > 0) {

      if (strlen(buffer) > 0) {

#if SHOW_OUTPUT
	puts(buffer);
	puts("\n");
#endif
	
	if (strcmp ( "ACK/RUN\0", buffer ) == 0) {
	  
	  ret = thrd_create((thrd_t *)&work_gthread_id, (thrd_start_t)work_run, (void*)0);
#if FAIL_ON_NOT_CONNECT
	  assert (ret == thrd_success);
#endif
      
	} else if (strcmp ( "ACK/EXT", buffer ) == 0) {

	  retr.flag_loop = false;
	  retr.exit_process = true;
#if SHOW_OUTPUT
	  // puts ("exiting");
#endif
	  return retr;
	} else if (strcmp ( "ACK/ENT\0", buffer ) == 0) {

	  ret = thrd_create(&work_gthread_id, (thrd_start_t)work_ent, (void*)0);
#if FAIL_ON_NOT_CONNECT
	  assert (ret == thrd_success);
#endif
#if SHOW_OUTPUT
	  // puts ("enter");
#endif
	} else {
	  
	  ret = thrd_create((thrd_t *)&work_gthread_id, (thrd_start_t)work_run, (void*)0);
#if FAIL_ON_NOT_CONNECT
	  assert (ret == thrd_success);
#endif
	}
      
      }

#if SEND_RESPONSE
      
      ret = ret ^ ret;

      data_fsend.buffer = buffer;
      data_fsend.address = cli_address;
      data_fsend.server_fd = server_fd;
      data_fsend.new_socket = new_socket;
      
      ret = thrd_create(&send_gthread_id, ack_response, (void*)&data_fsend);
#if FAIL_ON_NOT_CONNECT
      assert (ret == thrd_success);
#endif
#endif
#if SHOW_OUTPUT
      puts("ACK\n");
#endif
      
      memset (buffer, 0, BUFFER_SIZE - 1 );
      retr.n_socket = new_socket;
      
    } else { // client_fd_val < 0
      
      perror("accept");
#if SHOW_OUTPUT
      printf("buffer :: %s\n", buffer);
      printf("sockid :: %d\n", new_socket);
      printf("sockfd :: %d\n", server_fd);
#endif
      exit(EXIT_FAILURE);
    }
  }
  
  return retr;
}

int
main(int argc, char const* argv[]) {
  
  size_t server_fd, new_socket = 0;
  struct sockaddr_in address;
  size_t addrlen = sizeof(address);
  char buffer[BUFFER_SIZE] = { 0 };

  _unused ( argc );
  _unused ( argv );

  memset ( &buffer, 0x0, BUFFER_SIZE );

  // Creating socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }
  
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  // Forcefully attaching socket to the port 8080
  if (bind(server_fd, (struct sockaddr*)&address,
	   sizeof(address))
      < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }  

  Incoming in_ret_maybe_child =
    incoming_conection_fork (server_fd,
			     new_socket,
			     &address,
			     &addrlen,
			     buffer);
  
  if (in_ret_maybe_child.exit_process) {
#if SHOW_OUTPUT
    printf("close socket connection\n");
#endif
    // closing the connected socket
    close(in_ret_maybe_child.n_socket);
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
  }
  
  return 0;
}
