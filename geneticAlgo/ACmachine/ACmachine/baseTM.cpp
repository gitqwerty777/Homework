#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#include "baseTM.h"
#include <cstdlib>



TuringMachine::~TuringMachine(){}

void TuringMachine::init(string inst,string input){
	_Instruction=inst;
	_tape.clear();
	_tape.resize(1000,0);
	_output.clear();
	_input.clear();
	_input=input;
	_inputIndex=0;
	_status=0;
	specificInit();
}

int TuringMachine::run(void){
	int Iptr=0;
	int ptr=0;
	int time=0;
	while(time<_timeLimit && !(_status & (ERROR | END))){
		_status = instExec(_Instruction, _tape, ptr, Iptr, _input, _inputIndex, _output);
		++Iptr;
		if((unsigned)Iptr>=_Instruction.size())break;
		time++;
	}
	//cout << time << " " << _timeLimit << endl;
	if(time>=_timeLimit)_status&=TIMEOUT;
	return _status;
}

void TuringMachine::print(void){
	cout << endl;
	cout << "output:" << _output << endl;
	cout << "tape  :" << _tape << endl;
	cout << "instru:" << _Instruction << endl;
}

void TuringMachine::putInst(char inst){_instructionSet.push_back(inst);}
char TuringMachine::getInst(int i){return _instructionSet[i];}
int TuringMachine::getSetSize(){return _instructionSet.size();}

bool TuringMachine::setInstruction(string instruction){
	int length=instruction.size();
	for(int i=0;i<length;i++){
		size_t i1=0;
		for(;i1<_instructionSet.size();i1++)
			if(instruction[i]==_instructionSet[i1])
				break;
		if(i1==_instructionSet.size())
			return false;
	}
	_Instruction=instruction;
	return true;
}

const string& TuringMachine::getOutput(){return _output;}

int MachineMgr::run(){
	return 0;
}

bool MachineMgr::regMac(const string& name, unsigned, TuringMachine* machine){
	
	_macMap.insert(MacRegPair(name,machine));
	return true;
}

TuringMachine* MachineMgr::getMac(string machineType){
	return _macMap.find(machineType)->second;
}

char MachineMgr::getRndInst(string machineType){
	TuringMachine* tmpMac=getMac(machineType);
	return tmpMac->getInst((int)(tmpMac->getSetSize()*(double(rand())/RAND_MAX)));
}


