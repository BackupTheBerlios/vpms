
(define (welcome)
  (newline)
  (display "Pena Model parameters:")
  (newline))


(define config
  '((N  1000)
    (B  1.2)
    (R  4)
    (T  2)
    (M  1.0)
    (P  0.0)
    (initGenome  0.5)
    (rndSeed 3456)))



(welcome)
(read-config)
