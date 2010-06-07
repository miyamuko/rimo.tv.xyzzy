;;; -*- mode: lisp; package: user -*-
;;;
;;; File: rimo.tv.xyzzy/ffi/package.l
;;;
;;;   Copyright (c) 2007 MIYAMUKO Katsuyuki.
;;;
;;;   See rimo.tv.xyzzy/docs/MIT-LICENSE for full license.
;;;

(in-package :editor)

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "foreign")
  (require "wip/winapi")

  (defpackage :rimo-ffi
    (:use
     :lisp :editor :foreign :winapi
     ))

  (defpackage :rimo
    (:use
     :lisp :editor
     :rimo-ffi
     )))


(provide "rimo.tv.xyzzy/package")

;;; End
