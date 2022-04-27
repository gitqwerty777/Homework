#ifndef CHROMOSOME_H
#define CHROMOSOME_H

#include <string>
#include "baseTM.h"

#define TESTDATANUM 20

class TestData{
public:
    std::vector<int> input;
    int output;
    string inputstr;
    string outputstr;
    void init(std::vector<int>& input, int output){
        this->input = input;
        this->output = output;
        outputstr = "";
        outputstr += output;
        inputstr = "";
        for(int i = 0; i < input.size(); i++){
            inputstr += input[i];
        }
    }
    double evaluate(TuringMachine* machine, bool isPrint = false){
        int oneFitness = 0;

        int returnVal = machine->run();
        //if(machine->run()>0){
            string outputStr = outputstr;
            string realStr = machine->getOutput();

            if(realStr.size() >= 1){
                oneFitness += abs(outputStr[0]-realStr[0]);
                oneFitness *= 10;
                oneFitness += abs(int(outputStr.size() - realStr.size()));
            } else
                oneFitness += abs(int(outputStr.size() - realStr.size()))*100000;
            if(isPrint)
                fprintf(stderr, "input str %d %d, output %d len %d, evaluate %d len %d fitness %d\n", inputstr[0], inputstr[1], outputStr[0], outputStr.length(), realStr[0], realStr.length() ,oneFitness);
            //if(returnVal > 0)
            //oneFitness+=100000;
            return oneFitness;
            //} else {//no output, give maximum fitness punishment
            //return 2147483647;
            //}


    }
    void print(){
        fprintf(stderr, "input %d %d\noutput %d\n", inputstr[0], inputstr[1], outputstr[0]);
    }
};

class chromosome{
public:
	chromosome();
	chromosome(const int&, TuringMachine*);
	chromosome(const string&, TuringMachine*);
	~chromosome();

	static bool cmp(chromosome, chromosome);

	void evaluate(TuringMachine*, string);

	bool evaluated();
	string getGene();
	char getVal(int);
	void setVal(int,char);
	int getLength();
	string getInput();
	string getOutput();
	double getFit();
  void initTestData_Add();
  void initTestData_Minus();
  void initTestData_Mul2();
  void initTestData_Mul3();

  //not added
  void initTestData_xor();
  void initTestData_rev();
  void initTestData_hello();
  void initTestData_fib();
  void initTestData_if();
  void initTestData_countdown();
  /*
    hello {user} 1,793 42,800
    //Addition 2,698 92,400
    //Subtraction 4,305 177,900
    //Multiply x2 6,353 242,000
    //Multiply x3 5,165 87,200
    //XOR 2,095 146,400
    Fibonacci 21,862 151,900
    If/then conditionals 8,313 46,200
    //Reverse string 49 2,600
    CSV parse 173 9,000
    XML to JSON 6,866 820,900
    Warning countdown 48 900
   */

  bool _evaluated;
  void evaluate_testdata(TuringMachine*);
  void evaluate_testdata_print(TuringMachine* machine);
private:
	string _gene;
	int _length;
	string _input;
	string _output;
	double _fitness;

};

static std::vector<TestData> testdata;

#endif
