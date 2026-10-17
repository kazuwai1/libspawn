#define P_WAIT    0
#define P_OVERLAY 1

#define _HUGEARG_DEFAULT  0
#define _HUGEARG_FORCE    1
#define _HUGEARG_NOEXEC   2
#define _HUGEARG_INDIRECT 3

#define execvpe  execve  /* for compatibility */
#define spawnvpe spawnve /* for compatibility */

#define _POSIX_PATH_MAX    127
#define PATH_MAX	   _POSIX_PATH_MAX

int _spawn(int, const char *, char *const [], char *const [], int);
