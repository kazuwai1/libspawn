/*
 * PROJECT C Library, X68000 PROGRAMMING INTERFACE DEFINITION
 * --------------------------------------------------------------------
 * This file is written by the Project C Library Group,  and completely
 * in public domain. You can freely use, copy, modify, and redistribute
 * the whole contents, without this notice.
 * --------------------------------------------------------------------
 * based on the contribution from hupairlib by K.Abe (Abechan).
 * --------------------------------------------------------------------
 * $Id: _spawn.c,v 1.5 1994/11/27 13:10:08 mura Exp $
 */

/* System headers */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#if 1
#include <x68k/dos.h>
#include "x68spawn.h"
#else
#include <sys/dos.h>
#endif
/* #include <sys/xglob.h> */
/*#include <sys/xsignal.h> */
/* #include <sys/xstart.h> */
/* #include <sys/xunistd.h> */

/* Macros */
#define INITIAL_BUFSIZ 1024
#define STDFDS 5

/* External variables */
int _hugearg = _HUGEARG_DEFAULT;
int _keep_cwd_on_exec = 1;
char *_hugearg_indirect;

/* File scope variables */
static long nullenv[2] = {5, 0};

/* break cehck save */
static int obreak;

/* Structures and unions */
enum exec_mode {
    EXEC_LOADEXEC = 0,
    EXEC_LOAD,
    EXEC_PATHCHK,
    EXEC_LOADONLY,
    EXEC_EXECONLY,
    EXEC_BINDNO,
};

// 構造体の定義
struct _dregs {
    int d0;
    int d1;
    int d2;
    int d3;
    int d4;
    int d5;
    int d6;
    int d7;
    int a0;
    int a1;
    int a2;
    int a3;
    int a4;
    int a5;
    int a6;
};

#define DOSCALL_EXEC 0xFF4B

void _noexec_return(struct _dregs *regs)
{
    /* 
     * GCCの最適化による変数のレジスタ退避と競合しないよう、
     * 必要なすべての汎用レジスタ(d0-d7/a0-a5)の値を明示的に
     * インラインアセンブラ内で一度退避し、復元します。
     */
    __asm__ __volatile__ (
        "movem.l %%d0-%%d7/%%a0-%%a5, -(%%sp)\n\t" /* 現コンテキストの全レジスタを退避 */
        "move.l  %0, %%a0\n\t"                     /* 引数 regs のポインタを a0 にロード */
        "move.l  32(%%a0), %%a0\n\t"               /* regs->a0 (子プロセスのPDBアドレス) を a0 にロード */
        "lea     1f, %%a1\n\t"                     /* 下記のローカルラベル '1:' のアドレスを取得 */
        "move.l  %%a1, 0x14(%%a0)\n\t"             /* PDBの復帰アドレスを '1:' に書き換える */
        "move.l  %%sp, 0x3c(%%a0)\n\t"             /* PDBの復帰スタックポインタを現在のSPに書き換える */
        "clr.w   -(%%sp)\n\t"                      /* _EXIT2 の引数（終了コード 0）をスタックに積む */
        "move.w  #0xff, -(%%sp)\n\t"               /* _EXIT2 の機能コード 0xff をスタックに積む */
        "trap    #2\n\t"                           /* Human68k DOSコール実行 (子プロセスを破棄) */
        
        /* 
         * DOSコール発行後、OSによってスタックが巻き戻され、
         * PDBに仕込んだこの下のラベル '1:' へ直接ジャンプして戻ってきます。
         */
        "1:\n\t"
        "movem.l (%%sp)+, %%d0-%%d7/%%a0-%%a5\n\t" /* 退避していたレジスタをすべて復元 */
        :
        : "g" (regs)                               /* 入力オペランド %0: regsポインタ */
        : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", 
          "a0", "a1", "a2", "a3", "a4", "a5", "memory"
    );
}

