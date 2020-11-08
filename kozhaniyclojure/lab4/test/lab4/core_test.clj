(ns lab4.core-test
  (:require [clojure.test :refer :all]
            [lab4.core :refer :all]))

(defn no-doubles?
  "Проверяет, что в данном списке `elems` нет повторяющихся подряд значений."
  [elems]
  (or (< (count elems) 2)
    (let [[head & tail] elems]
      (and (not= head (first tail)) 
         (no-doubles? tail)))))

; проверить длину слов и алфавиты
(defn correctly-generated? [{words :words alpha :alpha len :len}]
  "Проверяет, что для данного алфавита `alpha` и длины `len` сгенерирован
  набор слов, соответствующих условиям задачи"
  (every? 
   (every-pred no-doubles? 
               #(= len (count %))
               (partial every? (set alpha))) 
   words))

(deftest a-test1
  (testing "1st"
    (let [test-fn (every-pred no-doubles? )]
      (is (every? correctly-generated?
          (for [alpha ["ab" "abc" "abcd" "abaa"] 
              len [0 1 2 3]] 
          {:words (generate (seq alpha) len)
           :alpha alpha
           :len len}))))))