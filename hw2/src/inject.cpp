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

#define SET_FP() \
  if ( out == NULL ) set_output_file();

#define INJECT_SYMBOL(lib, target, symbol, type) \
  if ( target == NULL ){ \
    void* handle; \
    handle = dlopen(lib, RTLD_LAZY); \
    if(handle == NULL) \
      fprintf(stderr, "Can't open %s, error: %s\n", lib, dlerror()); \
    target = (type)dlsym( handle, symbol ); \
    if ( target == NULL) \
      fprintf(stderr, "link lib %s failed\n", symbol ); \
    SET_FP(); \
  }  



/* general proto */
FILE* out = NULL;
void open_dl(void*,const char*);


/* dirent.h proto */
static int (*old_closedir)(DIR*) = NULL;
static DIR* (*old_fdopendir)(int) = NULL;
static DIR* (*old_opendir)(const char*) = NULL;
static struct dirent* (*old_readdir)(DIR*) = NULL;
static int (*old_readdir_r)(DIR*, struct dirent*, struct dirent**) = NULL;
static void (*old_rewinddir)(DIR*) = NULL;
static void (*old_seekdir)(DIR*, long) = NULL;
static long (*old_telldir)(DIR*) = NULL;

/* fcntl.h proto */
static int (*old_creat)(const char*, mode_t) = NULL;
static int (*old_open)(const char*, int, ...) = NULL;

/* stdio.h proto */
static int (*old_remove)(const char*) = NULL;
static int (*old_rename)(const char*, const char*) = NULL;
static void (*old_setbuf)(FILE*, char*) = NULL;
static int (*old_setvbuf)(FILE*, char*, int, size_t) = NULL;
static char* (*old_tempnam)(const char*, const char*) = NULL;
static FILE* (*old_tmpfile)(void) = NULL;
static char* (*old_tmpnam)(char*) = NULL;

/* stdlib.h proto */
static void (*old_exit)(int) = NULL;
static char* (*old_getenv)(const char*) = NULL;
static char* (*old_mkdtemp)(char*) = NULL;
static int (*old_mkstemp)(char*) = NULL;
static int (*old_putenv)(char*) = NULL;
static int (*old_rand)(void) = NULL;
static int (*old_rand_r)(unsigned*) = NULL;
static int (*old_setenv)(const char*, const char*, int) = NULL;
static void (*old_srand)(unsigned) = NULL;
static int (*old_system)(const char*) = NULL;

/* unistd.h proto */ 
static int (*old_chdir)(const char*) = NULL;
static int (*old_chown)(const char*, uid_t, gid_t) = NULL;
static int (*old_close)(int) = NULL;
static int (*old_dup)(int) = NULL;
static int (*old_dup2)(int,int) = NULL;
static void (*old__exit)(int) = NULL;
static int (*old_execl)(const char*, const char*, ...) = NULL;
static int (*old_execle)(const char*, const char*, ...) = NULL;
static int (*old_execlp)(const char*, const char*) = NULL;
static int (*old_execv)(const char*, char *const []) = NULL;
static int (*old_execve)(const char*, char *const [], char *const []) = NULL;
static int (*old_execvp)(const char*, char *const []) = NULL;
static int (*old_fchdir)(int) = NULL;
static int (*old_fchown)(int, uid_t, gid_t) = NULL;
static pid_t (*old_fork)(void) = NULL;
static int (*old_fsync)(int) = NULL;
static int (*old_ftruncate)(int, off_t) = NULL;
static char* (*old_getcwd)(char*, size_t) = NULL;
static gid_t (*old_getegid)(void) = NULL;
static uid_t (*old_geteuid)(void) = NULL;
static gid_t (*old_getgid)(void) = NULL;
static uid_t (*old_getuid)(void) = NULL;
static int (*old_link)(const char*, const char*) = NULL;
static int (*old_pipe)(int [2]) = NULL;
static ssize_t (*old_pread)(int, void*, size_t, off_t) = NULL;
static ssize_t (*old_pwrite)(int, const void*, size_t, off_t) = NULL;
static ssize_t (*old_read)(int, void*, size_t) = NULL;
static ssize_t (*old_readlink)(const char*, char*, size_t) = NULL;
static int (*old_rmdir)(const char*) = NULL;
static int (*old_setegid)(gid_t) = NULL;
static int (*old_seteuid)(uid_t) = NULL;
static int (*old_setgid)(gid_t) = NULL;
static int (*old_setuid)(uid_t) = NULL;
static unsigned (*old_sleep)(unsigned) = NULL;
static int (*old_symlink)(const char*, const char*) = NULL;
static int (*old_unlink)(const char*) = NULL;
static ssize_t (*old_write)(int, const void*, size_t) = NULL; 

