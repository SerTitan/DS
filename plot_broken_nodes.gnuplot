# ─── запуск ───
#   gnuplot -e 'ALG="singlecast"' plot_broken_nodes.gnuplot
# ────────────────────────────────────────────────────────────
set terminal pngcairo size 1500,700 font ",10"
set output sprintf("results/broken_nodes_%s.png", ALG)

set datafile separator ","
unset key
set multiplot layout 2,5 title sprintf("Delivery vs dead nodes — %s", ALG)

# одинаковые диапазоны для всех подграфиков
set xrange [0:*]          # 0 … макс-t (авто)
set yrange [0:*]          # 0 … макс-delivered (авто)

do for [idx = 1:9] {      # 10,20,…,90 dead-nodes
    broken = idx*10
    fname  = sprintf("results/%s_broken%d.csv", ALG, broken)

	set title sprintf("%d nodes broken", broken)
	set xlabel "Time, s"
	set ylabel "Delivered, pcs"

	# t0 — первое время в файле
	stats fname using 1 nooutput
	t0 = STATS_min

	# ($1-t0)/1000  → секунды от начала
	# $0            → номер строки (0,1,2,…)  ==  кол-во доставленных
	plot fname using (($1 - t0)/1000.):($0) every ::1 \
			with lines lw 2 lc rgb "steelblue"
}
unset multiplot
