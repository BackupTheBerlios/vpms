(load "gnuplot.scm")
(use-modules (vpms gnuplot))

(define *image-view-command* "display")

(define (chart-population)
  (plot-set "logscale y")
  (plot-set "xlabel \"age\"")
  (plot-set "ylabel \"population\"")
  (plot-set "grid")
  (plot-unset "key")
  (plot (get-avg-population) "with linespoints"))

(define (chart-mortality)
  (plot-set "logscale y")
  (plot-set "xlabel \"age\"")
  (plot-set "ylabel \"mortality\"")
  (plot-set "grid")
  (plot-unset "key")
  (plot (get-avg-mortality) "with linespoints"))

(define (chart-clusters)
  (plot-unset "logscale y")
  (plot-set "xlabel \"cluster volume\"")
  (plot-set "ylabel \"population\"")
  (plot-set "grid")
  (plot-unset "key")
  (plot (get-avg-clusters-histogram) "with boxes"))

(define (chart-genome)
  (plot-unset "logscale y")
  (plot-set "xlabel \"position\"")
  (plot-set "ylabel \"avg genome value\"")
  (plot-set "grid")
  (plot-unset "key")
  (plot (get-avg-genome) "with linespoints"))

(define (dump-4-charts fname)
  (plot-set "term png size 1024,768")
  (plot-set (string-concatenate `("output \"" ,fname "\"")))
  (plot-set "multiplot")
  (plot-set "size 0.5,0.5")
  
  (plot-set "origin 0.0,0.5")
  (chart-population)
  (plot-set "origin 0.5,0.5")
  (chart-mortality)
  (plot-set "origin 0.0,0.0")
  (chart-clusters)
  (plot-set "origin 0.5,0.0")
  (chart-genome)
  (plot-unset "multiplot")
  fname)



(define (save-current-data-chart . nsteps)
  (if (null? nsteps)
      (do-avg-step 20)
      (do-avg-step (car nsteps)))
  (let* ((cfg (get-config))
	 (N (assoc-ref cfg 'N))
	 (B (assoc-ref cfg 'B))
	 (R (assoc-ref cfg 'R))
	 (M (assoc-ref cfg 'M))
	 (T (assoc-ref cfg 'T))
	 (P (assoc-ref cfg 'P))
	 (cname (format "N~e-B~f-R~d-M~f-T~d-P~f-~6'0d" N B R M T P (get-time)))
	 (cname (string-delete cname #\+)))
    (dump-4-charts (string-concatenate
		    `("vp-" ,cname ".png")))))

(define (display-current-chart-and-calc-avg)
  (let ((fname (save-current-data-chart)))
    (usleep 200000) ;; wait for gnuplot (mu sec)
    (system (string-concatenate `(,*image-view-command* " " ,fname)))))
  