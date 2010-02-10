(load "lib.scm")
(load "gnuplot.scm")
(load "charts.scm")
(use-modules (vpms gnuplot))

;;;;;;;
; user config section
;;;;;;;



(use-modules (ice-9 readline))
(activate-readline)

(define (get-clusters)
  (let ((data (_vpms-get-clusters)))
        (map (lambda (i)
               (let* ((x (list-ref i 0))
                      (y (list-ref i 1))
                      (z (/ y x)))
                 `(,x ,y ,z)))
             data)))

(define (get-clusters-histogram . hsize_list)
  (let* ((cmax (caar (get-genome-ranking)))
         (hsize (if (null? hsize_list) 100 (car hsize_list)))
         (rawhist (_vpms-get-clusters-histogram hsize)))
    (map (lambda (p)
           (let* ((x (list-ref p 0))
                  (y (list-ref p 1))
                  (z (* (* x 1.0) (/ cmax (* hsize 1.0)))))
             `(,z ,y)))
         rawhist)))

    

(define (do-avg-step . n)
  (apply do-step (append n '(#t))))

(define (welcome)
  (newline)
  (display "Welcome to Pena Model Simulation")
  (newline)
  (newline)
  (newline))


(welcome)

(config
 '((N  10000)
   (B  1.2)
   (R  6)
   (T  3)
   (M  1.0)
   (P  0.0)
   (initGenome  0.5)
   (randomSeed 3456)))

