
#ifndef BRAIN_FUCK
#define BRAIN_FUCK

#include <string>
#include "baseTM.h"

class brainFuck :public TuringMachine{
public:
	brainFuck(); //><+-.,[]
	~brainFuck(){}
	
	void specificInit();
	int instExec(string&, string&, int&, int&, string&, int&, string&);
	bool qualify(string&);
};


#endif
