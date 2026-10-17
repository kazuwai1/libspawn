#include <stdio.h>
#include <string.h>

void main (int argc, char *argv[], char *envp) {
	printf("strlen(argv[1]) = %d\n", strlen( argv[1] ) );
	printf("argv[1] = %s\n", argv[1]);
}
