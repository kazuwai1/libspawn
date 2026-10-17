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

#define F_OK 0x00
#define X_OK 0x01
#define W_OK 0x02
#define R_OK 0x04

#ifndef NULL
#define NULL ((void *) 0)
#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif

#ifndef SEEK_END
#define SEEK_END 2
#endif

#define _POSIX_CHOWN_RESTRICTED 1
#define _POSIX_JOB_CONTROL	0
#define _POSIX_NO_TRUNC 	1
#define _POSIX_SAVED_IDS	0
#define _POSIX_VDISABLE 	0

#define _POSIX_VERSION	     199008L
#define _XOPEN_VERSION	     3
#define _AES_OS_VERSION	     1
#define _PROJ_VERSION	     0x010132

#define _SC_ARG_MAX	     1
#define _SC_CHILD_MAX	     2
#define _SC_CLK_TCK	     3
#define _SC_JOB_CONTROL      4
#define _SC_NGROUPS_MAX      5
#define _SC_OPEN_MAX	     6
#define _SC_SAVED_IDS	     7
#define _SC_VERSION	     8
#define _SC_PASS_MAX	     9
#define _SC_XOPEN_VERSION    10
#define _SC_ATEXIT_MAX	     11
#define _SC_PAGE_SIZE	     12
#define _SC_AES_OS_VERSION   13
#define _SC_PROJ_VERSION     14

#define _PC_CHOWN_RESTRICTED 1
#define _PC_LINK_MAX	     2
#define _PC_MAX_CANON	     3
#define _PC_MAX_INPUT	     4
#define _PC_NAME_MAX	     5
#define _PC_NO_TRUNC	     6
#define _PC_PATH_MAX	     7
#define _PC_PIPE_BUF	     8
#define _PC_VDISABLE	     9

#define STDIN_FILENO	     0
#define STDOUT_FILENO	     1
#define STDERR_FILENO	     2
#define STDAUX_FILENO	     3
#define STDPRN_FILENO	     4

#if !defined (STDIN) && !defined (STDOUT)
#define STDIN  STDIN_FILENO  /* for compatibility */
#define STDOUT STDOUT_FILENO /* for compatibility */
#define STDERR STDERR_FILENO /* for compatibility */
#define STDAUX STDAUX_FILENO /* for compatibility */
#define STDPRN STDPRN_FILENO /* for compatibility */
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
