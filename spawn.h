/*
 * PROJECT C Library, X68000 PROGRAMMING INTERFACE DEFINITION
 * --------------------------------------------------------------------
 * This file is written by the Project C Library Group,  and completely
 * in public domain. You can freely use, copy, modify, and redistribute
 * the whole contents, without this notice.
 * --------------------------------------------------------------------
 * $Id: unistd.h,v 1.14 1994/11/26 14:48:52 mura Exp $
 */

#ifndef __spawn_h__
#define __spawn_h__

#define __HUPAIRLIB

#ifndef __sys_types_h__
#include <sys/types.h>
#endif

#ifndef NULL
#define NULL ((void *) 0)
#endif

#define P_WAIT    0
#define P_OVERLAY 1

#define _HUGEARG_DEFAULT  0
#define _HUGEARG_FORCE    1
#define _HUGEARG_NOEXEC   2
#define _HUGEARG_INDIRECT 3

#define execvpe  execve  /* for compatibility */
#define spawnvpe spawnve /* for compatibility */

extern char **environ;
extern char *_hugearg_indirect;
extern int _fmode;
extern int _hugearg;
extern int _keep_cwd_on_exec;
extern mode_t _umask;

#if 0
extern int execl (const char *, ... );
extern int execle (const char *, ... /* , char *const[] */);
extern int execlp (const char *, ... );
extern int execv (const char *, char *const[]);
extern int execve (const char *, char *const[], char *const[]);
extern int execvp (const char *, char *const[]);
#endif
extern int spawnl (int, const char *, ... );
extern int spawnle (int, const char *, ... /* , char *const[] */);
extern int spawnlp (int, const char *, ... );
extern int spawnv (int, const char *, char *const[]);
extern int spawnve (int, const char *, char *const[], char *const[]);
extern int spawnvp (int, const char *, char *const[]);

#endif
