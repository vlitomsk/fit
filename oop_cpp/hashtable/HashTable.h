#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include <string>
#include <vector>
#include <utility>

#include <iostream>

typedef std::string Key;

struct Value {
  unsigned age;
  unsigned weight;
};
bool operator==(const Value& v1, const Value& v2);
bool operator!=(const Value& v1, const Value& v2);

//typedef size_t Value;

#include <memory>

typedef enum {EXCEPTION_NOTFOUND, EXCEPTION_UNREACHABLE} htexception_t;
class HashTable
{
public:
  HashTable() 
    : cont(std::vector< pcell_t >(INITIAL_SIZE)),
      aliveEntries(0) { }

  ~HashTable() { }

  HashTable(const HashTable& b) 
    : aliveEntries(b.aliveEntries), cont(b.cont) { }

  void swap(HashTable& b);

  HashTable& operator=(const HashTable& b);
  bool equals(const HashTable& b) const;

  /* Очищает контейнер */
  void clear();
  
  /*
    Удаляет элемент по заданному ключу.
    Возвращает false, если такого элемента не существовало
    иначе возвращает true
  */
  bool erase(const Key& k);
  // Вставка в контейнер. Возвращаемое значение - успешность вставки.

  /* 
    Вставляет элемент в HashMap
    Если элемент с таким ключом уже существует, то Value перезаписывается,
      и false возвращается;
    Если элемента с таким ключом не существует, то он создается,
      и true возвращается.
  */
  bool insert(const Key& k, const Value& v);

  /* 
    Возвращает значение по ключу. 
    Небезопасный метод. (Может разыменоваться nullptr) 
  */
  Value& operator[](const Key& k);
  const Value& operator[](const Key& k) const;

  /* 
    Возвращает значение по ключу. 
    Бросает исключение, если элемент не найден 
  */
  Value& at(const Key& k);
  const Value& at(const Key& k) const;

  /* Возвращает aliveEntries */
  size_t size() const;

  /* aliveEntries == 0 */
  bool empty() const;
private:

  /* 
    Количество записей в HashMap, 
    всегда выполнеяется aliveEntries < (CRITICAL_NOM * cont.size() ) / CRITICAL_DENOM.
    При достижении предельного количества записей размер вектора cont
    увеличивается в 2 раза с помощью doubleSize(void)
  */
  size_t aliveEntries; 
  typedef enum {ST_BUSY, ST_DELETED} cellstate_t;
  struct cell_t {
    cell_t(const Key& key, const Value& value, cellstate_t state)
      : k(std::make_shared<Key>(key)), v(std::make_shared<Value>(value)), st(state) {}
    std::shared_ptr<Key> k;
    std::shared_ptr<Value> v;
    cellstate_t st;
  };

  typedef std::shared_ptr<cell_t> pcell_t;

  /* Основной контейнер для хранения записей */
  std::vector< pcell_t > cont;

  /*
    Методы, реализующие пробирование (в данном случае двойное хэширование)
    В случае, если такого ключа не найдено, возвращается 
      ссылка на shared_ptr, равный nullptr 
    Если ключ найден, возвращается ссылка на эту запись с ST_BUSY
    Если deletedOk == true, то при отсутствии записи с таким ключом,
      возвращается ссылка на ячейку с ST_DELETED либо nullptr 
      (в зависимости от того, что раньше попадется при пробировании).
  */  
  pcell_t& getCellRef(const Key& k, bool deletedOk);
  const pcell_t& getCellConstRef(const Key& k, bool deletedOk) const;

  /* 
    Увеличивает размер вектора в 2 раза, "перевставляет" в новый вектор
    ранее добавленные записи (при этом они переупорядочиваются)
  */
  void doubleSize(void);

  /* 
    Методы для хэширования ключей. 
    Значение хэша в пределах от 0 до cont.size() - 1 включительно
    keyhash использует std::hash
    keyhash2 свертывает ключ по алгоритму DJB-2, 
      возвращая число взаимно простое с cont.size()
  */
  size_t keyhash(const Key& k) const;
  size_t keyhash2(const Key& k) const;

  /*
    INITIAL_SIZE -- начальный размер вектора cont, должен быть степенью двойки
    CRITICAL_NOM, CRITICAL_DENOM -- см. doubleSize(void), aliveEntries
  */
  static const size_t INITIAL_SIZE = 16, CRITICAL_NOM = 3, CRITICAL_DENOM = 4;
};

bool operator==(const HashTable & a, const HashTable & b);
bool operator!=(const HashTable & a, const HashTable & b);

#endif

