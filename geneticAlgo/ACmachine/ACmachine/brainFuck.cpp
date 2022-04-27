#include <string>
#include <iostream>
#include "brainFuck.h"

extern MachineMgr* macMgr;

void initbrainfuck(){
	macMgr->regMac("brainFuck",9,new brainFuck());
}

brainFuck::brainFuck(){
	TuringMachine::putInst('<');
	TuringMachine::putInst('>');
	TuringMachine::putInst('+');
	TuringMachine::putInst('-');
	TuringMachine::putInst('.');
	TuringMachine::putInst(',');
	TuringMachine::putInst('[');
	TuringMachine::putInst(']');
}

void brainFuck::specificInit(){}

int brainFuck::instExec(string& instruction, string& tape, int& ptr, int& Iptr, string& input, int& inputIndex, string& output){
	bool error;
		switch(instruction[Iptr]){
			case '>':
				++ptr;
				if(ptr>=1000){
					error=true;
					break;
				}
				break;
			case '<':
				--ptr;
				if(ptr<0){
					//printf("\npointer out of range!\n");
					error=true;
					break;
				}
				break;
			case '+':
				++tape[ptr];
				break;
			case '-':
				--tape[ptr];
				break;
			case '.':
				output.push_back(tape[ptr]);
				//printf("%c",tape[ptr]);
				break;
			case ',':
				//scanf("%c",&tape[ptr]);
				if(inputIndex<(int)input.size())
					tape[ptr]=input[inputIndex++];
				break;
    case '[':
				if(tape[ptr]==0){
					Iptr++;
					int count=1;
					while(true){
						if(instruction[Iptr]=='[')count++;
						if(instruction[Iptr]==']')count--;
						if(count==0)break;
						Iptr++;
						if((unsigned)Iptr>=instruction.size()){
							//printf("\nloop without ] !\n");
							error=true;
							break;
						}
					}
				}
				break;
			case ']':
				if(tape[ptr]!=0){
					Iptr--;
					int count=1;
					while(true){
						if(Iptr<0){
							//printf("\nloop without [ !\n");
							error=true;
							break;
						}
						if(instruction[Iptr]==']')count++;
						if(instruction[Iptr]=='[')count--;
						if(count==0)break;
						Iptr--;
					}
				}
				break;
			default:
				//printf("\nunknown instruction \"%d\"\n",(int)instruction[Iptr]);
				error=true;
				return (int)error;
				break;
		}
	return (int)error;
}

bool brainFuck::qualify(string& instruction){
	int upbracket=-1, downbracket=-1;
	int size=instruction.size();

	do{
		int count=0;
		while(++downbracket<size){
			if(instruction[downbracket]=='[')count++;
			if(instruction[downbracket]==']'){
				if(count)
					count--;
				else
					break;
			}
		}
		if(downbracket==size)break;
		upbracket=size;
		while(--upbracket>=0){
			if(instruction[upbracket]==']')count++;
			if(instruction[upbracket]=='['){
				if(count)
					count--;
				else
					break;
			}
		}
		if(upbracket==-1)break;

		instruction[downbracket]='[';
		instruction[upbracket]=']';
	}while(true);

	return true;
}
