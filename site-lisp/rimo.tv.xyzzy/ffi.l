;;; -*- mode: lisp; package: rimo-ffi -*-
;;;
;;; File: rimo.tv.xyzzy/ffi.l
;;;
;;;   Copyright (c) 2007 MIYAMUKO Katsuyuki.
;;;
;;;   See rimo.tv.xyzzy/docs/MIT-LICENSE for full license.
;;;

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "rimo.tv.xyzzy/package"))

(in-package :rimo-ffi)

(require "rimo.tv.xyzzy/ffi/macros")
(require "rimo.tv.xyzzy/ffi/user32")
(require "rimo.tv.xyzzy/ffi/rimo")

(provide "rimo.tv.xyzzy/ffi")

;;; End
