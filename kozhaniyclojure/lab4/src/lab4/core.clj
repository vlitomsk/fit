; Генерация слов (Clojure 1)
; cd trpo
; lein repl
; (load-file "lab4/src/lab4/core.clj")
; (lab4.core/main)
; (quit)

; cd lab4
; lein test

(ns lab4.core)

(defn generate
  "generate генерирует список из слов (списков) заданной длины из заданного алфавита.
  Список `choices` -- алфавит.
  `n_left` -- длина слов."
  [choices n_left]
  (:pre [
    (>= n_left 0)
    (not (and ; ситуация, когда на длинный хвост генерируемого слова остался 1 или не осталось символов из алфавита.
      (<= (count choices) 1) 
      (> n_left (- (count choices) 1))))])
  (cond 
    (= n_left 0) '()
    (= n_left 1) (map #(cons % nil) choices)
    true 
    (for [tail (generate choices (dec n_left))
        head choices
        :when (not= head (first tail))]
      (cons head tail))))

(defn main [& args]
  (generate (seq "ab") 5))