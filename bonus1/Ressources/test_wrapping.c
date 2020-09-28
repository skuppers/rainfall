#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
	int32_t result;
	if (argc == 1)
	{
		printf("Pass an argument");
		return 0;
	}
	result = atoi(argv[1]);
	printf("Wraping of %s is to int is %d then to unsigned is %u\n", argv[1], result, result * 4);
	return 0;
}