// 呼び出し側の型(int/long)に合わせて引数の宣言を変更
int _exec(short int mode, int fname, int comptr, int envptr, struct _dregs *regptr) {
    // 1. 各引数をDOSコール仕様に合わせて特定のレジスタへ強制配置
    register int       r_mode   __asm__("d1") = mode;
    register int       r_fname  __asm__("a0") = fname;
    register int       r_comptr __asm__("a1") = comptr;
    register int       r_envptr __asm__("a2") = envptr;
    
    // DOSコールの戻り値（d0, a0-a4）と一切衝突しない「d7」に構造体ポインタを配置
    register struct _dregs *r_regptr __asm__("d7") = regptr;
    int d0_result;

    __asm__ __volatile__ (
        // 2. 親プロセスの保存義務レジスタ（d3-d7, a3-a6）を手動でスタックへ完全退避
        // ※このとき、事前に d7 に入っていた regptr の値も一緒にスタック（16(sp)の位置）に保存されます
        "movem.l %%d3-%%d7/%%a3-%%a6, -(%%sp)\n\t"
        
        // 3. 引数をスタックに積む (Human68k DOSコールの引数仕様)
        "move.l  %%a2, -(%%sp)\n\t"    // envptr
        "move.l  %%a1, -(%%sp)\n\t"    // comptr
        "move.l  %%a0, -(%%sp)\n\t"    // fname
        "move.w  %%d1, -(%%sp)\n\t"    // mode
        
        // 4. Human68k DOSコール実行 (_EXEC)
        // 戻り値として d0, a0, a1, a2, a3, a4 に重要データが載って戻ってくる場合があります（d7は破壊される場合在り）
        "dc.w    0xFF4B\n\t"
        
        // 5. 引数用のスタックポインタを復元 (14バイト)
        "lea     14(%%sp), %%sp\n\t"
        
        // 6. C言語への戻り値 (d0) を確定
        "move.l  %%d0, %0\n\t"
        
        // 7. 構造体へのレジスタ一括保存処理
        // スタック上の「退避されたd7」の位置（現在のspから16バイト上）から、
        // 事前に仕込んでおいた regptr のアドレスを安全に回収します。
        // ※d3(0), d4(4), d5(8), d6(12), d7(16) なので 16(sp) です。
        // これにより、a3の戻り値（環境ポインタ）を1ミリも汚さずに構造体保存へ移行できます。
        "move.l  16(%%sp), %%a5\n\t"
        "movem.l %%d0-%%d7/%%a0-%%a6, (%%a5)\n\t"
        
        // 8. 退避していた親プロセスの全レジスタを完全に復元
        "movem.l (%%sp)+, %%d3-%%d7/%%a3-%%a6\n\t"

        : "=r" (d0_result)
        : "r" (r_regptr), 
          "r" (r_mode), "r" (r_fname), "r" (r_comptr), "r" (r_envptr) 
        : "d0", "memory" 
    );

    return d0_result;
}

#define _MALPHA 0x01
#define _MBLANK 0x02
#define _MDIGIT 0x04
#define _MKMOJI 0x08
#define _MKPNCT 0x10
#define _MLEAD  0x20
#define _MPUNCT 0x40
#define _MTRAIL 0x80

#define _MBALNUM (_MALPHA | _MDIGIT | _MKPNCT | _MKMOJI)
#define _MBALPHA (_MALPHA | _MKPNCT | _MKMOJI)
#define _MBGRAPH (_MALPHA | _MDIGIT | _MPUNCT | _MKPNCT | _MKMOJI)
#define _MBKANA  (_MKPNCT | _MKMOJI)
#define _MBPRINT (_MALPHA | _MDIGIT | _MPUNCT | _MBLANK | _MKPNCT | _MKMOJI)
#define _MBPUNCT (_MPUNCT | _MKPNCT)

#define _MBLMASK(c) ((c) &  255)
#define _MBHMASK(c) ((c) & ~255)
#define _MBGETL(c)  ((c) &  255)
#define _MBGETH(c)  (((c) >> 8) & 255)

/* Macros */
#define B _MBLANK
#define D _MDIGIT
#define P _MPUNCT
#define T _MTRAIL

/* Macros */
#define AT (_MALPHA | _MTRAIL)
#define GT (_MKPNCT | _MTRAIL)
#define KT (_MKMOJI | _MTRAIL)
#define LT (_MLEAD  | _MTRAIL)
#define PT (_MPUNCT | _MTRAIL)

