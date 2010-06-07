;;; -*- mode: lisp; package: rimo -*-
;;;
;;; File: rimo.tv.xyzzy/rimo.l
;;;
;;;   Copyright (c) 2007 MIYAMUKO Katsuyuki.
;;;
;;;   See rimo.tv.xyzzy/docs/MIT-LICENSE for full license.
;;;

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "rimo.tv.xyzzy/package")
  (require "rimo.tv.xyzzy/ffi"))

(in-package :rimo)

(export '(*rimo-width*
          *rimo-height*
          *rimo-position*
          rimo.tv.on
          rimo.tv.off
          rimo.tv.show
          rimo.tv.hide
          rimo.tv.move
          rimo.tv.window))

(defvar *rimo-width* 400)
(defvar *rimo-height* 300)
(defvar *rimo-position* '((:right . 20)
                          (:bottom . 60)))

(defun rimo.tv.on ()
  (interactive)
  (multiple-value-bind (x y w h)
      (rimo-calc-window-position)
    (%rimo-create-window (get-window-handle) x y w h)))

(defun rimo.tv.off ()
  (interactive)
  (%rimo-close-window))

(defun rimo.tv.show ()
  (interactive)
  (%rimo-show-window))

(defun rimo.tv.hide ()
  (interactive)
  (%rimo-hide-window))

(defun rimo.tv.move (w h pos)
  (multiple-value-bind (x y ww hh)
      (rimo-calc-window-position-1 w h pos)
    (%rimo-move-window x y ww hh)))

(defun rimo.tv.window ()
  (%rimo-get-window))



;; utils

(defun rimo-calc-window-position ()
  (check-type *rimo-height* integer)
  (check-type *rimo-width* integer)
  (let* ((pos *rimo-position*)
         (x-pos (or (assoc :left pos) (assoc :right pos)))
         (y-pos (or (assoc :top pos) (assoc :bottom pos))))
    (check-type x-pos cons)
    (check-type y-pos cons)
    (check-type (cdr x-pos) integer)
    (check-type (cdr y-pos) integer)
    (rimo-calc-window-position-1 *rimo-width*
                                 *rimo-height*
                                 x-pos y-pos)))

(defun rimo-calc-window-position-1 (w h x-pos y-pos)
  (let ((rect (client-rect-absolute (get-window-handle))))
    (let ((r (list
              (rimo-calc-window-position-x rect w x-pos)
              (rimo-calc-window-position-y rect h y-pos)
              w h)))
      (when (or (member-if-not #'integerp r)
                (member-if-not #'plusp r))
        (plain-error "ê›íËÇ™ïsê≥Ç≈Ç∑ (width=~S, heigth=~S, position-x=~S, position-y=~S): åvéZåãâ (x y w h)=~S"
                     w h x-pos y-pos r))
      (values-list r))))

(defun rimo-calc-window-position-x (rect w pos)
  (case (car pos)
    (:left
     (+ (nth 0 rect) (cdr pos)))
    (:right
     (- (nth 2 rect) (cdr pos) w))))

(defun rimo-calc-window-position-y (rect h pos)
  (case (car pos)
    (:top
     (+ (nth 1 rect) (cdr pos)))
    (:bottom
     (- (nth 3 rect) (cdr pos) h))))

(defun client-rect-absolute (hwnd)
  (let ((rect (get-client-rect hwnd))
        (diff (client-to-screen hwnd)))
    (mapcar #'+ rect (append diff diff))))


(provide "rimo.tv.xyzzy/rimo")

;;; End
