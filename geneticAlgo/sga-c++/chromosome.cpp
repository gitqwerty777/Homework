/***************************************************************************
 *   Copyright (C) 2004 by Tian-Li Yu                                      *
 *   tianliyu@cc.ee.ntu.edu.tw                                             *
 ***************************************************************************/

#include <stdio.h>
#include "global.h"
#include "chromosome.h"

Chromosome::Chromosome ()
{
    length = 0;
    gene = NULL;
    evaluated = false;
}


Chromosome::Chromosome (int n_length)
{
    gene = NULL;
    init (n_length);
}


Chromosome::~Chromosome ()
{
    delete[]gene;
}


void Chromosome::init (int n_length)
{
    length = n_length;

    if (gene != NULL)
        delete[]gene;

    gene = new bool[length];
    evaluated = false;
}

int Chromosome::getVal (int index) const
{
    if (index < 0 || index > length)
        outputErrMsg ("Index overrange in Chromosome::operator[]");

    return (gene[index])? 1:0;
}


void Chromosome::setVal (int index, int val)
{
    if (index < 0 || index > length)
        outputErrMsg ("Index overrange in Chromosome::operator[]");

    gene[index] = (val==1)? true:false;
    evaluated = false;
}


double Chromosome::getFitness ()
{
    if (evaluated)
        return fitness;
    else
        return (fitness = evaluate ());
}


bool Chromosome::isEvaluated () const
{
    return evaluated;
}


double Chromosome::evaluate ()
{
    evaluated = true;
    return oneMax ();
}


// OneMax
double Chromosome::oneMax () const
{
    int i;
    double result = 0;

    for (i = 0; i < length; i++)
        result += gene[i];

    return result;
}


Chromosome & Chromosome::operator= (const Chromosome & c)
{
    int i;

    if (length != c.length) {
        length = c.length;
        delete[]gene;
        init (length);
    }

    evaluated = c.evaluated;
    fitness = c.fitness;

    for (i = 0; i < length; i++)
        gene[i] = c.gene[i];

    return *this;
}


void Chromosome::printf () const
{
    int i;
    for (i = 0; i < length; i++)
        ::printf ("%d", gene[i]);
}


int Chromosome::getLength () const
{
    return length;
}


double Chromosome::getMaxFitness () const
{
    // For OneMax
    return ((double)length-1e-6);
}
