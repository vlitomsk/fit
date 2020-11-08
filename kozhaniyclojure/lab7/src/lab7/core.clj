; Параллельный фильтр (Clojure 3-2)
; cd trpo
; lein repl
; (load-file "lab7/src/lab7/core.clj")
; (lab7.core/main)
; (quit)

; cd lab7
; lein test

(ns lab7.core)

(defn pfilter-superblock
  "Разбивает входную коллекцию `coll`(суперблок) на блоки заданного
   размера `n_elems_per_block`, 
   параллельно фильтрует их, 
   дожидается окончания фильтрации их всех, 
   и склеивает результаты фильтрации в возвращаемый ленивый список."
  [n_elems_per_block pred coll]
  (let [parts (partition n_elems_per_block n_elems_per_block nil coll)
        ; самый внутренний doall -- для того, чтобы это была действительно многопоточная фильтрация, 
        ;                           а не многопоточное создание ленивых последовательностей
        ;                           и их разворачивание в каких-то других потоках.
        ; самый внешний doall -- для того, чтобы все футуры создались и запустилось НЕСКОЛЬКО потоков.
        futures (doall (map #(future (doall (filter pred %))) parts))]  
    (reduce concat (map deref futures))))

(defn pfilter
  "Параллельный фильтр разбивает входную коллекцию `coll` на
  суперблоки блоков и фильтрует блоки внутри суперблока параллельно.
  `n_max_futures_per_superblock` -- макс. количество блоков внутри суперблока
                                    и макс. степень паралеллизма.
  `n_elems_per_block` -- желаемое количество элементов коллекции в блоке."
  [n_max_futures_per_superblock n_elems_per_block pred coll]
  (:pre [
    (> n_max_futures_per_superblock 0)
    (> n_elems_per_block 0)])
  (if (empty? coll) 
    '()
    (let [n_fetch_elems (* n_max_futures_per_superblock n_elems_per_block)
          body (take n_fetch_elems coll)  ; макс. n_fetch_elems будет достато.
          tail (drop n_fetch_elems coll)] ; а здесь -- уж сколько останется после доставания..
      (concat 
        (pfilter-superblock n_elems_per_block pred body)
        (lazy-seq (pfilter n_max_futures_per_superblock n_elems_per_block pred tail))))))

(def nat (map inc (iterate inc 0))) ; натуральные числа для тестов

(defn test-pred 
  [x]
  (Thread/sleep 1)
  (> 0.5 (Math/random)))

(defn main [& args]
  ; второй вызов примерно в полтора-два раза быстрее первого.
  ;(println "Mnogo vremeni:")
  ;(time (doall (pfilter-superblock 2000 test-pred (take 2000 nat))))
  ;(println "Malo vremeni:")
  ;(time (doall (pfilter-superblock 1000 test-pred (take 2000 nat)))))
  (println "Mnogo vremeni:")
  (time (doall (take 2000 (pfilter 1 1000 test-pred nat))))
  (println "Malo vremeni:")
  (time (doall (take 2000 (pfilter 2 1000 test-pred nat)))))