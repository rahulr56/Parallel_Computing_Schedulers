$DATA << EOF
0 1507081020 1507081020
1 1507081020 1507081020
5 1507081020 1507081020
3 1507081020 1507081020
2 1507081020 1507081020
4 1507081020 1507081020
7 1507081020 1507081020
6 1507081020 1507081020
EOF

set terminal pdf
set output 'gant_static_sched_plots.pdf'

set xdata time
set format x "%b\n'%y"
set yrange [-1:]
OneMonth = strptime("%m","2")
set xtics OneMonth nomirror
set xtics scale 2, 0.5
set mxtics 4
set ytics nomirror
set grid x y
unset key
set title "{/=15 Simple Gantt Chart}\n\n{/:Bold Task start and end times in columns 2 and 3}"
set border 3

T(N) = timecolumn(N,timeformat)

set style arrow 1 filled size screen 0.02, 15 fixed lt 3 lw 1.5

plot $DATA using (T(2)) : ($0) : (T(3)-T(2)) : (0.0) : yticlabel(1) with vector as 1, \
         $DATA using (T(2)) : ($0) : 1 with labels right offset -2

