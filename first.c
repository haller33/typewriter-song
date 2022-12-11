#define NOBUILD_IMPLEMENTATION
#include "./include/nobuild.h"
#include "./include/nobuild_extended.h"

#define CC "clang"
#define BIN_PATH "bin"
#define SRC_PATH "src"
#define CFLAGS "-Wall", "-Wextra", "-lrt", "-std=c99", "-pedantic", "-pthread"

// pw-play Lost-title.wav
// pw-cat -p Lost-title.wav
// aplay Lost-title.wav


#define TRUE_T 1
#define FALSE_T 0

#define NOT(x) !x

#define PERMA_CLEAN TRUE_T

void
build_file(const char *tool, int debug_info) {
  
  if (!path_is_dir(BIN_PATH)) {
    
    MKDIRS(BIN_PATH);
  }
  Cstr tool_path = PATH(SRC_PATH, tool);
  Cstr bin_path = PATH(BIN_PATH, tool);
  Cstr debug_info_bin = "";
  if (debug_info) {
    
    debug_info_bin = "-g";
  }
  
#ifndef _WIN32
  CMD(CC, CFLAGS, debug_info_bin, "-o", NOEXT(bin_path), tool_path);
#else
  CMD("cl.exe", "/Fe.\\bin\\", tool_path);
#endif
}

void
build_src (int no_debug_info) {
  
    FOREACH_FILE_IN_DIR(tool, SRC_PATH, {
	
        if (ENDS_WITH(tool, ".c")) {

	  build_file(tool, NOT(no_debug_info));
        }
    });
}

void
clean_src (int deep_clean) {
  
  RM(PATH(BIN_PATH));

  if ( deep_clean ) {
    
    RM("first");
  }
  
#if PERMA_CLEAN
  RM("first");
#endif

  if(path_exists("first.old")) {
    
    RM("first.old");
  }
}

void
run_src (void) {
  
  CMD("./bin/typewriter-song");
}

int
main (int argc, char **argv) {

  GO_REBUILD_URSELF ( argc, argv );

  check_instaled_program (  );
  
  if (argc == 1) {

    build_src ( TRUE_T );
  } else {

    if ( strcmp(argv[1], "clean") == 0 ) {

      clean_src ( FALSE_T );
    } else if ( strcmp(argv[1], "deep-clean") == 0) {

      clean_src ( TRUE_T );
    } else if ( strcmp(argv[1], "debug") == 0) {

      build_src ( FALSE_T );
    } else if ( strcmp(argv[1], "build") == 0) {

      build_src ( TRUE_T );
    } else if ( strcmp(argv[1], "run") == 0) {

      run_src ( );
    } else {

      Cstr msg_err = "error on argument";
      ERRO(msg_err);
    }
  }

  return 0;
}


