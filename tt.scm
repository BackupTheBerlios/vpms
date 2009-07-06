(load "charts.scm")

(config
 '((N  1e5)
   (B  1.2)
   (R  5)
   (T  3)
   (M  1.0)
   (P  0.0)
   (initGenome  0.5)
   (randomSeed 3456)))

(create-environment 4e6)

(dotimes (i 100)
          (do-step 10)
          (optimize-environment))

(dump-current-data 10)

(dotimes (i 10)
         (dotimes (j 10)
                  (do-step 100)
                  (optimize-environment))
         (dump-current-data 10))