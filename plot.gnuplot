set terminal png size 1800,900
set output '/logs/plot.png'
set datafile separator ","

set xlabel "Time, s"
set ylabel "Delivered messages, pcs"

set multiplot layout 2,5 title "Message propagation vs packet loss"

do for [i=0:9] {
    loss = i*10
    set title sprintf("%d%% packets lost", loss)
    plot sprintf("/logs/stats_%d.csv", loss) using 1:2 \
         with lines lw 2 title "Received"
}
unset multiplot
