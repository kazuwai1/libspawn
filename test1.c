/* test program */
#include <stdio.h>
#include "spawn.h"

int main(int argc, char *argv[], char *envp[]) {
	int ret = 0;
	/* test command parameter */
	char *test1argv[] = {
		"c:/usrbin/ls.x",
		"ls.x", 
		"c:/usrbin/",
		NULL,
	};

	char *test2argv[] = {
		"c:/usrbin/ls.x", 
		"-al", 
		"c:/usrbin/", 
		NULL,
	};

	char *test3argv[] = {
		"ls.x", 
		"-al", 
		"c:/usrbin/", 
		NULL,
	};

	char *test4argv[] = {
		"./test2.x", 
		"aabbccddeeffgghhiijjkkllmmnnooppqqrrssttuuvvwwxxyyzz00112233445566778899AABBCCDDEEFFGGHHIIJJKKLLMMNNOOPPQQRRSSTTUUVVZZXXYYZZ99887766554433221100aabbccddeeffgghhiijjkkllmmnnooppqqrrssttuuvvwwxxyyzz00112233445566778899AABBCCDDEEFFGGHHIIJJKKLLMMNNOOPPQQRRSSTTUUVVZZXXYYZZ99887766554433221100aabbccddeeffgghhiijjkkllmmnnooppqqrrssttuuvvwwxxyyzz00112233445566778899AABBCCDDEEFFGGHHIIJJKKLLMMNNOOPPQQRRSSTTUUVVZZXXYYZZ99887766554433221100aabbccddeeffgghhiijjkkllmmnnooppqqrrssttuuvvwwxxyyzz00112233445566778899AABBCCDDEEFFGGHHIIJJKKLLMMNNOOPPQQRRSSTTUUVVZZXXYYZZ99887766554433221100", 
		NULL,
	};

	printf("\ntest1 : spawnl()\n");
	ret = spawnl(P_WAIT, test1argv[0], test1argv[0], NULL);
	printf("ret = %d\n", ret);

	printf("\ntest2 : spawnle()\n");
	ret = spawnle(P_WAIT, test1argv[0], test1argv[0], "test", NULL, envp);
	printf("ret = %d\n", ret);

	printf("\ntest3 : spawnlp()\n");
	ret = spawnlp(P_WAIT, test1argv[1], test1argv[1], NULL);
	printf("ret = %d\n", ret);

	printf("\ntest4 : spawnv()\n");
	ret = spawnv(P_WAIT, test2argv[0], test2argv);

	printf("\ntest5 : spawnve()\n");
	ret = spawnve(P_WAIT, test2argv[0], test2argv, envp);

	printf("\ntest6 : spawnvp()\n");
	ret = spawnvp(P_WAIT, test3argv[0], test3argv);

	printf("\ntest7 : spawnv with hupair\n");
	ret = spawnv(P_WAIT, test4argv[0], test4argv);
	
	return(0);
}
