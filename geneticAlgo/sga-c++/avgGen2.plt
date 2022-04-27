#!/usr/bin/env gnuplot

# http://gnuplot.sourceforge.net/demo/fit.html

# initial -- metadata
# reset
set xlabel "chromosome length"
set ylabel "avg. generation"
set title "3 crossover result"
set terminal pngcairo lw 3
set output 'crossover2.png'
#set style lw 5
# unset key
plot "uniform.dat" u 1:2 smooth csplines title "uniform",\
     "onepoint.dat" u 1:2 smooth csplines title "onepoint",\
     "column.dat" u 1:2 smooth csplines title "population-wise", \
     "theory.dat" u 1:2 smooth csplines title "Thierens" 
#pause -1
