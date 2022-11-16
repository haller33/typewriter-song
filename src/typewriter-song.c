#define NOBUILD_IMPLEMENTATION
#include "../include/nobuild.h"
#include "../lib/tinycthread.h"
#include "../lib/definition.h"

#define SRC_PATH "src"
#define MAX_CARACTER_FOR_PROGRAM_NAME 80 // fortran stuff

#define _unused(x) ((void)(x))

void
name_programs_path_array ( Cstr *programs_tofill, const char *dir_path ) {

  int counter = 0;
  FOREACH_FILE_IN_DIR(item, dir_path, {
      
      if (ENDS_WITH(item, ".c")) {

	programs_tofill[counter] = NOEXT(item);
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

void
check_instaled_program (void ) {
  ;
}

int
main (void) {

  char *envs = getenv("PATH");
  CMD("echo", "hello world");
  //  INFO("ENVS :: %s", envs);

  _unused(envs);
  
  // check_instaled_program();

  // TODO: create a way to check enviropment paths if files exists. suck as typewriter programs
  
  return 0;
}
