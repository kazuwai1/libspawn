/*
 * PROJECT C Library, X68000 PROGRAMMING INTERFACE DEFINITION
 * --------------------------------------------------------------------
 * This file is written by the Project C Library Group,  and completely
 * in public domain. You can freely use, copy, modify, and redistribute
 * the whole contents, without this notice.
 * --------------------------------------------------------------------
 * $Id: unistd.h,v 1.14 1994/11/26 14:48:52 mura Exp $
 */

#ifndef __unistd_h__
#define __unistd_h__

#ifndef __cdecl_h__
/* #include <cdecl.h> */
#endif

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
extern int_ _fmode;
extern int_ _hugearg;
extern int_ _keep_cwd_on_exec;
extern mode_t _umask;

_EXTERN (char *ctermid (char *));
_EXTERN (char *cuserid (char *));
_EXTERN (char *getcwd (char *, int_));
_EXTERN (char *getdcwd (int_, char *, int_));
_EXTERN (char *getlogin (void_));
_EXTERN (char *getwd (char *));
_EXTERN (char *ttyname (int_));
_EXTERN (gid_t getegid (void_));
_EXTERN (gid_t getgid (void_));
_EXTERN (int_ access (const char *, int_));
_EXTERN (int_ chdir (const char *));
_EXTERN (int_ chdrive (int_));
_EXTERN (int_ chown (const char *, uid_t, gid_t));
_EXTERN (int_ chsize (int_, int_));
_EXTERN (int_ close (int_));
_EXTERN (int_ closeall (void_));
_EXTERN (int_ commit (int_));
_EXTERN (int_ dup (int_));
_EXTERN (int_ dup2 (int_, int_));
_EXTERN (int_ eof (int_));
_EXTERN (int_ execl (const char *, const char * dotdot_));
_EXTERN (int_ execle (const char *, const char * dotdot_ /* , char *const[] */));
_EXTERN (int_ execlp (const char *, const char * dotdot_));
_EXTERN (int_ execv (const char *, char *const[]));
_EXTERN (int_ execve (const char *, char *const[], char *const[]));
_EXTERN (int_ execvp (const char *, char *const[]));
_EXTERN (int_ fchown (int_, uid_t, gid_t));
_EXTERN (int_ ftruncate (int_, off_t));
_EXTERN (int_ getdrive (void_));
_EXTERN (int_ getgroups (int_, gid_t[]));
_EXTERN (int_ getpagesize (void_));
_EXTERN (int_ isatty (int_));
_EXTERN (int_ nice (int_));
_EXTERN (int_ pause (void_));
_EXTERN (int_ read (int_, char *, unsigned int_));
_EXTERN (int_ readlink (const char *, char *, int_));
_EXTERN (int_ rmdir (const char *));
_EXTERN (int_ setgid (gid_t));
_EXTERN (int_ setgroups (int_, gid_t[]));
_EXTERN (int_ setmode (int_, int_));
_EXTERN (int_ setpgid (pid_t, pid_t));
_EXTERN (int_ setuid (uid_t));
_EXTERN (int_ spawnl (int_, const char *, const char * dotdot_));
_EXTERN (int_ spawnle (int_, const char *, const char * dotdot_ /* , char *const[] */));
_EXTERN (int_ spawnlp (int_, const char *, const char * dotdot_));
_EXTERN (int_ spawnv (int_, const char *, char *const[]));
_EXTERN (int_ spawnve (int_, const char *, char *const[], char *const[]));
_EXTERN (int_ spawnvp (int_, const char *, char *const[]));
_EXTERN (int_ symlink (const char *, const char *));
_EXTERN (int_ tcsetpgrp (int_, pid_t));
_EXTERN (int_ truncate (const char *, off_t));
_EXTERN (int_ unlink (const char *));
_EXTERN (int_ write (int_, const char *, unsigned int_));
_EXTERN (long filelength (int_));
_EXTERN (long fpathconf (int_, int_));
_EXTERN (long pathconf (const char *, int_));
_EXTERN (long sysconf (int_));
_EXTERN (long tell (int_));
_EXTERN (off_t lseek (int_, off_t, int_));
_EXTERN (pid_t getpgrp (void_));
_EXTERN (pid_t getpid (void_));
_EXTERN (pid_t getppid (void_));
_EXTERN (pid_t setsid (void_));
_EXTERN (pid_t tcgetpgrp (int_));
_EXTERN (uid_t geteuid (void_));
_EXTERN (uid_t getuid (void_));
_EXTERN (unsigned int_ alarm (unsigned int_));
_EXTERN (unsigned int_ sleep (unsigned int_));
_EXTERN (unsigned int_ usleep (unsigned int_));

#endif
