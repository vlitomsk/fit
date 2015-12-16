set terminal postscript landscape enhanced mono dashed lw 1 "Helvetica" 8
set output "out.ps"
set title "Memory speed"
set logscale x
set xlabel "Array size (KiB)"
set ylabel "Clock cycles/element"
set grid
set xtics(1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768)
plot "fwd" title "Forward" with lines lt rgb "green" , \
     "bwd" title "Backward" with lines lt rgb "red", \
     "rnd" title "Shuffle" with lines lt rgb "blue"
     
