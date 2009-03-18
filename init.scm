(use-modules (ice-9 syncase))
(use-modules (ice-9 format))
(use-modules (ice-9 readline))
(activate-readline)


(define (welcome)
  (newline)
  (display "Welcome to Pena Model Simulation")
  (newline)
  (newline)
  (newline))

(define-syntax dolist
  (syntax-rules ()
    ((dolist (var vlist) body ...)
     (for-each (lambda (var) body ...) vlist))))

(define-syntax dotimes
  (syntax-rules ()
    ((dotimes (var count) body ... )
     (let loop ((var 0))
         (cond ((< var count)
                body ...
               (loop (+ var 1))))))))

(define (print-list lst)
  (dolist (i lst)
          (if (list? i)
              (dolist (j i)
                      (display (format "~10a" j)))
              (display i))
          (newline)))


(welcome)

(config
 '((N  1000)
   (B  1.2)
   (R  4)
   (T  2)
   (M  1.0)
   (P  0.0)
   (initGenome  0.5)
   (rndSeed 3456)))

