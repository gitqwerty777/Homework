/***************************************************************************
                          myrand.h  -  description
                             -------------------
    begin                : Aug 24 2001
    copyright            : (C) 2001 by Tian-Li Yu
    email                : tianliyu@cc.ee.ntu.edu.tw
 ***************************************************************************/

#ifndef MYRAND_H
#define MYRAND_H

#include "mt19937ar.h"

class MyRand
{
    public:
        MyRand ();
        ~MyRand ();

        bool flip (double prob = 0.5);

        /** From [a,b) */
        double uniform (double a, double b);

        /** From [0,1) */
        double uniform ();

        /** Int From [a,b] */
        int uniformInt (int a, int b);

        /** Generate a random array of size num, from [a,b] */
        void uniformArray (int *array, int num, int a, int b);
};
#endif
