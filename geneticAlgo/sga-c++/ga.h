/***************************************************************************
 *   Copyright (C) 2004 by Tian-Li Yu                                      *
 *   tianliyu@cc.ee.ntu.edu.tw                                             *
 ***************************************************************************/

#ifndef GA_H
#define GA_H

#include "chromosome.h"
#include "statistics.h"
#include "myrand.h"

class GA
{

    public:
        GA ();
        GA (int n_ell, int n_nInitial, int n_selectionPressure, double n_pc,
            double n_pm, int n_maxGen, int n_maxFe);

        ~GA ();

        void init (int n_ell, int n_nInitial, int n_selectionPressure, double n_pc,
            double n_pm, int n_maxGen, int n_maxFe);

        void initializePopulation ();
        void evaluate ();

        void selection ();

        /** tournament selection without replacement */
        void tournamentSelection ();

	/** Roulette wheel selection */
	void rwSelection ();

        void crossover ();
        void pairwiseXO (const Chromosome &, const Chromosome &, Chromosome &, Chromosome &);
	void onePointXO (const Chromosome &, const Chromosome &, Chromosome &, Chromosome &);
        void uniformXO (const Chromosome &, const Chromosome &, Chromosome &, Chromosome &, double);

        void mutation ();
        void simpleMutation ();
	void mutationClock ();

        void replacePopulation ();

        void showStatistics ();
        void oneRun (bool output = true);
        int doIt (bool output = true);
        void populationWiseShuffling();

        bool shouldTerminate ();
        int getNextPopulation ();

        Statistics stFitness;

    protected:

        int ell;                 // chromosome length
        int nInitial;            // initial population size
        int nCurrent;            // current population size
        int nNextGeneration;     // population size for the next generation
        int selectionPressure;

        double pc;               // prob of XO
        double pm;               // prob of Mutation
        Chromosome *population;
        Chromosome *offspring;
        int *selectionIndex;
        int maxGen;
        int maxFe;
        int repeat;
        int fe;
        int generation;
        int bestIndex;

};
#endif
