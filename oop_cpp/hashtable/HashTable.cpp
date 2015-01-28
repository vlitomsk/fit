#include "HashTable.h"

bool operator==(const Value& v1, const Value& v2) {
  return v1.age == v2.age && v1.weight == v2.weight;
}
bool operator!=(const Value& v1, const Value& v2) {
  return v1.age != v2.age || v1.weight != v2.weight;
}

// Обменивает значения двух хэш-таблиц.
// Подумайте, зачем нужен этот метод, при наличии стандартной функции
// std::swap.
void HashTable::swap(HashTable& b) {
  std::swap(aliveEntries, b.aliveEntries);
  cont.swap(b.cont);
}

HashTable& HashTable::operator=(const HashTable& b) {
  aliveEntries = b.aliveEntries;
  cont = b.cont;
  return *this;
}

bool HashTable::equals(const HashTable& b) const {
  if (aliveEntries != b.aliveEntries) 
    return false;
  for (auto pcell : cont) {
    try {
      if (pcell->st == ST_BUSY && b.at(*(pcell->k)) != *(pcell->v))
        return false;
    } catch (htexception_t e) {
      return false;
    }
  }

  return true;
}

// Очищает контейнер.
void HashTable::clear() {
  aliveEntries = 0;
  cont.clear();
  cont.resize(INITIAL_SIZE);
}
// Удаляет элемент по заданному ключу.
// false -- если такого элемента не существовало
bool HashTable::erase(const Key& k) {
  pcell_t& pcell = getCellRef(k, false);
  if (pcell && pcell->st == ST_BUSY) {
    pcell->st = ST_DELETED;
    --aliveEntries;
    return true;
  }
  return false;
}

// Вставка в контейнер. Возвращаемое значение - успешность вставки.

// false -- если значения повторяются для одного ключа
bool HashTable::insert(const Key& k, const Value& v) {
  pcell_t& pcell = getCellRef(k, true);
  if (!pcell || pcell->st != ST_BUSY) { 
    pcell = std::make_shared<cell_t>(k, v, ST_BUSY);
    if (++aliveEntries >= (cont.size() * CRITICAL_NOM) / CRITICAL_DENOM)
      doubleSize();
    return true;
  }

  *(pcell->v) = v;
  return false;
}

// Возвращает значение по ключу. Небезопасный метод.
Value& HashTable::operator[](const Key& k) {
  return *(getCellRef(k, false)->v); // ->v can be NULL
}

const Value& HashTable::operator[](const Key& k) const {
  return *(getCellConstRef(k, false)->v); // ->v can be NULL
}

// Возвращает значение по ключу. Бросает исключение при неудаче.
Value& HashTable::at(const Key& k) {
  pcell_t& pcell = getCellRef(k, false);
  if (!pcell)
    throw EXCEPTION_NOTFOUND;
  return *(pcell->v);
}

const Value& HashTable::at(const Key& k) const {
  const pcell_t& pcell = getCellConstRef(k, false);
  if (!pcell)
    throw EXCEPTION_NOTFOUND;
  return *(pcell->v);
}

size_t HashTable::size() const {
  return aliveEntries;
}

bool HashTable::empty() const {
  return !aliveEntries;
}

HashTable::pcell_t& HashTable::getCellRef(const Key& k, bool deletedOk) {
  size_t firstHash = keyhash(k),
         secondHash = keyhash2(k);
  size_t i = firstHash;

  do {
    pcell_t& pcell = cont.at(i);
    if (!pcell || (deletedOk && pcell->st == ST_DELETED) ||
        (pcell->st == ST_BUSY && *(pcell->k) == k))
      return pcell;
  } while ((i = (i + secondHash) % cont.size()) != firstHash);

  throw EXCEPTION_UNREACHABLE;
}

const HashTable::pcell_t& HashTable::getCellConstRef(const Key& k, bool deletedOk) const {
  size_t firstHash = keyhash(k),
         secondHash = keyhash2(k);
  size_t i = firstHash;
  do {
    const pcell_t& pcell = cont.at(i);
    if (!pcell || (deletedOk && pcell->st == ST_DELETED) ||
        (pcell->st == ST_BUSY && *(pcell->k) == k))
      return pcell;
  } while ((i = (i + secondHash) % cont.size()) != firstHash);

  throw EXCEPTION_UNREACHABLE;
}

void HashTable::doubleSize(void) {
  std::cout << "Doubling size to " << (cont.size() << 1) << std::endl;
  std::vector< pcell_t > old_cont(cont.size() << 1);
  aliveEntries = 0;
  old_cont.swap(cont); // now it's really old :-)

  for (auto pcell : old_cont) {
    if (pcell && pcell->st == ST_BUSY)
      insert(*(pcell->k), *(pcell->v));
  }
}

/* hashes in 0,...,cont.size()-1 */
size_t HashTable::keyhash(const Key& k) const {
  std::hash<std::string> hashObj;
  return hashObj(k) % cont.size();
}

#include <numeric>
size_t HashTable::keyhash2(const Key& k) const {
  return (std::accumulate(k.begin(), k.end(), 5381, 
           [](size_t acc, size_t x) { return ((acc << 5) + acc) + x; }) % cont.size()) | 1;
}

std::ostream& operator<<(std::ostream& out, const Value& v) {
  return out << "(age = " << v.age << ", weight = " << v.weight << ")";
}

bool operator==(const HashTable & a, const HashTable & b) {
  return a.equals(b);
}

bool operator!=(const HashTable & a, const HashTable & b) {
  return !a.equals(b);
}

