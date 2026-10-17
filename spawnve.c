/*
 * PROJECT C Library, X68000 PROGRAMMING INTERFACE DEFINITION
 * --------------------------------------------------------------------
 * This file is written by the Project C Library Group,  and completely
 * in public domain. You can freely use, copy, modify, and redistribute
 * the whole contents, without this notice.
 * --------------------------------------------------------------------
 * based on the contribution from hupairlib by K.Abe (Abechan).
 * --------------------------------------------------------------------
 * $Id: spawnve.c,v 1.2 1993/09/16 15:30:02 mura Exp $
 */

/* System headers */
#include <unistd.h>
/* #include <sys/xunistd.h> */
#include "x68spawn.h"

/* Functions */
int spawnve (int mode, const char *path, char *const argv[], char *const envp[])
{
    /* 実行する */
    return _spawn (mode, path, argv, envp, 0);
}
