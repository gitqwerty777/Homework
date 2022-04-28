#include <cstdio>
#include <vlc.h>


int main(int argv,char* argc[]){
	if(argv != 2){
		puts("input parameter num wrong!!");
		return 0;
	}
	init_vlcs();
}

