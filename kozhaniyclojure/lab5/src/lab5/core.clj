; Интегрирование с мемоизацией (Clojure 2)
; cd trpo
; lein repl
; (load-file "lab5/src/lab5/core.clj")
; (lab5.core/main)
; (quit)

; cd lab5
; lein test

(ns lab5.core)

(defn trap-area
  [a b height]
  (* 0.5 (+ a b) height))

(defn integrate-helper
  "Посчитать интеграл функции `f` в пределах от 0 до `x` с шагом `step`.
  Также передается параметр `f-x-val` -- значение f(x), это используется для того,
  чтобы не считать значение в точке два раза.
  `integrate-rest` -- синтетический параметр, для передачи мемоизованного integrate-helper."
  [f step x f-x-val integrate-rest] 
  (:pre [(>= x 0)])
  (if (< x step)
    0
    ; f-x-val = f(x_i)
    ; f-x-left-val = f(x_i-1)
    (let [f-x-left-val (f (- x step))
          trarea (trap-area f-x-val f-x-left-val step)] 
     (+
      trarea
      (integrate-rest f step (- x step) f-x-left-val integrate-rest)))))

; используем def, чтобы создать кэш значений единожды, 
; а не при каждом обращении к memoized.
(def memoized (memoize integrate-helper)) 

(defn integrate
  "Посчитать интеграл функции `f` в пределах от 0 до `x` с шагом `step`."
  [f step x]
  (integrate-helper f step x (f x) memoized))

(defn test-func [x]
    (Math/sin (Math/sin (Math/sin (Math/sin (Math/sin x))))))

(defn main [& args]
  (println "Mnogo vremeni:")
  (time (integrate test-func 0.1 5)) 
  (println "Malo vremeni:")
  (time (integrate test-func 0.1 5.1))
  (println "Mnogo vremeni:")
  (time (integrate test-func 0.1 10))
  (println "Malo vremeni:")
  (time (integrate test-func 0.1 10.1)))
