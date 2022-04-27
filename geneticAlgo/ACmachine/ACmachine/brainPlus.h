
#ifndef BRAIN_PLUS
#define BRAIN_PLUS

#include <string>
#include <stack>
#include "baseTM.h"

class brainPlus :public TuringMachine{
public:
	brainPlus();
	~brainPlus(){}
	
	void specificInit();
	int instExec(TreeString&, string&, int&, int&, string&, int&, string&);
	bool qualify(TreeString&);
	char _storage;
	stack<int> _stack;
};


#endif
