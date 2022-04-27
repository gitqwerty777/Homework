
#include <string>
#include <iostream>
#include "brainPlus.h"

brainPlus::brainPlus(){
	TuringMachine::putInst('<');
	TuringMachine::putInst('>');
	TuringMachine::putInst('+');
	TuringMachine::putInst('-');
	TuringMachine::putInst('.');
	TuringMachine::putInst(',');
	TuringMachine::putInst('[');
	TuringMachine::putInst(']');

	//TuringMachine::putInst('$');
	//TuringMachine::putInst('!');
	for(int i=0;i<10;i++)
		TuringMachine::putInst('0'+i);
	for(int i=0;i<6;i++)
		TuringMachine::putInst('A'+i);
	//for(int i=0;i<26;i++)
	//	TuringMachine::putInst('a'+i);
	//TuringMachine::putInst('@');
}

void brainPlus::specificInit(){
	
}

int brainPlus::instExec(TreeString& instruction, string& tape, int& ptr, int& Iptr, string& input, int& inputIndex, string& output){
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
			case '$':
				_storage=tape[ptr];
				break;
			case '!':
				tape[ptr]=_storage;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				tape[ptr]=16*(instruction[Iptr]-'0');
				break;
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
				tape[ptr]=16*(instruction[Iptr]-'A'+10);
				break;
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':
				int functionNumber;
				functionNumber=instruction[Iptr]-'a'+1;
				int i;
				for(i=0;i<(int)instruction.size();i++)
					if(instruction[i]=='@')
						if(!(--functionNumber)){
							_stack.push(Iptr);
							Iptr=i;
							break;
						}
				if(i>=(int)instruction.size()-1){
					//printf("\nno function %c exist\n",instruction[Iptr]);
					error=true;
					return (int)error;
				}
				break;
			case '@':
				if(_stack.size()){
					Iptr=_stack.top();
					_stack.pop();
				}else{
					return 2;
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

bool brainPlus::qualify(TreeString& instruction){
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
