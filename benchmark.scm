(config
 '((N  100000)
   (B  1.2)
   (R  5)
   (T  3)
   (M  1.0)
   (P  0.1)
   (initGenome  0.5)
   (randomSeed 3456)))

(logging #f)
(create-environment 40000)
(do-step 20)
(optimize-environment)

(dotimes (i 100)
  (do-step 10)
  (optimize-environment))

(print-list (get-genome-ranking))
