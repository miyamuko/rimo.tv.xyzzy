;;; -*- mode: lisp; package: rimo-ffi -*-
;;;
;;; File: rimo.tv.xyzzy/ffi/ffi.l
;;;
;;;   Copyright (c) 2007 MIYAMUKO Katsuyuki.
;;;
;;;   See rimo.tv.xyzzy/docs/MIT-LICENSE for full license.
;;;

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "rimo.tv.xyzzy/package")
  (require "rimo.tv.xyzzy/ffi/macros"))

(in-package :rimo-ffi)

(defparameter *load-directory* (directory-namestring *load-pathname*))

(eval-when (:compile-toplevel :load-toplevel :execute)
  (defun rimo-dll ()
    (merge-pathnames "xyzzy-rimo.dll" *load-directory*))

  (defun rimo-map-c-function (name)
    (labels ((gsub (str sublst)
               (if sublst
                   (gsub (substitute-string str (caar sublst) (cdar sublst))
                         (cdr sublst))
                 str)))
      (intern (gsub (string name)
                    '(("\\([A-Z][a-z0-9_]+\\)" . "\\L\\1-")
                      ("-$" . "")
                      ("^xy-" . "%")
                      ("_" . "-")))))))

(define-library define-rimo-api
                '(rimo-dll)
                'rimo-map-c-function)

(define-rimo-api
  DWORD
  XyRimoGetLastError ())

(define-rimo-api
  HWND
  XyRimoGetWindow ())

(define-rimo-api
  BOOL
  XyRimoUpdateWindow ())

(define-rimo-api
  BOOL
  XyRimoShowWindow ())

(define-rimo-api
  BOOL
  XyRimoHideWindow ())

(define-rimo-api
  BOOL
  XyRimoMoveWindow ((c:int x)
                    (c:int y)
                    (c:int w)
                    (c:int h)))

(define-rimo-api
  DWORD
  XyRimoCloseWindow ())

(define-rimo-api
  HWND
  XyRimoCreateWindow ((HWND parent)
                      (c:int x)
                      (c:int y)
                      (c:int w)
                      (c:int h)))


(provide "rimo.tv.xyzzy/rimo/ffi/rimo")

;;; End
