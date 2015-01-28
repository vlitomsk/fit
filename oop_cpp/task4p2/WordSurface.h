#ifndef _WORD_SURFACE_H_
#define _WORD_SURFACE_H_

#include "Surface.h"
#include <string>
#include <iostream>

/***********************************/
/* Word surface specialization   */
/***********************************/

typedef Surface<std::string, uint> WordSurface;

template<>
class Surface<std::string, uint> {
public:
  Surface(const std::string& startWord, const std::string& endWord) 
    : dirty(false), startPos(startWord), endPos(endWord) {}
  
  std::string move(const std::string& dst) throw (BadMove) {
    if (dirty)
      refreshLookup();

    for (auto ent : lookupResult) 
      if (dst == std::get<0>(ent)) {
        dirty = true;
        return (pos = dst);
      }
    
    throw BadMove();
  }

  std::vector<std::tuple<std::string, uint> > lookup(void) {
    refreshLookup();
    return std::vector<std::tuple<std::string, uint> >(lookupResult);
  }

  friend std::istream& operator>>(std::istream&, WordSurface&);
private:
  bool dirty;
  std::vector<std::string> wordList;
  std::vector<std::tuple<std::string, uint> > lookupResult;
  std::string pos, startPos, endPos;
  
  void refreshLookup(void) {
    lookupResult.clear();
    for (auto word : wordList) {
      uint dist = levenshteinDistance(word, pos);
      if (dist <= 1)
        lookupResult.push_back(std::tuple<std::string, uint>(word, dist));
    }
    dirty = false;
  }

  uint levenshteinDistance(const std::string &s1, const std::string& s2) const {
    const size_t len1 = s1.size(), len2 = s2.size();
    std::vector<uint> col(len2+1), prevCol(len2+1);
   
    for (uint i = 0; i < prevCol.size(); i++)
      prevCol[i] = i;
    for (uint i = 0; i < len1; i++) {
      col[0] = i + 1;
      for (uint j = 0; j < len2; j++)
        col[j + 1] = std::min( std::min(prevCol[1 + j] + 1, col[j] + 1),
                  prevCol[j] + (s1[i] == s2[j] ? 0 : 1) );
      col.swap(prevCol);
    }
    return prevCol[len2];
  }
};

std::istream& operator>>(std::istream& is, WordSurface& s) {
  std::string line;
  while (std::getline(is, line)) 
    s.wordList.push_back(line);
  return is; 
}


#endif