/***************************************************************************
 *   Copyright (C) 2004 by Tian-Li Yu                                      *
 *   tianliyu@cc.ee.ntu.edu.tw                                             *
 *                                                                         *
 *   You can redistribute it and/or modify it as you like                  *
 ***************************************************************************/

#include <cmath>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <chrono>

#include "statistics.h"
#include "ga.h"
#include "chromosome.h"
#include "global.h"

using namespace std;

int main (int argc, char *argv[])
{

    //if (argc != 9) {
    //printf ("GA ell nInitial selectionPressure pc pm maxGen maxFe repeat\n");
    //return -1;
    //}

    // 50~500
    std::vector<int> ellsize;//ell
    std::vector<int> popsize;//ninitial
    int nowell = 50;
    while(nowell <= 500){
        ellsize.push_back(nowell);
        popsize.push_back(4*nowell*log(nowell));// 4*ell*ln(ell)
        nowell += 50;
    }

    int selectionPressure = 2;
    double pc = 1;//prob of XO
    double pm = 0;//no mutation
    int maxGen = -1; // max generation: inf
    int maxFe = -1;  // max fe -1 = INF
    int repeat = 30; // how many time to repeat

    FILE * timeFile = fopen("uniformtime.dat", "w");
    for(int e = 0; e < ellsize.size(); e++){
        auto start = std::chrono::steady_clock::now();
        Statistics stGenS, stGenF;
        int usedGen;
        int failNum = 0;
        for (int i = 0; i < repeat; i++) {
            GA ga (ellsize[e], popsize[e], selectionPressure, pc, pm, maxGen, maxFe);

            if (repeat == 1)
                usedGen = ga.doIt (true);
            else
                usedGen = ga.doIt (false);

            Chromosome ch(ellsize[e]);
            if (ga.stFitness.getMax() < ch.getMaxFitness()) {
                printf ("-");
                failNum++;
                stGenF.record (usedGen);
            }
            else {
                printf ("+");
                stGenS.record (usedGen);
            }

            fflush (NULL);
        }
        printf ("\nAverage Gen of Success: %f\n", stGenS.getMean());
        printf ("Average Gen of Failure: %f\n", stGenF.getMean());
        printf ("Total number of Failure: %d\n", failNum);
        auto end = std::chrono::steady_clock::now();
        fprintf(timeFile, "%d %lf\n",
                ellsize[e], double(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count())/1000);
    }
    fclose(timeFile);
    return EXIT_SUCCESS;
}
