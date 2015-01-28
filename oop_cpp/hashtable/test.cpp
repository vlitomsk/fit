#include <gtest/gtest.h>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include "HashTable.h"
#include <functional>

using namespace std;

const std::string path("numbers");
const size_t nameCount = 10000;

void processTable(HashTable& ht, ifstream& f, size_t count, std::function<> func) {
  f.clear();
  f.seekg(0);
  for (size_t i = 0; i < count && getline(f, name); ++i) {
    func();
  }
}

TEST(pack0, insert) {
    string name;
    unordered_map<Key, Value> um;
    HashTable ht;

    ifstream f(path);
    for (size_t i = 0 ; i < nameCount && getline(f, name); ++i) {
      Value v = {static_cast<unsigned>(i), static_cast<unsigned>(i)};
      um.insert(std::pair<Key, Value>(name, v));
      ht.insert(name, v);
    }
  
    f.clear();
    f.seekg(0);
    for (size_t i = 0; i < nameCount && getline(f, name); ++i) {
        Value v = um.at(name);
        EXPECT_EQ(i, v.age);
        EXPECT_EQ(i, v.weight);
        if (i != v.age) {
          cout << "Bad key: " << name << endl;
        }
        v = ht.at(name);
        EXPECT_EQ(i, v.age);
        EXPECT_EQ(i, v.weight);
    }
}

TEST(pack0, erase) {
    string name;
    unordered_map<Key, Value> um;
    HashTable ht;

    ifstream f(path);
    for (size_t i = 0 ; i < nameCount && getline(f, name); ++i) {
      Value v = {static_cast<unsigned>(i), static_cast<unsigned>(i)};
      um.insert(std::pair<Key, Value>(name, v));
      ht.insert(name, v);
    }
  
    f.clear();
    f.seekg(0);
    for (size_t i = 0; i < nameCount && getline(f, name); ++i) {
      ht.erase(name);
      um.erase(name);
    }

    f.clear();
    f.seekg(0);
    for (size_t i = 0 ; i < nameCount && getline(f, name); ++i) {
      Value v = {static_cast<unsigned>(i), static_cast<unsigned>(i)};
      um.insert(std::pair<Key, Value>(name, v));
      ht.insert(name, v);
    }
    f.clear();
    f.seekg(0);

    for (size_t i = 0; i < nameCount && getline(f, name); ++i) {
        Value v = um.at(name);
        EXPECT_EQ(i, v.age);
        EXPECT_EQ(i, v.weight);
        v = ht.at(name);
        EXPECT_EQ(i, v.age);
        EXPECT_EQ(i, v.weight);
    }
}

TEST(pack0, clear) {
    string name;
    unordered_map<Key, Value> um;
    HashTable ht;

    ifstream f(path);
    for (size_t i = 0 ; i < nameCount && getline(f, name); ++i) {
      Value v = {static_cast<unsigned>(i), static_cast<unsigned>(i)};
      um.insert(std::pair<Key, Value>(name, v));
      ht.insert(name, v);
    }
  
    ht.clear();

    f.clear();
    f.seekg(0);
    for (size_t i = 0 ; i < nameCount && getline(f, name); ++i) {
      Value v = {static_cast<unsigned>(i), static_cast<unsigned>(i)};
      um.insert(std::pair<Key, Value>(name, v));
      ht.insert(name, v);
    }
    f.clear();
    f.seekg(0);

    for (size_t i = 0; i < nameCount && getline(f, name); ++i) {
        Value v = um.at(name);
        EXPECT_EQ(i, v.age);
        EXPECT_EQ(i, v.weight);
        v = ht.at(name);
        EXPECT_EQ(i, v.age);
        EXPECT_EQ(i, v.weight);
    }
}

