(define-module (vpms gnuplot))

(use-modules (ice-9 popen))
(use-modules (ice-9 syncase))

(define gnuplot-command "gnuplot")
(define gnuplot-port #f)

(define (init-gnuplot)
  (if (not gnuplot-port)
      (set! gnuplot-port (open-output-pipe gnuplot-command))))

(define-public (gnuplot-reset)
  (if gnuplot-port
      (begin
	(close-pipe gnuplot-port)
	(set! gnuplot-port #f))))

(define-public (plot-set param)
  (init-gnuplot)
  (display "set " gnuplot-port)
  (display param gnuplot-port)
  (newline gnuplot-port))

(define-syntax dolist
  (syntax-rules ()
    ((dolist (var vlist) body ...)
     (for-each (lambda (var) body ...) vlist))))

(define-syntax display-gnuplot-data
  (syntax-rules ()
    ( (display-gnuplot-data data params)
      (let ((tmpfile (tmpnam)))
        (dynamic-wind

          ;; create temporary file from data
          (lambda ()
            (call-with-output-file tmpfile
                (lambda (p)                  
                  (dolist (i data)
                          (dolist (j i)
                                  (display j p)
                                  (display " " p))
                          (newline p)))))
                  
          ;; execute body
          (lambda ()
            (display "plot '" gnuplot-port)
            (display tmpfile gnuplot-port)
            (display "' " gnuplot-port)
            (if (not (null? params))
                (display (car params) gnuplot-port))
            (newline gnuplot-port))
            

          ;; delete temporary file
          (lambda ()
            (add-hook! exit-hook
                       (lambda () (delete-file tmpfile)))))))))


(define-public (plot data . params)
  (init-gnuplot)
  (display-gnuplot-data data params))

