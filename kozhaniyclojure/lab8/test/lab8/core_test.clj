(ns lab8.core-test
  (:require [clojure.test :refer :all]
            [lab8.core :refer :all]))

(deftest bool-expr-construction-test
  (testing "Testing boolean expressions construction correctness"
    (is (constant? (constant 1)))
    (is (= 0 (constant-value (constant 0))))
    (is (variable? (variable :a)))
    (is (op-and? (op-and (variable :a) (constant 0))))
    (is (op-or? (op-or (variable :b) (constant 1))))
    (is (op-neg? (op-neg (variable :c))))
    (is (op-then? (op-then (constant 0) (constant 1))))))

(deftest dnf-test
	(testing "Testing Expr->DNF conversion"
		(is (= (convert-to-dnf (op-and (op-or (variable :x) (variable :y)) (op-or (variable :a) (variable :b))))
		       '(:lab8.core/or 
		       		(:lab8.core/and 
		       			(:lab8.core/var :x) 
		       			(:lab8.core/var :a)) 
		       		(:lab8.core/and 
		       			(:lab8.core/var :x) 
		       			(:lab8.core/var :b)) 
		       		(:lab8.core/and 
		       			(:lab8.core/var :y) 
		       			(:lab8.core/var :a)) 
		       		(:lab8.core/and 
		       			(:lab8.core/var :y) 
		       			(:lab8.core/var :b)))))))

(deftest eval-test
	(testing "Testing bool evaluation"
		(is (= (evaluate-expr (op-and (op-or (variable :x) (variable :y)) (op-or (variable :a) (variable :b)))
		                      {:x (constant 0) :y (constant 1)})
				'(:lab8.core/or (:lab8.core/var :a) (:lab8.core/var :b))))))