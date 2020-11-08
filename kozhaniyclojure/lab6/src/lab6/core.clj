; Интегрирование с бесконечными списками (Clojure 3-1)
; cd trpo
; lein repl
; (load-file "lab6/src/lab6/core.clj")
; (lab6.core/main)
; (quit)

; cd lab6
; lein test

(ns lab6.core)

(defn partial-sums
  "Строит потенциально бесконечную последовательность частичных сумм
   последовательности `seq`, начиная с суммы=0."
  ([seq] 
    (partial-sums seq 0))
  ([seq acc]
    (if (empty? seq) 
      '()
      (let [new-acc (+ acc (first seq))]
       (lazy-seq (cons new-acc  
                       (partial-sums (next seq) new-acc)))))))

(defn trap-area
  [a b height]
  (* 0.5 (+ a b) height))

(defn trap-area-seq
  "Строит бесконечную последовательность площадей трапеций
   функции `f` с шагом `step`, начиная с x=0"
  ([f step]
    (trap-area-seq f step 0 (f 0)))
  ([f step idx f-at-idx]
    (let [f-at-next-idx (f (* step (inc idx)))]
    (lazy-seq (cons (trap-area f-at-idx f-at-next-idx step)
                    (trap-area-seq f step (inc idx) f-at-next-idx))))))

(defn integrals-seq
  "Строит бесконечную последовательность интегральных сумм
   функции `f` c шагом `step`, начиная с x=0"
  [f step]
  (partial-sums (trap-area-seq f step)))

; Запоминаем для каждой пары (f;step) бесконечную последовательность интегральных сумм.
; Со временем в каждой такой ленивой последовательности будет увеличиваться вычисленная часть.
(def integrals-seq-memoized (memoize integrals-seq))

(defn integrate-helper
  ([integ-seq step x]
    (nth integ-seq
         (dec (int (/ x step))))))

(defn integrate
  "Считает интеграл функции `f` с шагом `step`, начиная с x=0."
  ([f step x]
    ; можно обойтись и не-memoized версией, но тогда для того, чтобы заметить
    ; прирост производительности при повторном вычислении интеграла до x_prev + (delta>0),
    ; пришлось бы def-ом запоминать последовательность интегральных сумм для данных конкретных f и step.
    (integrate-helper (integrals-seq-memoized f step) step x)))

(defn test-func [x]
    (Math/sin (Math/sin (Math/sin (Math/sin (Math/sin x))))))

(defn main [& args]
  (println "Mnogo vremeni:")
  (time (integrate test-func 0.01 2))
  (println "Malo vremeni:")
  (time (integrate test-func 0.01 2.03))
  (println "Mnogo vremeni:")
  (time (integrate test-func 0.01 10))
  (println "Malo vremeni:")
  (time (integrate test-func 0.01 10.1)))