/* sys_stat.h */
static int (*old_chmod)(const char*, mode_t) = NULL;
static int (*old_fchmod)(int, mode_t) = NULL;
static int (*old_fstat)(int, struct stat*) = NULL;
static int (*old_lstat)(const char*, struct stat*) = NULL;
static int (*old_mkdir)(const char*, mode_t) = NULL;
static int (*old_mkfifo)(const char*, mode_t) = NULL;
static int (*old_stat)(const char*, struct stat*) = NULL;
static mode_t (*old_umask)(mode_t) = NULL;

/* general function */ 
void set_output_file(){
  char* file = old_getenv == NULL ? getenv("MONITOR_OUTPUT") : old_getenv("MONITOR_OUTPUT");
  if (out != NULL)
    return;
  if (strcmp(file, "stderr") == 0)
    out = stderr;
  else if (strcmp(file, "stdout") == 0)
    out = stdout;
  else
    out = fopen(file ,"w");
}

/* dirent.h injection implementation */
int closedir(DIR* dir){ 
  INJECT_SYMBOL("libc.so.6", old_closedir, "closedir", int(*)(DIR*));
  int ret = (*old_closedir)(dir);
  fprintf(out,"[monitor] closedir(%p) = %d\n", dir, ret);
  return ret;
}


DIR* fdopendir(int fd){
  INJECT_SYMBOL("libc.so.6", old_fdopendir, "fdopendir", DIR*(*)(int));
  DIR* ret = old_fdopendir(fd);
  fprintf(out,"[monitor] fdopendir(%d) = %p\n", fd, ret);
  return ret;
}

DIR* opendir(const char* path){
  INJECT_SYMBOL("libc.so.6", old_opendir, "opendir", DIR*(*)(const char*));
  DIR* ret = old_opendir(path);
  fprintf(out,"[monitor] opendir('%s') = %p\n", path, ret);
  return ret;
}

struct dirent* readdir(DIR* dir){
  INJECT_SYMBOL("libc.so.6", old_readdir, "readdir", struct dirent*(*)(DIR*));
  struct dirent* ret = old_readdir(dir);
  fprintf(out,"[monitor] readdir(%p) = %p\n", dir, ret);
  return ret;
}

int readdir_r(DIR* dir, struct dirent* ptr, struct dirent** d_ptr){
  INJECT_SYMBOL("libc.so.6", old_readdir_r, "readdir_r", int(*)(DIR*, struct dirent*, struct dirent**));
  int ret = old_readdir_r(dir, ptr, d_ptr);
  fprintf(out,"[monitor] readdir_r(%p, %p, %p) = %d\n", dir, ptr, d_ptr, ret);
  return ret;
}

void rewinddir(DIR* dir){
  INJECT_SYMBOL("libc.so.6", old_rewinddir, "rewinddir", void(*)(DIR*));
  old_rewinddir(dir);
  fprintf(out, "[monitor] rewinddir(%p)\n", dir);
  return ;
}

void seekdir(DIR* dir, long num){
  INJECT_SYMBOL("libc.so.6", old_seekdir, "seekdir", void(*)(DIR*,long));
  old_seekdir(dir, num);
  fprintf(out, "[monitor] seekdir(%p, %ld)\n", dir, num);
  return ;
}

long telldir(DIR* dir){
  INJECT_SYMBOL("libc.so.6", old_telldir, "telldir", long(*)(DIR*));
  long ret = old_telldir(dir);
  fprintf(out, "[monitor] telldir(%p) = %ld\n", dir, ret);
  return ret;
}

/* fcntl.h injection implementation*/
int creat(const char* name, mode_t mode){
  INJECT_SYMBOL("libc.so.6", old_creat, "creat", int(*)(const char*, mode_t));
  int ret = old_creat(name, mode);
  fprintf(out, "[monitor] creat('%s', %d) = %d\n", name, mode, ret);
  return ret;
}

