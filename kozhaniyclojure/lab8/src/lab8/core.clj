; Булевы выражения и ДНФ (Clojure 4)
; cd trpo
; lein repl
; (load-file "lab8/src/lab8/core.clj")
; (lab8.core/main)
; (quit)

; cd lab8
; lein test

(ns lab8.core)

(defn constant
  "Конструктор булевой константы `x` (0 или 1)"
  [x]
  {:pre [(or (= 0 x) (= 1 x))]}
  (list ::constant x))

(defn constant?
  "Проверка, является ли терм константой"
  [expr]
  (= ::constant (first expr)))

(defn constant-value
  "(Деконструкция) Извлечение значения константы из константного терма"
  [const]
  (second const))

(defn variable
  "Конструктор терма именованной переменной"
  [name]
  {:pre [(keyword? name)]}
  (list ::var name))

(defn variable?
  "Проверка, является ли терм именованной переменной"
  [expr]
  (= ::var (first expr)))

(defn op-and
  "Конструктор терма логического И"
  [expr & rest-expr]
  (cons ::and (cons expr rest-expr)))

(defn op-and?
  "Проверка, является ли терм логическим И"
  [expr]
  (= ::and (first expr)))

(defn op-or
  "Конструктор терма логического ИЛИ"
  [expr & rest-expr]
  (cons ::or (cons expr rest-expr)))

(defn op-or?
  "Проверка, является ли терм логическим ИЛИ"
  [expr]
  (= ::or (first expr)))

(defn op-neg
  "Конструктор терма логического НЕ"
  [expr]
  (list ::neg expr))

(defn op-neg?
  "Проверка, является ли терм логическим НЕ"
  [expr]
  (= ::neg (first expr)))

(defn op-then
  "Конструктор терма логической импликации"
  [from to]
  (cons ::then (list from to)))

(defn op-then?
  "Проверка, является ли терм импликацией"
  [expr]
  (= ::then (first expr)))

(defn args
  "Получение списка параметров терма"
  [expr]
  (rest expr))

(defn fst-arg
  "Специально для терма логического отрицания, получение внутреннего терма"
  [expr]
  (second expr))

(declare convert-to-dnf)

