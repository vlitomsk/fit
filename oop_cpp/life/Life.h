#ifndef _LIFE_H_
#define _LIFE_H_

#include "Bitmap.h"
#include <memory>
#include <cstdint>
#include <string>
#include <set>

struct LifeCoord {
public:
    LifeCoord(int64_t xval, int64_t yval) : x(xval), y(yval) {}
    bool operator<(const LifeCoord& lc) const {
        //return x < lc.x && y < lc.y;
        if (x < lc.x)
            return true;
        return y < lc.y;
    }
    int64_t x, y;
};

class Life {
public:
    static const uint8_t
        ZERO = 1, ONE = 2, TWO = 4, THREE = 8, FOUR = 16, FIVE = 32, SIX = 64, SEVEN = 128;
    static const uint8_t 
        CONWAY_BIRTH = TWO,
        CONWAY_SURVIVE = TWO | THREE;
    Life(const std::string& name, size_t width, size_t height, 
         uint8_t birthFactor = CONWAY_BIRTH, uint8_t surviveFactor = CONWAY_SURVIVE);
    Life(const std::string& filename);
    ~Life();

    void set(int64_t x, int64_t y, bool value = true);
    void clear(int64_t x, int64_t y);
    bool get(int64_t x, int64_t y);
    void step(int stCount = 1);
    void dump(const std::string& filename);
    size_t getAliveCount(void);
    size_t getWidth(void);
    size_t getHeight(void);
    const std::set<LifeCoord>& getDifference(void);
private:
    size_t aliveCount;
    size_t w, h;
    std::string univName;
    std::set<LifeCoord> *difference, *auxDifference;
    uint8_t bFactor, sFactor; // birth, survive factors
    Bitmap *field, *auxField;
    
    size_t flatIdx(int64_t x, int64_t y);
    void parseFactors(const std::string& factorString);
    std::string strFactors(void);
    void print(void);
    void addDiffBlock(int64_t i, int64_t j);

    int64_t normalizeX(int64_t x);
    int64_t normalizeY(int64_t y);
};

#endif