(module 
;;(module
;;(import "console" "log" (func $log (param i32)))
;; (import "String" "fromCharCode" (func $toChar (param i32)))
(import "console" "log" (func $log (param i32 i32)))
(import "math" "add" (func $add (param i32 i32) (result i32)))
(import "math" "sub" (func $sub (param i32 i32) (result i32)))
(import "math" "mul" (func $mul (param i32 i32) (result i32)))
(import "math" "div" (func $div (param i32 i32) (result i32)))
(import "math" "mod" (func $mod (param i32 i32) (result i32)))
(import "math" "eq" (func $eq (param i32 i32) (result i32)))
(import "math" "neq" (func $neq (param i32 i32) (result i32)))
(import "math" "lt" (func $lt (param i32 i32) (result i32)))
(import "math" "gt" (func $gt (param i32 i32) (result i32)))
(import "math" "lte" (func $lte (param i32 i32) (result i32)))
(import "math" "gte" (func $gte (param i32 i32) (result i32)))
(import "math" "not" (func $not (param i32 i32) (result i32)))
(import "math" "or" (func $or (param i32 i32) (result i32)))
(import "math" "and" (func $and (param i32 i32) (result i32)))

;; 10 64kB pages of memory
;;(memory (export "memory") 1 10)
(import "js" "memory" (memory 10))

(func $writeln_string (param $offset i32)
  ;; (param $size i32)
  local.get $offset
  ;; local.get $size
  i32.const 20
  call $log)
;; (func (export "toChar")
    ;; i32.const 13
    ;; call $toChar)

(data (i32.const 10) "TEST")

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
;;)

(func (export "main") 
 i32.const 10
 call $writeln_string
)
)