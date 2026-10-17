/*
 * PROJECT C Library, X68000 PROGRAMMING INTERFACE DEFINITION
 * --------------------------------------------------------------------
 * This file is written by the Project C Library Group,  and completely
 * in public domain. You can freely use, copy, modify, and redistribute
 * the whole contents, without this notice.
 * --------------------------------------------------------------------
 * based on the contribution from hupairlib by K.Abe (Abechan).
 * --------------------------------------------------------------------
 * $Id: spawnle.c,v 1.2 1993/09/16 15:29:47 mura Exp $
 */

/* System headers */
#include <stdarg.h>
#include <unistd.h>
/* #include <sys/xunistd.h> */
#include "x68spawn.h"

/* Functions */
int spawnle (int mode, const char *path, const char *arg0, ...)
{
    va_list vp;
    char *argn, **envp;

    /* envp を取り出す */
    va_start (vp, arg0);
    do { argn = va_arg (vp, char *); } while (argn);
    envp = va_arg (vp, char **);
    va_end (vp);

    /* 実行する */
    return _spawn (mode, path, (char **) &arg0, (char **) envp, 0);
}
