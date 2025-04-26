(begin
  (define x 20)
  (init-window 600 800)
  (define (frame) (
      if (window-should-close) 0 (begin
          (begin-drawing)
          (clear 200 0 0)
          (rect x 50 20 20 0 255 0)
          (define x (+ x 0.01))
          (end-drawing)
          (frame)
        )
  ))
  (frame)

)