/* File scope variables */
/* static __common const unsigned char _lc_C_mbctype[257] = { */
static const unsigned char _lc_C_mbctype[257] = {
    0,						     /* EOF */
    0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 , /* 0 */
    0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 , /* 1 */
    B ,P ,P ,P ,P ,P ,P ,P ,P ,P ,P ,P ,P ,P ,P ,P , /* 2 */
    D ,D ,D ,P ,D ,D ,D ,D ,D ,D ,P ,P ,P ,P ,P ,P , /* 3 */
    PT,AT,AT,AT,AT,AT,AT,AT,AT,AT,AT,AT,AT,AT,AT,AT, /* 4 */
    AT,AT,AT,AT,AT,AT,AT,AT,AT,AT,AT,PT,PT,PT,PT,PT, /* 5 */
    PT,AT,AT,AT,AT,AT,AT,AT,AT,AT,AT,AT,AT,AT,AT,AT, /* 6 */
    AT,AT,AT,AT,AT,AT,AT,AT,AT,AT,AT,PT,PT,PT,PT,0 , /* 7 */
    T ,LT,LT,LT,LT,LT,LT,LT,LT,LT,LT,LT,LT,LT,LT,LT, /* 8 */
    LT,LT,LT,LT,LT,LT,LT,LT,LT,LT,LT,LT,LT,LT,LT,LT, /* 9 */
    T ,GT,GT,GT,GT,GT,KT,KT,KT,KT,KT,KT,KT,KT,KT,KT, /* A */
    KT,KT,KT,KT,KT,KT,KT,KT,KT,KT,KT,KT,KT,KT,KT,KT, /* B */
    KT,KT,KT,KT,KT,KT,KT,KT,KT,KT,KT,KT,KT,KT,KT,KT, /* C */
    KT,KT,KT,KT,KT,KT,KT,KT,KT,KT,KT,KT,KT,KT,GT,GT, /* D */
    LT,LT,LT,LT,LT,LT,LT,LT,LT,LT,LT,LT,LT,LT,LT,LT, /* E */
    LT,LT,LT,LT,LT,LT,LT,LT,LT,LT,LT,LT,LT,0 ,0 ,0 ,  /* F */
};

/* External variables */
/* __common const unsigned char *_mbctype = &_lc_C_mbctype[1]; */
const unsigned char *_mbctype = &_lc_C_mbctype[1];

/* Functions */
char *_tobslash (char *path)
{
    char *p;

    /* path を一文字づつ調べる */
    for (p = path; *p; p++) {

	/* スラッシュを変換する */
	if (*p == '/')
	    *p = '\\';

    }

    /* path を返す */
    return path;
}

/* Functions */
int ismbblead (int c)
{
    /* SJIS 1 バイト目 */
    return (_mbctype[c & 0xff] & _MLEAD);
}

/* Functions */
char *_addlastsep (char *path)
{
    char *p, *q;

    /* path を parse する */
    for (p = q = path; *p; p++) {

	/* 現在位置をマーク */
	q = p;

	/* 有効な漢字ならば 2byte スキップ */
	if (ismbblead (*p) && p[1])
	    p++;

    }

    /* 最後が / や \ でなければ / を追加 */
    if (*q != '/' && *q != '\\') {
	p[0] = '/';
	p[1] = '\0';
    }

    /* path を返す */
    return path;
}

