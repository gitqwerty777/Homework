#include <algorithm>
#include <iostream>
#include <cstdlib>
#include "ga.h"

using namespace std;

extern MachineMgr* macMgr;

bool Equal(double a , double b){
  return std::abs(a-b) < 0.0000001;
}

GA::GA():	_populationSize(1),
			_generation(0),
			_maxGen(1000),
			_currentLength(100),
			_maxLength(150),
			_machineType("brainPlus"),
			_selectionPressure(2),
			_pCross(0.5),
			_pMutate(0)
			{}
GA::GA(int populationSize, int maxGen, int initLength, int maxLength, string machineType, int sp, double pc, double pm){
	init(populationSize, maxGen, initLength, maxLength, machineType, sp, pc, pm);
}
GA::~GA(){}

void GA::init(int populationSize, int maxGen, int initLength, int maxLength, string machineType, int sp, double pc, double pm){
	_machineType=machineType;
	_populationSize=populationSize;
	_generation=0;
	_maxGen=maxGen;
	_currentLength=initLength;
	_maxLength=maxLength;
	_target="aa";

	_selectionPressure=sp;
	_pCross=pc;
	_pMutate=pm;
	_population.clear();
	for(int i=0;i<_populationSize;i++){
		_population.push_back(chromosome(_currentLength, macMgr->getMac(_machineType)));
	}
	_offspring.clear();
}

double GA::targetFitness(){
  if(newTarget())
    return 0;
  else
    return _target.size()*256+10;
}

bool GA::newTarget(){
  return _target == "add" || _target == "minus" || _target == "mul2" || _target == "mul3";
}

bool GA::runGA(){
	cout << "target: " << _target ;
	for(size_t i=0;i<_target.size();i++)
		cout << " " << int(_target[i]);
	cout << endl;
	while(_generation<_maxGen){
		vector<int> selectedList;
		selection(selectedList);
		double max=-2147483647;
		int best=-2147483647;
		double min=1000000;
		double total=0;
		for(int i=0;i<_populationSize;i++){
			total+=_population[i].getFit();
			if(_population[i].getFit()>max){max=_population[i].getFit();best=i;}
			if(_population[i].getFit()<min)min=_population[i].getFit();
		}
		if(_generation%30==0 || Equal(max, targetFitness())){
			cout << "generation : " << _generation << endl;
			for(int i=0;i<_populationSize;i++)
      				cout << _population[i].getGene() << " " << _population[i].evaluated() << " " \
      << _population[i].getFit() << endl;// " [" << _population[i].getOutput() << "]" << endl;
			cout << "fitness (max/mean/min) " << max << "/" << total/_populationSize << "/" << min <<endl;
			cout << "best chromosome: " << _population[best].getGene() <<endl;
      if(newTarget()){
        cout << "print data information" << endl;
        _population[best]._evaluated = false;
        _population[best].evaluate_testdata_print(macMgr->getMac(_machineType));
      } else {
        cout << "best output: [";
        if(0){//chracter or number
          cout << _population[best].getOutput() << "]" << endl;
        }else{
          for(size_t i=0;i<_population[best].getOutput().size();i++)
            cout << (int)_population[best].getOutput()[i]  <<" " ;
          cout << "]" << endl;
        }
      }
			if(Equal(max, targetFitness()))return true;
			//char tmp;
			//tmp=getchar();
		}
		crossover(selectedList);
		mutation();
		_population=_offspring;
		_generation++;
	}

	//printf("best fitness %d / %d\n",ga.bestFit(),ga.maxFit());
	//printf("best output : %s \n",ga.bestOutput());
	return false;
}