int open(const char* path, int oflag, ...){
  INJECT_SYMBOL("libc.so.6", old_open, "open", int(*)(const char*, int, ...));
  int ret;
  if (oflag & O_CREAT){
    va_list ap;
    va_start(ap, oflag);
    mode_t mode = va_arg(ap, mode_t);
    ret = old_open(path, oflag, mode);
    fprintf(out, "[monitor] open('%s', %d, %d) = %d\n", path, oflag, mode, ret);
  }
  else{
    ret = old_open(path, oflag);
    fprintf(out, "[monitor] open('%s', %d) = %d\n", path, oflag, ret);
  }
  return ret;
}


/* stdio.h injection implementation */
int remove(const char* path){
  INJECT_SYMBOL("libc.so.6", old_remove, "remove", int(*)(const char*));
  int ret = old_remove(path);
  fprintf(out, "[monitor] remove('%s') = %d\n", path, ret);
  return ret;
}

int rename(const char* old, const char* new_name){
  INJECT_SYMBOL("libc.so.6", old_rename, "rename", int(*)(const char*, const char*));
  int ret = old_rename(old, new_name);
  fprintf(out, "[monitor] rename('%s', '%s') = %d\n", old, new_name, ret);
  return ret;
}

void setbuf(FILE* file, char* buf){
  INJECT_SYMBOL("libc.so.6", old_setbuf, "setbuf", void(*)(FILE*, char*));
  old_setbuf(file, buf);
  fprintf(out, "[monitor] setbuf(%p, '%s')\n", file, buf);
  return ;
}
int setvbuf(FILE* file, char* buf, int flag, size_t size){
  INJECT_SYMBOL("libc.so.6", old_setvbuf, "setvbuf", int(*)(FILE*, char*, int, size_t));
  int ret = old_setvbuf(file, buf, flag, size);
  fprintf(out, "[monitor] setvbuf(%p, '%s', %d, %ld) = %d\n", file, buf, flag, size, ret);
  return ret;
}

char* tempnam(const char* name1, const char* name2){
  INJECT_SYMBOL("libc.so.6", old_tempnam, "tempnam", char*(*)(const char*, const char*));
  char* ret = old_tempnam(name1, name2);
  fprintf(out, "[monitor] tempnam('%s', '%s') = '%s'\n", name1, name2, ret == NULL ? NULL_STRING : ret);
  return ret;
}
FILE* tmpfile(void){
  INJECT_SYMBOL("libc.so.6", old_tmpfile, "tmpfile", FILE*(*)(void));
  FILE* ret = old_tmpfile();
  fprintf(out, "[monitor] tmpfile() = %p\n", ret);
  return ret;
}

char* tmpnam(char* name){
  INJECT_SYMBOL("libc.so.6", old_tmpnam, "tmpnam", char*(*)(char*));
  char* ret = old_tmpnam(name);
  fprintf(out, "[monitor] tmpnam('%s') = '%s'\n", name, ret == NULL ? NULL_STRING : ret);
  return ret;
}

/* stdlib.h injection implementaton */
void exit(int status){
  INJECT_SYMBOL("libc.so.6", old_exit, "exit", void(*)(int));
  fprintf(out, "[monitor] exit(%d)\n", status);
  old_exit(status);
}

char* getenv(const char* var){
  INJECT_SYMBOL("libc.so.6", old_getenv, "getenv", char*(*)(const char*));
  char* ret = old_getenv(var);
  fprintf(out, "[monitor] getenv('%s') = '%s'\n", var == NULL ? NULL_STRING : var, ret == NULL ? NULL_STRING : ret);
  return ret;
}

char* mkdtemp(char* path){
  INJECT_SYMBOL("libc.so.6", old_mkdtemp, "mkdtemp", char*(*)(char*));
  char* ret = old_mkdtemp(path);
  fprintf(out, "[monitor] mkdtemp('%s') = '%s'\n", path, ret == NULL ? NULL_STRING : ret);
  return ret;
}

int mkstemp(char* path){
  INJECT_SYMBOL("libc.so.6", old_mkstemp, "mkstemp", int(*)(char*));
  int ret = old_mkstemp(path);
  fprintf(out, "[monitor] mkstemp('%s') = %d\n", path, ret);
  return ret;
}

int putenv(char* str){
  INJECT_SYMBOL("libc.so.6", old_putenv, "putenv", int(*)(char*));
  int ret = old_putenv(str);
  fprintf(out, "[monitor] putenv('%s') = %d\n", str, ret);
  return ret;
}

