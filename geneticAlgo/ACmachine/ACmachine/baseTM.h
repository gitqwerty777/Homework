
#ifndef BASE_TM
#define BASE_TM

#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#include <map>
#include <stack>

using namespace std;

enum status{
	OK		=0,
	ERROR	=0x1,
	END		=0x2,
	TIMEOUT	=0x4
};

class TuringMachine{
public:
	TuringMachine():_timeLimit(100000),_status(0) {}
	virtual ~TuringMachine();
	
	void init(string inst, string input);
	virtual void specificInit()=0;
	int run();
	void print(void);
	virtual int instExec(string&, string&, int&, int&, string&, int&, string&)=0;
	virtual bool qualify(string&)=0;
	void putInst(char inst);
	char getInst(int i);
	int getSetSize(void);
	bool setInstruction(string);

	const string& getOutput(void);

private:
	string _tape;
	string _Instruction;
	string _input;
	int _inputIndex;
	string _output;
	vector<char> _instructionSet;
	int _timeLimit;
	int _status;
};

class MachineMgr{

typedef map<const string, TuringMachine*>   MacMap;
typedef pair<const string, TuringMachine*>  MacRegPair;

public:
   MachineMgr(){}
   virtual ~MachineMgr() {}

   bool openDofile(const string& dof);
   void closeDofile();

   bool regMac(const string&, unsigned, TuringMachine*);
   int run();

   // public helper functions
   TuringMachine* getMac(string);
   char getRndInst(string);

private:
   // Private member functions
   bool readMac(istream&);
   TuringMachine* chooseMac(string&);
   void listMac(const string&);


   // Data members
   ifstream* _dofile;                // for command script
   MacMap    _macMap;                // map from string to command
   stack<ifstream*> _dofileStack;    // For recursive dofile calling
};

#endif
