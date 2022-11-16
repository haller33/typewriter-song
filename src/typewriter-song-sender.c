// Server side C program to use Socket for play music song on user space
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>

// #define SA_RESTART 0x10000000

#define _unused(x) ((void)(x))

#define PORT 4433
#define BUFFER_SIZE 16

#define SEND_RESPONSE false

int
main (int argc, char const* argv[]) {
  char possibilities[4][BUFFER_SIZE] = {
    "ACK/RUN\0",
    "ACK/EXT\0",
    "ACK/ENT\0",
    "ACK/DEL\0"
  };
  char buffer[BUFFER_SIZE];
  size_t sockfd;
  size_t ack_position = 0;
  struct sockaddr_in servaddr;

  _unused ( buffer );
  
  if ( argc > 1 ) {

    if (strcmp ( "RUN", argv[1] ) == 0) {
      ack_position = 0;
    } else if (strcmp ( "EXIT", argv[1] ) == 0) {
      ack_position = 1;
    } else if (strcmp ( "ENT", argv[1] ) == 0) {
      ack_position = 2;
    } else {
      ack_position = 0;
    }
  } else {
    ack_position = 0;
  }
     
  // Creating socket file descriptor 
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
    perror("socket creation failed"); 
    exit(EXIT_FAILURE); 
  }
  
  memset(&servaddr, 0, sizeof(servaddr)); 
  
  // Filling server information 
  servaddr.sin_family = AF_INET; 
  servaddr.sin_port = htons(PORT); 
  servaddr.sin_addr.s_addr = INADDR_ANY;
  
        
  sendto(sockfd, (const char *)possibilities[ack_position], BUFFER_SIZE, MSG_CONFIRM,
	 (const struct sockaddr *) &servaddr,
	 sizeof(servaddr)); 
  // printf("ack message sent.\n");

#if SEND_RESPONSE
  memset(buffer, 0, BUFFER_SIZE - 1);
  n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_DONTWAIT, // | MSG_WAITALL  
	       (struct sockaddr *) &servaddr, 
	       &len); 
  //  buffer[n] = '\0';
  printf("Server : %s\n -- %d", buffer, n); 
#endif
  
  close(sockfd); 
  exit(0);
}