int rand(void){
  INJECT_SYMBOL("libc.so.6", old_rand, "rand", int(*)(void));
  int ret = old_rand();
  fprintf(out, "[monitor] rand() = %d\n", ret);
  return ret;
}

int rand_r(unsigned* var){
  INJECT_SYMBOL("libc.so.6", old_rand_r, "rand_r", int(*)(unsigned*));
  int ret = old_rand_r(var);
  fprintf(out, "[monitor] rand_r('%p') = %d\n", var, ret);
  return ret;
}

int setenv(const char* env, const char* val, int flag){
  INJECT_SYMBOL("libc.so.6", old_setenv, "setenv", int(*)(const char*, const char*, int));
  int ret = old_setenv(env, val, flag);
  fprintf(out, "[monitor] setenv('%s', '%s', %d) = %d\n", env, val, flag, ret);
  return ret;
}

void srand(unsigned seed){
  INJECT_SYMBOL("libc.so.6", old_srand, "srand", void(*)(unsigned));
  old_srand(seed);
  fprintf(out, "[monitor] srand(%u)\n", seed);
  return ;

}
int system(const char* cmd){
  INJECT_SYMBOL("libc.so.6", old_system, "system", int(*)(const char*));
  int ret = old_system(cmd);
  fprintf(out, "[monitor] system('%s') = %d\n", cmd, ret);
  return ret;

}

/* unistd.h injection implementation */
uid_t getuid(void){
  INJECT_SYMBOL("libc.so.6", old_getuid, "getuid", uid_t(*)(void));
  uid_t ret = old_getuid();
  fprintf(out, "[monitor] getuid() = %d\n", ret);
  return ret;
}

/* sys_stat.h injection implementation */
int chmod(const char* path, mode_t mode){
  INJECT_SYMBOL("libc.so.6", old_chmod, "chmod", int(*)(const char*, mode_t));
  int ret = old_chmod(path, mode);
  fprintf(out, "[monitor] chmod('%s', %d) = %d\n", path, mode, ret);
  return ret;
}

int fchmod(int n, mode_t mode){
  INJECT_SYMBOL("libc.so.6", old_fchmod, "fchmod", int(*)(int, mode_t));
  int ret = old_fchmod(n, mode);
  fprintf(out, "[monitor] fchmod(%d, %u) = %d\n", n, mode, ret);
  return ret;
}

int fstat(int fd, struct stat* st){
  INJECT_SYMBOL("libc.so.6", old_fstat, "__fxstat", int(*)(int, struct stat*));
  int ret = old_fstat(fd, st);
  fprintf(out, "[monitor] __fxstat(%d, %p) = %d\n", fd, st, ret);
  return ret;
}

int lstat(const char* path, struct stat* st){
  INJECT_SYMBOL("libc.so.6", old_lstat, "__lxstat", int(*)(const char*, struct stat*));
  int ret = old_lstat(path, st);
  fprintf(out, "[monitor] __lxstat('%s', %p) = %d\n", path, st, ret);
  return ret;
}

int mkdir(const char* name, mode_t mode){
  INJECT_SYMBOL("libc.so.6", old_mkdir, "mkdir", int(*)(const char*, mode_t));
  int ret = old_mkdir(name, mode);
  fprintf(out, "[monitor] mkdir('%s', %d) = %d\n", name, mode, ret);
  return ret;
}

int mkfifo(const char* name, mode_t mode){
  INJECT_SYMBOL("libc.so.6", old_mkfifo, "mkfifo", int(*)(const char*, mode_t));
  int ret = old_mkfifo(name, mode);
  fprintf(out, "[monitor] mkfifo('%s', %d) = %d\n", name, mode, ret);
  return ret;
}

int stat(const char* path, struct stat* st){
  INJECT_SYMBOL("libc.so.6", old_stat, "__xstat", int(*)(const char*, struct stat*));
  int ret = old_stat(path, st);
  fprintf(out, "[monitor] __xstat('%s', %p) = %d\n", path, st, ret);
  return ret;
}

mode_t umask(mode_t mode){
  INJECT_SYMBOL("libc.so.6", old_umask, "umask", mode_t(*)(mode_t));
  int ret = old_umask(mode);
  fprintf(out, "[monitor] umask(%d) = %d\n", mode, ret);
  return ret;
}

