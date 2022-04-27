#!/usr/bin/env gnuplot

# http://gnuplot.sourceforge.net/demo/fit.html

# initial -- metadata
# reset
set xlabel "chromosome length"
set ylabel "used time of 30 independent run(second)"
set title "3 crossover time"
set terminal pngcairo lw 3
set output 'crossovertime.png'
#set style lw 5
# unset key
plot "uniformtime.dat" u 1:2 smooth csplines title "uniform",\
     "onepointtime.dat" u 1:2 smooth csplines title "onepoint",\
     "populationtime.dat" u 1:2 smooth csplines title "population-wise"
#pause -1