/* File scope variables */
static int _doserr_conv[_DOSE_MAX + 1] = {
    0,			/*   0 エラーではない -> 0 */
    ENOENT,		/*  -1 無効なファンクション -> ENOENT */
    ENOENT,		/*  -2 ファイルが見付かりません -> ENOENT */
    ENOENT,		/*  -3 ディレクトリが見付かりません -> ENOENT */
    EMFILE,		/*  -4 オープンファイルが多すぎる -> EMFILE */
    EISDIR,		/*  -5 ディレクトリにである -> EISDIR */
    EBADF,		/*  -6 ハンドルは無効 -> EBADF */
    EFAULT,		/*  -7 メモリ管理領域破壊 -> ENOMEM */
    ENOMEM,		/*  -8 実行にメモリがたりない -> ENOMEM */
    EFAULT,		/*  -9 不正なメモリ管理ポインタ -> EFAULT */
    EFAULT,		/* -10 不正な環境 -> EFAULT */
    ENOEXEC,		/* -11 実行フォーマットエラー -> ENOEXEC */
    EINVAL,		/* -12 不正な引数 -> EINVAL */
    ENOENT,		/* -13 不正なファイル名 -> ENOENT */
    EINVAL,		/* -14 不正なパラメータ -> EINVAL */
    EXDEV,		/* -15 不正なドライブ -> EXDEV */
    ENOTEMPTY,		/* -16 カレントは削除できない -> ENOTEMPTY */
    EPERM,		/* -17 ioctlできないデバイス -> EPERM */
    ENOENT,		/* -18 これ以上見付からない -> ENOENT */
    EACCES,		/* -19 書き込みできないファイル -> EACCES */
    EEXIST,		/* -20 すでに登録されたディレクトリ -> EEXIST */
    ENOTEMPTY,		/* -21 ディレクトリが空でない -> ENOTEMPTY */
    EPERM,		/* -22 リネームできない -> EPERM */
    ENOSPC,		/* -23 ディスクが一杯 -> ENOSPC */
    ENOSPC,		/* -24 ディレクトリが一杯 -> ENOSPC */
    ESPIPE,		/* -25 シークできない -> ESPIPE */
    EPERM,		/* -26 多重 supervisor -> EPERM */
    ENOSYS,		/* -27 未定義 ..... -> ENOSYS */
    ETXTBSY,		/* -28 同名のスレッドが存在する -> ETXTBSY */
    EIO,		/* -29 メッセージが受け取れませんでした -> EIO */
    ESRCH,		/* -30 スレッド番号が異常です -> ESRCH */
    ENOSYS,		/* -31 未定義 ..... -> ENOSYS */
    ENFILE,		/* -32 シェアリングファイル数をオーバー -> ENFILE */
    ENOLCK,		/* -33 ロック違反 -> ENOLCK */
    ENOSYS,		/* -34 未定義 ..... -> ENOSYS */
    ELOOP,		/* -35 シンボリックリンクループ -> ELOOP (拡張) */
    EEXIST,		/* -36 ファイルがすでに存在する -> EEXIST (拡張) */
    ERANGE,		/* -37 バッファオーバー -> ERANGE (拡張) */
    ENOSYS,		/* -38 サポートしていない -> ENOSYS (拡張) */
    EEXIST,		/* -39 すでに存在する? -> EEXIST */
};

/* Functions */
int _errcnv (int dos_error)
{
    /* 対応するエラー番号に変換する */
    return (dos_error > 0 || dos_error < -_DOSE_MAX)
	? 0 : _doserr_conv[-dos_error];
}

/* File scope functios */
static int indirect_create (char *arg, char *buffer)
{
    int fd;
    char *dir;
    char *iname;

    /* テンプレート文字列作成 */
    if ((dir = getenv ("temp")) == 0) {
	strcpy (buffer, "#exeXXXXXX");
    }
    else {
	_addlastsep (strcpy (buffer, dir));
	_tobslash (strcat (buffer, "#exeXXXXXX"));
    }

    /* インダイレクトファイル名を作成 */
    if ((int) (iname = mktemp (buffer)) < 0)
	return -1;

    /* インダイレクトファイルを作成 */
    if ((fd = open (iname, O_WRONLY | O_TRUNC | O_CREAT, 0666)) < 0)
	return -1;

    /* 内容を書き込む */
    if (write (fd, arg, strlen (arg)) < 0) {
	close (fd);
	unlink (iname);
	return -1;
    }

    /* ファイルをクローズして終わり */
    close (fd);

    /* 成功 */
    return 0;
}

/* Macros */
#define PUT(c)		\
(void) ({		\
    if (q < max)	\
	*q++ = c;	\
    else		\
	return -1;	\
})

/* Functions */
int isblank (int c)
{
    /* ブランク文字 */
    return (c == ' ' || c == '\t');
}

/* HUPAIR エンコードする際、どんなクオート文字が適切か */
static __inline int decide_quote (const char *arg)
{
    int ch;

    /* 空引き数はクオート */
    if (*arg == 0)
	return '"';

    /* arg を解析する */
    while ((ch = *arg++)) {

	/* " があれば ' が適切 */
	if (ch == '"')
	    return '\'';

	/* ' あるいは空白があれば " が適切 */
	if (ch == '\'' || isblank (ch))
	    return '"';

    }

    /* 特に必要なし */
    return 0;
}

