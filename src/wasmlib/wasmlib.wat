(module
;;(import "console" "log" (func $log (param i32)))
;; (import "String" "fromCharCode" (func $toChar (param i32)))
(import "console" "log" (func $log (param i32 i32)))

;; 10 64kB pages of memory
(memory (export "memory") 1 10)

(func (export "print") (param $offset i32) (param $size i32)
  local.get $offset
  local.get $size
  call $log)
;; (func (export "toChar")
    ;; i32.const 13
    ;; call $toChar)

;; https://developer.mozilla.org/en-US/docs/WebAssembly/Understanding_the_text_format
;;(table 2 funcref)
;;  (func $f1 (result i32)
;;    i32.const 42)
;;  (func $f2 (result i32)
;;    i32.const 13)
;;  (elem (i32.const 0) $f1 $f2)
;;  (type $return_i32 (func (result i32)))
;;  (func (export "callByIndex") (param $i i32) (result i32)
;;    local.get $i
;;    call_indirect (type $return_i32))
)
