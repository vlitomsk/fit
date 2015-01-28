#include <iostream>
#include <fstream>
#include <array>
#include <string>
#include <unordered_map>
#include "HashTable.h"

using namespace std;

void putnlines(HashTable& ht, size_t N) {
    ifstream f("names");
    Key name;
    for (size_t i = 0; i < N && getline(f, name); ++i) {
        Value v = {static_cast<unsigned>(i), static_cast<unsigned>(i)};
        ht.insert(name, v);
    }
    f.close();
}
    
const string path("names");
const size_t nameCount = 10000;

int main(int argc, const char *argv[]) {
    string name;
    unordered_map<Key, Value> um;
    HashTable ht;

    ifstream f(path);
    for (size_t i = 0 ; i < nameCount && getline(f, name); ++i) {
      Value v = {static_cast<unsigned>(i), static_cast<unsigned>(i)};
      um.insert(std::pair<Key, Value>(name, v));
      ht.insert(name, v);
    }
  
    cout << "insert done\n";
    f.clear();
    f.seekg(0);
    for (size_t i = 0; i < nameCount && getline(f, name); ++i) {
      ht.erase(name);
      um.erase(name);
    }

    cout << "erase done\n";
    f.clear();
    f.seekg(0);
    for (size_t i = 0 ; i < nameCount && getline(f, name); ++i) {
      Value v = {static_cast<unsigned>(i), static_cast<unsigned>(i)};
      um.insert(std::pair<Key, Value>(name, v));
      ht.insert(name, v);
    }
    cout << "reinsert done\n";
   
    f.clear();
    f.seekg(0);
    for (size_t i = 0; i < nameCount && getline(f, name); ++i) {
        Value v = um.at(name);
        (i, v.age);
        (i, v.weight);
        v = ht.at(name);
        (i, v.age);
        (i, v.weight);
    }
  return 0;
}

