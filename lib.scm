(use-modules (ice-9 syncase))
(use-modules (ice-9 format))

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
                      (display (format "~15a" j)))
              (display i))
          (newline)))
