#include <stdio.h>
#include <syscall.h>

int main(int argc, char **argv)
{
	int i, j;

	for(int i=0; i < argc; i++)
		printf("%s", argv[i]);
	printf("\nnumber of argv's: %d", argc);
	printf("\n");

	return EXIT_SUCCESS;

}
