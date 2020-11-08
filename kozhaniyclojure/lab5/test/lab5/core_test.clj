(ns lab5.core-test
  (:require [clojure.test :refer :all]
            [lab5.core :refer :all]))

(defn eql? 
	[eps a b]
	(:pre [(> eps 0)])
	(< (Math/abs (- a b)) eps))

(deftest a-test
  (testing "test"
    (is (eql? 0.01 (integrate (fn [x] (* x x)) 0.01 2) 2.666))))
