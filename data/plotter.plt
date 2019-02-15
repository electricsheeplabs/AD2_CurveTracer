set autoscale

set key left

plot	"zener3.txt" using 1:2 title "zener_3",\
	"hot1.txt" using 1:2 title "hot",\
	"hot2.txt" using 1:2 title "hotter",\
	"hot3.txt" using 1:2 title "reallyHot",\
	"hot4.txt" using 1:2 title "soooHot",\
	"hot5.txt" using 1:2 title "veryHott",\
	"hot6.txt" using 1:2 title "tremendouslyHot"
