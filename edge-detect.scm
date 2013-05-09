 ; Usage: first place in .gimp-2.8/scripts
 ; Then call it with the syntax $ gimp -i -b '(edge-detect "dog.jpg" 10 255)' -b '(gimp-quit 0)'
 ; The 10 and 25 are for white value low and high thresholds.  Feel free to change these as you
 ; see fit.

 (define (edge-detect filename low high)
   (let* ((image (car (gimp-file-load RUN-NONINTERACTIVE filename filename)))
          (drawable (car (gimp-image-get-active-layer image))))
     (plug-in-laplace RUN-NONINTERACTIVE image drawable)
     (gimp-hue-saturation drawable 0 0.0 0.0 -100.0)
     (gimp-threshold drawable low high)
     ; (gimp-brightness-contrast drawable 0 contrast)
     (gimp-file-save RUN-NONINTERACTIVE image drawable "result.jpg" "result.jpg")
     (gimp-image-delete image)))

(define (edge-batch pattern low high)
(let* ((filelist (cadr (file-glob pattern 1))))
(while (not (null? filelist))
       (let* ((filename (car filelist))
              (image (car (gimp-file-load RUN-NONINTERACTIVE
                                          filename filename)))
              (drawable (car (gimp-image-get-active-layer image))))
        (plug-in-laplace RUN-NONINTERACTIVE image drawable)
        (gimp-hue-saturation drawable 0 0.0 0.0 -100.0)
        (gimp-threshold drawable low high)
         (gimp-file-save RUN-NONINTERACTIVE
                         image drawable filename (string-append (substring filename (- (string-length filename) 3) "jpg"))
         (gimp-image-delete image))
       (set! filelist (cdr filelist)))))