/* Functions */
int _enhupair (const char *argv0,
	       char *const *argv, char *cmdline, int bufsiz)
{
    int c, ret, quote;
    char *q;
    const char *arg;
    const char *max;

    /* バッファ終端を計算する */
    q = cmdline;
    max = q + bufsiz - (strlen (argv0) + 1);

    /* コマンドラインの長さ用の領域 */
    PUT ('\0');

    /* argv 配列をパース */
    while ((arg = *argv++)) {

	/* 適切な「最初の」クオート文字を選択 */
	quote = decide_quote (arg);

	/* クオート文字が必要ならそれを挿入 */
	if (quote)
	    PUT (quote);

	/* 引き数 n をパース */
	while ((c = *arg++)) {

	    /* quote 文字があらわれた場合 */
	    if (c == quote) {

		/* いったんクオートを閉じて... */
		PUT (quote);

		/* クオート文字を切り替える */
		quote = (c == '"') ? '\'' : '"';

		/* クオートを再開する */
		PUT (quote);

	    }

	    /* c を出力 */
	    PUT (c);

	}

	/* クオート文字が必要ならそれを挿入 */
	if (quote)
	    PUT (quote);

	/* 次の引き数があれば間に空白を入れる */
	if (*argv)
	    PUT (' ');

    }

    /* 最後に EOS を打つ */
    PUT ('\0');

    /* その後に argv0 を入れる */
    strcpy (q, argv0);

    /* cmdline - 8 に #HUPAIR 識別氏を */
    strcpy (cmdline - 8, "#HUPAIR");

    /* コマンドラインの長さを計算 */
    ret = q - 2 - cmdline;

    /* 最大 255 までしか設定できない */
    *cmdline = (ret > 255) ? 255 : ret;

    /* バイト数を返す */
    return ret;
}

/* File scope functions */
static int indirect_exec (char *base, char *cmdline,
			  struct _dregs *regptr, const char *argv0)
{
    int rc;
    char iname[PATH_MAX + 1];

    /* インダイレクトファイルを作成する */
    if (indirect_create (cmdline + 1, iname) < 0) {
	_noexec_return (regptr);
	errno = EINVAL;
	return -1;
    }

    /* 実際にコマンドラインに渡す文字列を作成する */
    strcat (strcpy (cmdline + 1, base), iname);
    cmdline[0] = strlen (cmdline + 1) & 255;

    /* 本来の break に */
    _dos_breakck (obreak);

    /* プログラムを実行する */
    rc = _exec (EXEC_EXECONLY, regptr->a4, (long) cmdline, regptr->a3, regptr);

    /* 子が break を変えた時の為 */
    obreak = _dos_breakck (-1);
    _dos_breakck (2);

    /* インダイレクトファイルを削除する */
    unlink (iname);

    /* エラーチェック */
    if (rc < 0) {
	errno = _errcnv (rc);
	return -1;
    }

    /* 成功 */
    return 0;
}

/* File scope functios */
static char *copy_environ (char *const envp[])
{
    int envsize;
    char *dos_envbuf, *dos_envp, *q;
    char *const *ptr;

    /* envp がヌルならば environ を用いる */
    if (envp == 0)
	envp = (char **) environ;

    /* 環境エリアのトータルサイズを求める */
    for (ptr = envp, envsize = 4 + 1; *ptr; ptr++)
	envsize += strlen (*ptr) + 1;

    /* 偶数バンダリに調整する */
    envsize = (envsize + 1) & ~1;

    /* コピー先の領域を確保する */
    if ((dos_envbuf = malloc (envsize)) == 0)
	return 0;

    /* 先頭にバイト数を設定 */
    *(unsigned long *) dos_envbuf = envsize;

    /* 新しい環境エリアにすべてコピーする */
    for (ptr = envp, dos_envp = dos_envbuf + 4; *ptr; ptr++)
	for (q = *ptr; (*dos_envp++ = *q++); );

    /* 最後をヌル文字で止める */
    *dos_envp++ = '\0';

    /* 新しい領域を返す */
    return dos_envbuf;
}

