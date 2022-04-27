/***************************************************************************
 *   Copyright (C) 2004 by Tian-Li Yu                                      *
 *   tianliyu@cc.ee.ntu.edu.tw                                             *
 ***************************************************************************/

#ifndef _STATISTICS_H
#define _STATISTICS_H

#include <math.h>
#include <float.h>


class Statistics
{

    public:

        Statistics () {
            reset ();
        }

        void reset () {
            precision = 1e-6;
            min = DBL_MAX-1;
            second_min = DBL_MAX-2;
            max = -DBL_MAX+1;
            second_max = -DBL_MAX+2;
            sum = 0.0;
            variance = 0.0;
            number = 0;
            status = true;
        }

        void record (double value) {

            if (status == false)
                return;

            number++;
            sum += value;
            variance += value * value;
            if (min > value + precision) {
                second_min = min;
                min = value;
            }
            if (max < value - precision) {
                second_max = max;
                max = value;
            }
        }

        /** get the number of samples */
        long int getNumber () {
            return number;
        }

        /** get mean */
        double getMean () {
            return sum / number;
        }

        /** get variance */
        double getVariance () {
            double mean = getMean ();
            return variance / number - mean * mean;
        }

        /** get standard deviation */
        double getStdev () {
            return::sqrt (getVariance ());
        }

        double getMin () {
            return min;
        }

        double getMax () {
            return max;
        }

        double getSecondMax () {
            return second_max;
        }

        double getSecondMin () {
            return second_min;
        }

        void turnOn () {
            status = true;
        }

        void turnOff () {
            status = false;
        }

    private:

        double precision;
        double min;
        double max;
        double second_min;
        double second_max;
        double sum;
        double variance;
        long int number;
        bool status;
};
#endif
