#include <cstdio>
#include <iostream>
#include "ga.h"
#include "baseTM.h"

using namespace std;

MachineMgr* macMgr= new MachineMgr();
extern void initbrainfuck();
//extern void initbrainplus();

int main(int argc,char* argv[]){

/*
	//hello
	//str="+-+-+>-<[++++>+++++<+<>++]>[-[---.--[[-.++++[+++..].]]]]";
	//I love all humans
	//str="+[>+<+++]+>------------.+<+++++++++++++++++++++++++++++++.>++++++++++++++++++++++++++++++++++.+++.+++++++.-----------------.--<.>--.+++++++++++..---<.>-.+++++++++++++.--------.------------.+++++++++++++.+++++.";

	//TuringMachine* acMachine;
	//acMachine= new brainFuck("+[>+<+++]+>------------.+<+++++++++++++++++++++++++++++++.>++++++++++++++++++++++++++++++++++.+++.+++++++.-----------------.--<.>--.+++++++++++..---<.>-.+++++++++++++.--------.------------.+++++++++++++.+++++.");
	//printf("\nmachine read.\n");
	//acMachine->run();
	//acMachine->print();
	//printf("\nfinish.\n");
*/
	int repeat=1;
	int count=0;
	int genCount=0;

	initbrainfuck();
//	initbrainplus();

	printf("GA init\n");
	printf("repeat %d time(s) \n",repeat);
	printf("-------------------------\n");
	string target("add");
	for(int i=0;i<repeat;i++){
		printf("#%d \n",i+1);
		//population generation length maxL machine s pc pm
    string s = "brainFuck";
    srand(time(NULL));
		GA ga(100,1000000,150,150,s,2,0.7,0.01);
		//GA ga(100,10000,150,150,"brainPlus",2,0.7,0.01);

		ga.setTarget(target);

		if(ga.runGA()){
			count++;
			genCount+=ga.currentGen();
		}
		printf("----------\n");
		target[1]=int(target[1])+1;
		if(target[1]>122){target[0]=int(target[0])+1;target[1]='a';}
	}
	printf("average gen of success %lf\n",(double)genCount/count);
	printf("number of failure %d \n",repeat-count);


	return 0;
}
