set terminal postscript landscape enhanced mono dashed lw 1 "Helvetica" 10
set output "out.ps"
set xtics 1
set ytics (0,9,16,25,37,41,63,69)
set title "Memory speed"
set xlabel "Amount of fragments"
set ylabel "Min. avg. Clock cycles/element"
set grid
plot "output" title "f" with linespoints linetype 13 
     
