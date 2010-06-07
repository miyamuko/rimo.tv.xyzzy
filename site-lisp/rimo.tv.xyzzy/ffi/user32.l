;;; -*- mode: lisp; package: rimo-ffi -*-
;;;
;;; File: rimo.tv.xyzzy/ffi/user32.l
;;;
;;;   Copyright (c) 2007 MIYAMUKO Katsuyuki.
;;;
;;;   See rimo.tv.xyzzy/docs/MIT-LICENSE for full license.
;;;

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "rimo.tv.xyzzy/package")
  (require "rimo.tv.xyzzy/ffi/macros"))

(in-package :rimo-ffi)

(export '(get-client-rect
          client-to-screen))

(define-library define-user32-api
                "user32"
                'no-conversion)

(define-user32-api
  BOOL
  ClientToScreen ((HWND handle)
                  ((POINT *) point)))

(defun get-client-rect (hwnd)
  (let ((rect (winapi::make-RECT)))
    (winapi::GetClientRect hwnd rect)
    (list (winapi::RECT-left rect)
          (winapi::RECT-top rect)
          (winapi::RECT-right rect)
          (winapi::RECT-bottom rect))))

(defun client-to-screen (hwnd)
  (let ((pt (winapi::make-POINT)))
    (setf (winapi::POINT-x pt) 0
          (winapi::POINT-y pt) 0)
    (when (not (zerop (ClientToScreen hwnd pt)))
      (list (winapi::POINT-x pt)
            (winapi::POINT-y pt)))))


(provide "rimo.tv.xyzzy/rimo/ffi/user32")

;;; End
