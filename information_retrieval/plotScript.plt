#!/usr/bin/env gnuplot

# http://gnuplot.sourceforge.net/demo/fit.html

# initial -- metadata
# reset
set xlabel "c"
set ylabel "value"
set title "Skip Pointer"
set terminal pngcairo
set output 'predictSkipPointer.png'
set xrange[0:100]
# unset key
k=4
n=1000000
f(c) = k*n/c + 1000*c/2
g(c) = k*n/c + 10000*c/2
plot f(x) w l  lt rgb 'blue' t '1000' , g(x) w l lt rgb 'green' t '10000' 

pause -1
