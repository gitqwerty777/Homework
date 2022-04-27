#!/usr/bin/env gnuplot

# http://gnuplot.sourceforge.net/demo/fit.html

# initial -- metadata
# reset
set xlabel "chromosome length"
set ylabel "avg. generation"
set title "3 crossover result"
set terminal pngcairo
set output 'crossover.png'
# unset key
plot "uniform.dat" u 1:2 smooth csplines title "uniform",\
     "onepoint.dat" u 1:2 smooth csplines title "onepoint",\
     "column.dat" u 1:2 smooth csplines title "population-wise" 
#pause -1
