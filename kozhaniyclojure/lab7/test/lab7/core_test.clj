(ns lab7.core-test
  (:require [clojure.test :refer :all]
            [lab7.core :refer :all]))

(deftest a-test
  (testing "test"
    (let [seq (take 1000 nat)
          flt1 (filter even? seq)
          flt2 (pfilter 2 10 even? seq)]
      (is (= flt1 flt2)))))
