#ifndef NOBUILD_IMPLEMENTATION
#define NOBUILD_IMPLEMENTATION
#endif
   // #include "../include/nobuild.h"
#include "../lib/tinycthread.h"
#include "../lib/definition.h"

#define SRC_PATH "src"
#define LIB_PATH "lib"
#define FILE_TO_CONFIG "definition.h"
#define MAX_CARACTER_FOR_PROGRAM_NAME 32 // fortran stuff, sqn
#define DEBUG_MODE 0
#define DEBUG_MEM 1

typedef struct Cstr_array_t Cstr_arr;

struct Cstr_array_t {

  int flag_ok;
  char *cstr_arr;
  int size;
  int total_colunas;
};

void
name_programs_path_array ( char **programs_tofill, const char *dir_path ) {

  int counter = 0;
  FOREACH_FILE_IN_DIR(item, dir_path, {
      
      if (ENDS_WITH(item, ".c")) {

	strcpy ((programs_tofill)[counter], (char *)NOEXT(item));
	counter = counter + 1;
      }
    });
}

void
count_programs_to_be_installed ( int *counts, const char *dir_path ) {

  FOREACH_FILE_IN_DIR(item, dir_path, {
      
      if (ENDS_WITH(item, ".c")) {
	
	*counts = (*counts)+1;
      }
    });
}

Cstr_arr
check_will_install_program (void) {
  
  Cstr_arr retrs;
  char **programs_path_and_name = NULL;
  char *programs_str = NULL;
  Cstr PATH_PWD = getenv("PWD");
  Cstr pathTo = PATH(PATH_PWD, SRC_PATH); 
  int i = 0, j = 0;
  int counted = 0;
  int offset = 0;
  
  count_programs_to_be_installed ( &counted, pathTo );

  if ( !( TOTAL_PROGRAMS == counted ) ) {

    programs_path_and_name = (char**) malloc ( sizeof ( char** ) * counted );
  
    for (i = 0; i < counted; i++) {

      (programs_path_and_name)[i] =
	(char*) malloc ( sizeof ( char ) * MAX_CARACTER_FOR_PROGRAM_NAME );
    
      memset(&(programs_path_and_name)[i][0], 0, sizeof(char) * MAX_CARACTER_FOR_PROGRAM_NAME);
    }
  
    name_programs_path_array (programs_path_and_name, pathTo );

#if DEBUG_MODE
  
    for (i = 0; i < counted; i++) {
      puts (programs_path_and_name[i]);
    }

#endif
  
    programs_str =
      (char*) calloc (counted * MAX_CARACTER_FOR_PROGRAM_NAME + counted + 1, sizeof(char));

    offset = 0;
    for (i = 0; i < counted; i++) {
      for (j = 0; j < MAX_CARACTER_FOR_PROGRAM_NAME; j++) {
	if (programs_path_and_name[i][j] == '\0')
	  continue;
	// printf("%c", programs_path_and_name[i][j]);
	programs_str[i*MAX_CARACTER_FOR_PROGRAM_NAME + j + offset] = programs_path_and_name[i][j];
      }

      if (offset < counted - 1)
	programs_str[i*MAX_CARACTER_FOR_PROGRAM_NAME + j + offset] = ',';
      offset = offset + 1;

      if (offset == counted)
	programs_str[i*MAX_CARACTER_FOR_PROGRAM_NAME + j + offset] = '\0';
      // printf("\n");
    }

    assert ( offset == counted );

#if DEBUG_MODE

    for (i = 0; i < counted*MAX_CARACTER_FOR_PROGRAM_NAME; i++)
      printf("%c", programs_str[i]);

#endif

    retrs.flag_ok       = 0;
    retrs.size          = counted * MAX_CARACTER_FOR_PROGRAM_NAME + offset + 1;
    retrs.cstr_arr      = programs_str;
    retrs.total_colunas = counted;

#if DEBUG_MEM
    for (i = 0; i < counted; i++)
      free(programs_path_and_name[i]);

    free(programs_path_and_name);

#endif
  } else {
    
    retrs.flag_ok       = 1;
    retrs.size          = sizeof(PROGRAMS);
    retrs.cstr_arr      = PROGRAMS;
    retrs.total_colunas = counted;
  }
    
  return retrs;
}

int
check_instaled_program (void) {

  Cstr_arr programs_list;
  char* programs_str = NULL;
  char total_str[3] = { 0 };
  int i = 0;

  programs_list = check_will_install_program (  );

#if DEBUG_MODE

  for (i = 0; i < programs_list.size; i++) 
    printf("%c", (char)programs_list.cstr_arr[i]);
  //  puts(programs_list.cstr_arr);
  puts("\n");

#endif
  
  if ( !programs_list.flag_ok ) {
    FILE *fd = fopen ( PATH(LIB_PATH, FILE_TO_CONFIG), "w" );

    fputs("#define TOTAL_PROGRAMS ", fd);

    fprintf(fd, "%d", programs_list.total_colunas);
    // fputs(total_str, fd);
  
    fputs("\n", fd);

    fputs("#define PROGRAMS \"", fd);

    for (i = 0; i < programs_list.size; i++) {
      if (!programs_list.cstr_arr[i] == '\0') {
	fputc(programs_list.cstr_arr[i], fd);
      }
    }
  
    fputs("\"", fd);

    fputs("\n", fd);
  
    fclose(fd);
    free(programs_list.cstr_arr);
  }
  return 0;
}

