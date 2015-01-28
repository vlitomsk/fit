#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <cstdint>
#include <vector>
#include <cstdlib>

// it's impossible to define type like signed size_t

class Bitmap {
public:
    // size in bits
    Bitmap(size_t sizeInBits, bool isCyclic = false );

    // throws exception
    bool at(int64_t bitIdx);

    // unsafe method
    bool operator[](int64_t bitIdx);

    void set(int64_t bitIdx, bool value = true);

    void clear(int64_t bitIdx);

    void swap(Bitmap& b);

    void zeroAll(void);

    void resize(size_t sizeInBits);

    Bitmap& operator=(const Bitmap& b);
private:
    bool stub; // for operator[]
    bool cyclic;
    size_t sizeBits;
    std::vector<uint8_t> container;

    uint8_t& indexReference(int64_t bitIdx);
};

#endif