(def dnf-rules
  (list
    ; константы переводим как есть
    [(fn [expr] (constant? expr)) identity]
    
    ; переменные переводим как есть
    [(fn [expr] (variable? expr)) identity]
    
    ; отрицания констант и переменных (элементарные конъюнкты) переводим как есть
    [(fn [expr] 
       (and (op-neg? expr)
            (or
              (variable? (fst-arg expr))
              (constant? (fst-arg expr)))))

      identity]
    
    ; выше приоритетом стоит раскрытие "сложных" конструкций (импликации) до 
    ; базисных для ДНФ
    [(fn [expr] (op-then? expr))

     (fn [expr] 
       ;(println ">Unfold Implications for :: " expr)
       (let [arg (args expr)] 
          ;(println arg)
          (op-or 
            (convert-to-dnf (op-neg (first arg))) 
            (convert-to-dnf (second arg)))))]

    ; затем отрицания скобок
    [(fn [expr]
       (and (op-neg? expr)
            (or (op-or? (fst-arg expr))
                (op-and? (fst-arg expr)))))

     (fn [expr]
       ;(println ">De Morgan for :: " expr)
       (let [negated (convert-to-dnf (fst-arg expr))
             neg-args (map op-neg (args negated))]
       ;(println "negated: " negated)
       ;(println "neg-args:" neg-args)
       (if (op-and? negated)
         (apply op-or neg-args)
         (apply op-and neg-args))))]
    
    ; следующим по приоритету стоит избавление от множественных отрицаний одного члена
    [(fn [expr] 
       (and (op-neg? expr)
            (op-neg? (fst-arg expr))))

     (fn [expr] 
      ;(println ">Double Neg for :: " expr)
      (convert-to-dnf (fst-arg expr)))]

    ; затем раскрытие скобок по дистрибутивности
    [(fn [expr] 
       (and (op-and? expr)
            (some op-or? (args expr))))

     (fn [expr]
        ;(println ">Distrib for :: " expr)
        (let 
           [dnf-args (map convert-to-dnf (args expr))
            r2 (filter #(not (op-or? %)) dnf-args) ; список дизънктов
            r4 (filter op-or? dnf-args) ; список полновесных днф
            f1 (fn [disjunct] (if (op-and? disjunct) (args disjunct) (cons disjunct nil)))
            just-el-conjuncts-sets (cons (mapcat f1 r2) nil) ; список эл. конъюнктов из корня expr
            ors-disjuncts-sets (map #(map f1 (args %)) r4)
            places (concat (map list just-el-conjuncts-sets) ors-disjuncts-sets)
            my-reducer (fn [acc x] 
             (for [ds1 acc 
                   ds2 x] (concat ds1 ds2)))
            xxx (reduce my-reducer places) ; массив массивов элементраных конъю
            expr-new (apply op-or (map #(apply op-and %) xxx))
           ]
           ;(println ">> " expr-new)
           expr-new))]

    ; обычное раскрытие "И" по индукции
    [(fn [expr] (op-and? expr))
    
     (fn [expr]
       ;(println ">Default And for :: " expr)
       (apply op-and (map convert-to-dnf (args expr))))]

    ; обычное раскрытие "ИЛИ" по индукции
    [(fn [expr] (op-or? expr))
    
     (fn [expr]
       ;(println ">Default Or for :: " expr)
       (apply op-or (map convert-to-dnf (args expr))))]

    ; обычное раскрытие "НЕ" по индукции
    [(fn [expr] (op-neg? expr))
     (fn [expr]
      ;(println ">Default Not for :: " expr)
      (op-neg (convert-to-dnf (fst-arg expr))))]

    ; предохранитель на всякий случай (которого нет...)
    [(fn [expr] true)
     (fn [expr]
       ;(println ">DEFAULT for :: " expr)
       (expr))]))


(defn convert-to-dnf-iter
  [expr]
  ((some #(and ((first %) expr) (second %))
         dnf-rules)
   expr))

(defn convert-to-dnf
  "Преобразование термов в ДНФ"
  [expr]
  (convert-to-dnf-iter
    (convert-to-dnf-iter
      (convert-to-dnf-iter
        (convert-to-dnf-iter expr)))))


(declare evaluate-dnf-iter)

(def eval-dnf-rules
  (list
    ; константы интерпретируем как есть
    [(fn [expr] (constant? expr))
     (fn [expr vars-vals-map] expr)]

    ; переменные пытаемся заменять значениями из поданного снаружи словаря
    [(fn [expr] (variable? expr))
     (fn [expr vars-vals-map] 
       ;(println ">Try replace variable " expr)
       (let [lookup-result (get vars-vals-map (first (args expr)) expr)]
        lookup-result))]

    ; сразу интерпретируем отрицания констант
    [(fn [expr] (and (op-neg? expr)) (constant? (fst-arg expr)))
     (fn [expr vars-vals-map] (constant (- 1 (constant-value (fst-arg expr)))))]

    ; избавляемся от тавтологий типа (0|smth) , (1&smth), сворачиваем все константы под И и ИЛИ.
    [(fn [expr] (or (op-and? expr) (op-or? expr)))
     (fn [expr vars-vals-map]
       ;(println ">Default And/Or for :: " expr)
       (let [processed-args (map #(evaluate-dnf-iter % vars-vals-map) (args expr))
             not-consts (filter #(not (constant? %)) processed-args)
             consts (map constant-value (filter constant? processed-args))
             killer-const (if (op-and? expr) 0 1)
             op-func (if (op-and? expr) op-and op-or)]
       (if (some #(= killer-const %) consts) 
          (constant killer-const)
          (if (and (empty? not-consts) (every? #(= (- 1 killer-const %)) consts))
            (constant (- 1 killer-const))
            (if (= 1 (count not-consts))
              (first not-consts)
              (apply op-func not-consts))))))]))

(defn evaluate-dnf-iter
  [dnf-expr vars-vals-map]
  ((some #(and ((first %) dnf-expr) (second %))
         eval-dnf-rules)
   dnf-expr vars-vals-map))

(defn evaluate-expr
  "Подставить из мапы значения переменных и сократить все константы"
  [expr vars-vals-map]
  ; 3 итераций хватит -- подстановка значений, схлопывание and-ов , схлопывание or-ов.
  (evaluate-dnf-iter 
    (evaluate-dnf-iter
      (evaluate-dnf-iter 
        (convert-to-dnf expr) 
        vars-vals-map) 
      vars-vals-map) 
    vars-vals-map))

(defn main [& args]
  (println "See tests!"))