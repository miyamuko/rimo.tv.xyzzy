;;; -*- mode: lisp; package: rimo-ffi -*-
;;;
;;; File: rimo.tv.xyzzy/ffi/macros.l
;;;
;;;   Copyright (c) 2007 MIYAMUKO Katsuyuki.
;;;
;;;   See rimo.tv.xyzzy/docs/MIT-LICENSE for full license.
;;;

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "rimo.tv.xyzzy/package"))

(in-package :rimo-ffi)

(defun no-conversion (name)
  name)

(defmacro define-library (macroname dll converter)
  `(progn
     (defmacro ,macroname (return-type export-name (&rest args-with-name))
       (let ((name (funcall ,converter export-name))
             (args (mapcar #'car args-with-name)))
         `(c:*define-dll-entry ,return-type ,name ,args
            ,,dll ,(string export-name))))
     (setf (get ',macroname 'ed:lisp-indent-hook) 'defun)))


(provide "rimo.tv.xyzzy/rimo/ffi/macros")

;;; End
