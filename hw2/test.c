#include <dlfcn.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdarg.h>
#include <fcntl.h>
#include <string.h>


#define EMPTY_STRING  ""
#define NULL_STRING "(null)"

#define inject_symbol(lib, target, symbol, type) \
  if ( target == NULL ){ \
    void* handle; \
    handle = dlopen(lib, RTLD_LAZY); \
    if(handle == NULL) \
      fprintf(stderr, "Can't open %s, error: %s\n", lib, dlerror()); \
    target = (type)dlsym( handle, symbol ); \
    if ( target == NULL) \
      fprintf(stderr, "link lib %s failed\n", symbol ); \
  } 
 
static FILE* out = NULL;

static int (*old_closedir)(DIR*) = NULL;


int closedir(DIR* dir){ 
  inject_symbol("libc.so.6", old_closedir, "closedir", int(*)(DIR*));
  printf("hello2\n");
  int ret = (*old_closedir)(dir);
  printf("hello3\n");
  fprintf(stderr,"[monitor] closedir(%p) = %d\n", dir, ret);
  return ret;
}

int main(){
  DIR* dir = opendir(".");
  closedir(dir);
  return 0;
}