void GA::selection(vector<int>& selectedList){
	//w/o replacement tournament selection
	//printf("s\n");
	for(int i=0;i<_populationSize;i++)
		if(!_population[i].evaluated())
			_population[i].evaluate(macMgr->getMac(_machineType), _target);

	vector<int> usedList;
	int tournamentRound=1, maxFit=-10000000, count=0, winner;
	usedList.resize(_populationSize,0);
	for(int i=0;i<_populationSize;i++){
		for(int i1=0;i1<_selectionPressure;i1++){
			int pos;
			pos=double(rand())/RAND_MAX*_populationSize;
			while(usedList[pos]==tournamentRound)pos=double(rand())/RAND_MAX*_populationSize;
			usedList[pos]++;
			if(maxFit<=_population[pos].getFit()){
				winner=pos;
				maxFit=_population[pos].getFit();
			}
			count++;
			if(count==_populationSize){
				count=0;
				tournamentRound++;
			}
		}
		selectedList.push_back(winner);
		maxFit=0;
	}

	//for(int i=0;i<_populationSize;i++)
	//	if(_population[i].getFit()==0)
	//		_population.erase(_population.begin()+i);
	//sort(_population.begin(),_population.end(),chromosome::cmp);

	//for(int i=0;i<_populationSize;i++)
	//	cout << selectedList[i] << " " <<_population[selectedList[i]].getFit() << " " <<_population[selectedList[i]].getGene() <<endl;
	//cout << endl;
}
void GA::crossover(vector<int>& selectedList){
	//printf("c\n");
	_offspring.clear();
	for(int i=0;i<_populationSize/2;i++){
		int pos=(double(rand())/RAND_MAX*(_currentLength-2))+1;
		if(!(double(rand())/RAND_MAX<_pCross)) pos=_currentLength;
		string child1,child2;
		for(int i1=0;i1<_currentLength;i1++){
			if(i1<pos){
				child1.push_back(_population[selectedList[i*2]].getGene()[i1]);
				child2.push_back(_population[selectedList[i*2+1]].getGene()[i1]);
			}else{
				child1.push_back(_population[selectedList[i*2+1]].getGene()[i1]);
				child2.push_back(_population[selectedList[i*2]].getGene()[i1]);
			}
		}

    chromosome c1(child1, macMgr->getMac(_machineType));
    chromosome c2(child2, macMgr->getMac(_machineType));
    c1.evaluate(macMgr->getMac(_machineType), _target);
    c2.evaluate(macMgr->getMac(_machineType), _target);
    if(c1.getFit() + c2.getFit() <= _population[selectedList[i*2]].getFit() + _population[selectedList[i*2+1]].getFit()){
      _offspring.push_back(c1);
      _offspring.push_back(c2);
    } else {
      _offspring.push_back(_population[selectedList[i*2]]);
      _offspring.push_back(_population[selectedList[i*2+1]]);
    }
	}
}
void GA::mutation(){
	for(int i=0;i<_populationSize;i++){
		for(int i1=0;i1<_currentLength;i1++){
			if(double(rand())/RAND_MAX<_pMutate){
				double mutateMethod;
				mutateMethod=double(rand())/RAND_MAX;
//				cout <<_offspring[i].getGene() << mutateMethod << " " << i1 << " " ;
				if(mutateMethod<0.25){//replace
						_offspring[i].setVal(i1,macMgr->getRndInst(_machineType));
				}else{
					char direction=double(rand())/RAND_MAX*2;
//					cout << int(direction) << endl;
					int startpos,endpos,insertpos;
					char bit;
					if(mutateMethod<0.5){//insert
						bit=_offspring[i].getVal(i1);
						startpos=i1+1;	endpos=_currentLength-1;
						insertpos=i1;
					}else if(mutateMethod<0.75){//delete
						bit=_offspring[i].getVal(0);
						startpos=1;		endpos=i1;
						insertpos=0;
					}else if(mutateMethod<1){//shilt
						bit=_offspring[i].getVal(0);
						startpos=1;		endpos=_currentLength-1;
						insertpos=0;
					}
					if(direction){
						for(int i2=startpos;i2<=endpos;i2++){
							char nextbit=_offspring[i].getVal(i2);
							_offspring[i].setVal(i2,bit);
							bit=nextbit;
						}
					}else{
						if(mutateMethod<0.5){
							startpos-=2;endpos=0;
						}else if(mutateMethod<0.75){
							insertpos=startpos=_currentLength-1;
						}else if(mutateMethod<1){
							startpos=_currentLength-1;
							endpos=insertpos=0;
						}
						for(int i2=startpos;i2>=endpos;i2--){
							char nextbit=_offspring[i].getVal(i2);
							_offspring[i].setVal(i2,bit);
							bit=nextbit;
						}
					}
					if(mutateMethod<0.75){
						_offspring[i].setVal(insertpos,macMgr->getRndInst(_machineType));
					}else if(mutateMethod<1){
						_offspring[i].setVal(insertpos,bit);
					}
				}
//				cout << _offspring[i].getGene() << endl;
			}
		}
	}
}

int GA::bestFit(){return 0;}
string GA::bestOutput(){return 0;}
int GA::currentGen(){return _generation;}

int GA::maxFit(){return 0;}
void GA::setTarget(string target){_target=target;}