/* Functions */
int _spawn (int mode, const char *pathname,
	    char *const argv[], char *const envp[], int search_path)
{
    char *child_envp;						/* 子プロセスの環境 */
    char *cmdline = 0;						/* コマンドライン */
    char fpath[PATH_MAX + 1];					/* 実行パス名 */
    char cwd[PATH_MAX + 1];					/* カレントディレクトリ */
    const char *argv0;						/* 補正した argv[0] */
    int cmdlen;							/* コマンドラインの長さ */
    int rc = 0;							/* リターンコード */
    int exec_success = 0;					/* */
#if 0
    int std_fds[STDFDS];					/* ファイルハンドル保存用 */
#endif
    struct _dregs regs;						/* 保存レジスタ */

    enum {
	JUST_BEGIN, AFTER_ENVSET, AFTER_ENCODE, AFTER_SIGEND
    } state = JUST_BEGIN;					/* 実行ステートフラグ */

#define RETURN(x) { rc = (x); goto bailout; }

    /*
    ** (1) エラーチェック及び準備
    */
    {
	/* 引数チェック */
	if (mode != P_WAIT && mode != P_OVERLAY) {
	    errno = EINVAL;
	    return -1;
	}

	/* _hugearg チェック */
	if (_hugearg != _HUGEARG_DEFAULT &&
	    _hugearg != _HUGEARG_FORCE &&
	    _hugearg != _HUGEARG_NOEXEC &&
	    _hugearg != _HUGEARG_INDIRECT) {
	    errno = EINVAL;
	    return -1;
	}

	/* 子プロセス用環境エリアを作成する */
	if ((child_envp = copy_environ (envp)) == 0)
	    RETURN (-1);

	/* ステートフラグ設定 */
	state = AFTER_ENVSET;
    }

    /*
    ** (2) 実行ファイル検索.....
    */

    {
	char tmparg[256];					/* テンポラリ */

	/* パス名をすべてスラッシュにする */
	_tobslash (strcpy (fpath, pathname));

	/* パスチェックを行う */
	if ((rc = _exec (EXEC_PATHCHK, (long) fpath, (long) tmparg,
			 (long) (search_path ? 0 : nullenv), &regs)) < 0) {
	    errno = _errcnv (rc);
	    RETURN (-1);
	}
    }

    /*
    ** (3) コマンドラインを HUPAIR エンコードして ARGV を作成する.....
    */

    {
	int bufsiz = INITIAL_BUFSIZ;				/* バッファサイズ */

	/* argv[0] の調整を行う */
	argv0 = argv[0] ? : pathname;

	/* コマンドライン生成用のバッファを確保する */
	if ((cmdline = malloc (bufsiz)) == 0)
	    RETURN (-1);

	/* argv を全部作成するまでループ */
	do {

	    /* コマンドラインを HUPAIR エンコードする */
	    if ((cmdlen = _enhupair (argv0, argv + 1,
				     cmdline + 8, bufsiz - 8)) < 0) {

		/* バッファが足りなければどんどん追加 */
		bufsiz += 1024;

		/* バッファ再割り当てを行う */
		if ((cmdline = realloc (cmdline, bufsiz)) == 0)
		    RETURN (-1);

	    }

	} while (cmdlen < 0);

    }

    /*
    ** (4) チェック用にプログラムをロードする.....
    */

    {
#if 0
	int i;							/* カウンタ */
#endif

	/* ステートフラグ設定 */
	state = AFTER_ENCODE;

#if 0
	/* ファイルハンドルを保存する */
	for (i = 0; i < STDFDS; i++)
	    if ((std_fds[i] = dup (i)) < 0)
		    RETURN (-1);
#endif

	/* 割り込み停止 */
#if 0
	_sigend ();
#endif

	/* 状態遷移 */
	state = AFTER_SIGEND;

	/* break を kill にする */
	obreak = _dos_breakck (-1);
	_dos_breakck (2);

	/* プログラムをロードする */
	rc = _exec (EXEC_LOAD, (long) fpath,
		    (long) cmdline + 8, (long) child_envp, &regs);

	/* エラー処理 */
	if (rc < 0) {
	    errno = _errcnv (rc);
	    RETURN (-1);
	}

	/* カレントディレクトリを保存する */
	if (_keep_cwd_on_exec)
	    getcwd (cwd, sizeof cwd);
    }

    /*
    ** (5) プログラムが HUPAIR かどうかでコマンドラインの渡し方を決める.....
    */

    {
	char *arg;						/* cmdline + 8 */
	char *hugearg;						/* HUGEARG の設定 */
	char *indstr;						/* indirect 文字列 */
	int ishupair;						/* HUPAIR フラグ */

	/* HUPAIR かどうか調べる */
	ishupair = (strcmp ((char *) regs.a4 + 2, "#HUPAIR") == 0);
#if 1
printf("_spawn: ishupair = %d\n", ishupair);
#endif
	/* cmdline + 8 を求める */
	arg = (char *) regs.a2;

	/* HUPAIR あるいは 255 文字以内なら ... */
	if (ishupair || cmdlen <= 255) {

	force:

	    /* 本来の break に */
	    _dos_breakck (obreak);

	    /* プログラムを実行する */
	    rc = _exec (EXEC_EXECONLY, regs.a4, (long) arg, regs.a3, &regs);

	    /* 子が break を変えた時の為 */
	    obreak = _dos_breakck (-1);
	    _dos_breakck (2);

	    /* エラー処理 */
	    if (rc < 0) {
		errno = _errcnv (rc);
		RETURN (-1);
	    }

	}
	else {

	    /* 環境変数 HUGEARG を見る */
	    hugearg = getenv ("HUGEARG");

	    /* _hugearg によって決定 */
	    switch (_hugearg) {

	    case _HUGEARG_FORCE:

		/* 強制的に実行する */
		goto force;

	    case _HUGEARG_NOEXEC:

	    noexec:
		/* 実行失敗にする */
		_noexec_return (&regs);
		errno = E2BIG;
		RETURN (-1);

	    case _HUGEARG_INDIRECT:

		/* インダイレクト文字列が指定されていなければエラー */
		if (_hugearg_indirect == 0) {
		    _noexec_return (&regs);
		    errno = EINVAL;
		    RETURN (-1);
		}

		/* インダイレクト実行 */
		rc = indirect_exec (_hugearg_indirect, cmdline + 8, &regs, argv0);

		/* 次の処理へ */
		break;

	    case _HUGEARG_DEFAULT:

		/* HUGEARG が設定されていなければ実行失敗 */
		if (hugearg == 0)
		    goto noexec;

		/* HUGEARG = force ならば強制実行 */
		if (strcmp (hugearg, "force") == 0)
		    goto force;

		/* HUGEARG = indirect でなければ後はエラー */
		if (strncmp (hugearg, "indirect", 8) != 0)
		    goto noexec;

		/* インダイレクト文字列の取り出し */
		indstr = hugearg[8] ? &hugearg[9] : "-+-+-";

		/* インダイレクト実行 */
		rc = indirect_exec (indstr, cmdline + 8, &regs, argv0);

		/* 次の処理へ */
		break;

	    default:

		/* 該当しない場合はエラー */
		goto noexec;
	    }
	}

	/* 成功した? */
	exec_success = 1;

	/* カレントディレクトリを戻す */
	if (_keep_cwd_on_exec)
	    chdir (cwd);

    }

    /*
    ** (6) 後処理を行う
    */

    {
#if 0
	int i;							/* カウンタ */
#endif

    bailout:

	/* 実行ステートに応じてメモリを解放する */
	switch (state) {

	case AFTER_SIGEND:

	    /* break フラグをに戻す */
	    _dos_breakck (obreak);

	    /* 割り込み再開 */
#if 0
	    _siginit ();
#endif

	case AFTER_ENCODE:

	    /* 資源を解放する */
	    free (cmdline);

        /* ファイルハンドルを戻す */
#if 0
        for (i = 0; i < STDFDS && std_fds[i] >= 0; i++) {
            dup2 (std_fds[i], i);
            close (std_fds[i]);
        }
#endif

	case AFTER_ENVSET:

	    /* 環境を解放する */
	    free (child_envp);

	case JUST_BEGIN:

	    /* 次の処理へ */
	    break;

	}
	}

    /* オーバーレイの処理 */
    if (exec_success && mode == P_OVERLAY)
	exit (rc);

    /* リターンコードを戻す */
    return rc;
}
