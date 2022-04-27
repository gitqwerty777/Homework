#include <cstdlib>
#include "chromosome.h"
#include "baseTM.h"
#include <iostream>
#include <cctype>

//srand(unsigned(0));

chromosome::chromosome(){
	_length=0;
	_fitness=0;
	_evaluated=false;
}
chromosome::chromosome(const int& l, TuringMachine* machine){
	do{
		_gene.clear();
		for(int i=0;i<l;i++)
			_gene.push_back(machine->getInst((int)( machine->getSetSize()*(double(rand())/RAND_MAX))));
	}while(!machine->qualify(_gene));

	_length=l;
	_fitness=0;
	_evaluated=false;
}
chromosome::chromosome(const string& s,TuringMachine* machine){
	_gene=s;
	_length=s.size();
	_fitness=0;
	_evaluated=false;
}
chromosome::~chromosome(){}

bool chromosome::cmp(chromosome a,chromosome b){return a._fitness>b._fitness;}

void chromosome::initTestData_Add(){
  fprintf(stderr, "initTestData_Add\n");
  testdata.clear();
  for(int i = 0; i < TESTDATANUM; i++){
    TestData td;
    std::vector<int> input;
    input.push_back(rand() % 50);
    input.push_back(rand() % 50);
    td.init(input, input[0]+input[1]);
    testdata.push_back(td);
  }
}
void chromosome::initTestData_Minus(){
  fprintf(stderr, "initTestData_Minus\n");
  testdata.clear();
  for(int i = 0; i < TESTDATANUM; i++){
    TestData td;
    std::vector<int> input;
    input.push_back(rand() % 120);
    input.push_back(rand() % 120);
    if(input[1] > input[0])
      swap(input[0], input[1]);
    td.init(input, input[0]-input[1]);
    testdata.push_back(td);
  }
}
void chromosome::initTestData_Mul2(){
  fprintf(stderr, "initTestData_Mul2\n");
  testdata.clear();
  for(int i = 0; i < TESTDATANUM; i++){
    TestData td;
    std::vector<int> input;
    input.push_back(rand() % 50);
    td.init(input, input[0]*2);
    testdata.push_back(td);
  }
}
void chromosome::initTestData_Mul3(){
  fprintf(stderr, "initTestData_Mul3\n");
  testdata.clear();
  for(int i = 0; i < TESTDATANUM; i++){
    TestData td;
    std::vector<int> input;
    input.push_back(rand() % 40);
    td.init(input, input[0]*3);
    testdata.push_back(td);
  }
}
void chromosome::initTestData_xor(){
  fprintf(stderr, "initTestData_xor\n");
  testdata.clear();
  for(int i = 0; i < TESTDATANUM; i++){
    TestData td;
    std::vector<int> input;
    input.push_back(rand() % 2);
    input.push_back(rand() % 2);
    td.init(input, input[0]^input[1]);
    testdata.push_back(td);
  }
}

/*void chromosome::initTestData_fib(){
  fprintf(stderr, "initTestData_fib\n");
  testdata.clear();
  for(int i = 0; i < TESTDATANUM; i++){
    TestData td;
    std::vector<int> input;
    input.push_back(1);
    input.push_back(1);
    for(int j = 2; j < 10; j++)
      input.push_back(input[j-2]+input[j-1]);//TODO: outputstr instead int output;
    td.init(input, input[0]^input[1]);
    testdata.push_back(td);
  }
}

void chromosome::initTestData_if(){
  fprintf(stderr, "initTestData_if\n");
  testdata.clear();
  for(int i = 0; i < TESTDATANUM; i++){
    TestData td;
    std::vector<int> input;
    std::vector<int> output;
    for(int j = 0 ;j < 3; j++){
      output.push_back(rand()%26);
    }
    td.init(input, output);
    testdata.push_back(td);
  }
}

void chromosome::initTestData_countdown(){
  fprintf(stderr, "initTestData_countdown\n");
  testdata.clear();
  for(int i = 0; i < TESTDATANUM; i++){
    TestData td;
    std::vector<int> input;
    std::vector<int> output;
    input.push_back(rand()%10);
    for(int j = input[0] ;j >= 0; j--){
      //string s =
      //output.push_back("time left %d\n", );
    }
    td.init(input, output);
    testdata.push_back(td);
  }
}

void chromosome::initTestData_hello(){
  fprintf(stderr, "initTestData_hello\n");
  testdata.clear();
  for(int i = 0; i < TESTDATANUM; i++){
    TestData td;
    std::vector<int> input;
    string output = "";
    for(int j = 0 ;j < 20; j++){
      input.push_back(rand() % 26);
      output += input[j];
    }
    output = "hello "+output;
    td.init(input, output);
    testdata.push_back(td);
  }
}

void chromosome::initTestData_rev(){
  fprintf(stderr, "initTestData_rev\n");
  testdata.clear();
  for(int i = 0; i < TESTDATANUM; i++){
    TestData td;
    std::vector<int> input;
    string output = "";
    for(int j = 0 ;j < 20; j++){
      input.push_back(rand() % 26);
      output = ""+input[j]+output;
    }
    td.init(input, output);
    testdata.push_back(td);
  }
  }*/

void chromosome::evaluate_testdata(TuringMachine* machine){
  _fitness=0;

  for(int t = 0; t < testdata.size(); t++){
    machine->init(_gene,testdata[t].inputstr);
    _fitness += testdata[t].evaluate(machine);
  }
  if(_fitness > testdata.size() * 2147483647){
    evaluate_testdata_print(machine);
  }
  _fitness = -_fitness;//max 0, min -inf
	_evaluated=true;
}

void chromosome::evaluate_testdata_print(TuringMachine* machine){
  _fitness=0;

  for(int t = 0; t < testdata.size(); t++){
    machine->init(_gene,testdata[t].inputstr);
    _fitness += testdata[t].evaluate(machine, true);
  }
  _fitness = -_fitness;//max 0, min -inf

  fprintf(stderr, "total fitness %lf\n", _fitness);
	_evaluated=true;
}

void chromosome::evaluate(TuringMachine* machine, string target){
  bool isNormal = false;
  if(target == "add"){
    if(testdata.size() == 0)
      initTestData_Add();
  } else if(target == "minus"){
    if(testdata.size() == 0)
      initTestData_Minus();
  } else if(target == "mul2"){
    if(testdata.size() == 0)
      initTestData_Mul2();
  } else if(target == "mul3"){
    if(testdata.size() == 0)
      initTestData_Mul3();
  } else {
    isNormal = true;
  }
  if(!isNormal){
    evaluate_testdata(machine);
    return;
  }
	_fitness=0;
	machine->init(_gene,"");
	if(machine->run()>0){
		_output=machine->getOutput();
		for(unsigned i=0;i<target.size();i++){
			if(i>machine->getOutput().size())break;
			int tmp=target[i]-machine->getOutput()[i];
			if(tmp<0)tmp*=-1;
			_fitness+=256-tmp;
		}
		int tmp=target.size()-machine->getOutput().size();
		if(tmp<0)tmp*=-1;
		_fitness+=10*((double)target.size()-tmp)/target.size();
	}
	_evaluated=true;
}
bool chromosome::evaluated(){return _evaluated;}
string chromosome::getGene(){return _gene;}
char chromosome::getVal(int index){return _gene[index];}
void chromosome::setVal(int index, char value){_gene[index]=value;}
int chromosome::getLength(){return _length;}
string chromosome::getInput(){return _input;}
string chromosome::getOutput(){return _output;}
double chromosome::getFit(){return _fitness;}
