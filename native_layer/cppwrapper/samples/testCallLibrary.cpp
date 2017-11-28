#include "cppwrapper.h"
#include <stdio.h>

//int test(int argc, char * const argv[]);
int main()
{
	int argc = 10;
  char* const argv[] = {"1","2","3","4"};
	totalFunc(argc, argv);
	return 0;	
}
