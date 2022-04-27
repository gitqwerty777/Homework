#ifndef GA_H
#define GA_H

#include <vector>
#include <string>
#include "chromosome.h"

using namespace std;

class GA{
public:
	GA();
	GA(int populationSize, int maxGen, int initLength, int maxLength, string machintype, int sp, double pc, double pm);
	~GA();

	void init(int populationSize, int maxGen, int initLength, int maxLength, string machintype, int sp, double pc, double pm);

	bool runGA();
  double targetFitness();
  bool newTarget();

	void selection(vector<int>&);
	void crossover(vector<int>&);
	void mutation();

	int bestFit();
	string bestOutput();
	int currentGen();

	int maxFit();
	void setTarget(string);

private:
	vector<chromosome> _population;
	vector<chromosome> _offspring;
	int _populationSize;
	int _generation;
	int _maxGen;
	int _currentLength;
	int _maxLength;
	string _target;

	string _machineType;

	int _selectionPressure;
	double _pCross,_pMutate;

};

#endif
