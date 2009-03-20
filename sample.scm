(config
 '((N  100000)
   (B  1.2)
   (R  5)
   (T  3)
   (M  1.0)
   (P  0.1)
   (initGenome  0.5)
   (randomSeed 3456)))


(create-environment 40000)
(do-step 20)
(optimize-environment)

(dotimes (i 10)
  (do-step 50)
  (optimize-environment))

(print-list (get-genome-ranking))
