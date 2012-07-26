#lang racket/base

;;; rackffi.scm
;;; Our test for the Client

; We need the FFI libraries.

(require ffi/unsafe
         ffi/unsafe/define
         setup/dirs)

; Set up rdbus-define to be a link to our sample library
(define-ffi-definer gio-define (ffi-lib "libgio-2.0"))
(define-ffi-definer glib-define (ffi-lib "libglib-2.0"))
(define-ffi-definer gobj-define (ffi-lib "libgobject-2.0"))
; (define-ffi-definer rdbus-define (ffi-lib "rdbus"))

; An interface to 
;   void rdbus_init (void)
;(rdbus-define rdbus_init (_fun -> _void))

; Okay, we need pointers to GDBusProxy objects
;(define _GDBusProxy (_cpointer 'GDBusProxy))

;(rdbus-define rdbus_get_object (_fun _string _string _string -> _GDBusProxy)) 

;(define ramstest (rdbus_get_object "edu.grinnell.cs.glimmer.RAMStest"
;                                   "/edu/grinnell/cs/glimmer/RAMStest"
;                                   "edu.grinnell.cs.glimmer.RAMStest"))

(load-extension "/home/zarni/RDBus/rdbus.so")
(rdbus_init)
(define is (rdbus-get-object "edu.grinnell.cs.glimmer.RAMStest" "/edu/grinnell/cs/glimmer/RAMStest" "edu.grinnell.cs.glimmer.RAMStest"))
(rdbus-call-method is "v2v" '())
