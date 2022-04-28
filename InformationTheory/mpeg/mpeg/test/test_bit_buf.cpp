#include <cstdio>
#include <bit_buf.h>
int main(){
	FILE *file = fopen("testfile","r");
	bit_buf OAO(file);
	uint32_t tmp;
	tmp = OAO.get(3);
	//printf("%x",tmp);
	OAO.next_start_code();
	for(int i=0;i<32;i++){
		tmp = OAO.get(4);
		//printf("%x\n",tmp);
	}
	tmp = OAO.get(7);
	//printf("%x",tmp);
	OAO.next_start_code();
	
	for(int i=32;i>=1;i--){
		tmp = OAO.get(4);
		//printf("%x\n",tmp);
	}
	/*
	tmp = OAO.get(32);
	for(int i=32;i>=1;i--){
		tmp = OAO.nextbits(i);
		printf("%d %x\n",tmp,tmp);
	}
	*/
	
	
}
