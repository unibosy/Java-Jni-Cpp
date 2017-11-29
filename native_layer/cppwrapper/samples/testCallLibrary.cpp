#include "cppwrapper.h"
#include <stdio.h>

//int test(int argc, char * const argv[]);
int main()
{
	int argc = 13;
  char* const argv[] = {"./recorder_local","--recordFileRootDir","./","--channel","video","--uid","0","--appliteDir","/home/wangshiwei/ars/code_ars/javaDemo/native_layer/cppwrapper/bin","--idle","20","--appId"
		,"0c0b4b61adf94de1befd7cdd78a50444"};
	totalFunc(argc, argv);
	return 0;	
}
