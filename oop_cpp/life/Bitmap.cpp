#include "Bitmap.h"
#include <exception>
#include <algorithm>

void Bitmap::resize(size_t sizeInBits) {
    sizeBits = sizeInBits;
    size_t sizeBytes = (sizeInBits >> 3) + (sizeInBits & 7 ? 1 : 0);
    container.resize(sizeBytes);
}

// size in bits
Bitmap::Bitmap(size_t sizeInBits, bool isCyclic) 
    : sizeBits(sizeInBits), cyclic(isCyclic)
{
    resize(sizeInBits);
}

Bitmap& Bitmap::operator=(const Bitmap& b) {
    cyclic = b.cyclic;
    sizeBits = b.sizeBits;
    container = b.container;
    return *this;
}

// throws exception
bool Bitmap::at(int64_t bitIdx) {
    return (indexReference(bitIdx) >> (bitIdx & 7)) & 1;
}

// unsafe method
bool Bitmap::operator[](int64_t bitIdx) {
    try {
        return at(bitIdx);
    } catch (std::exception& e) {
        return stub;
    }
}

void Bitmap::set(int64_t bitIdx, bool value) {
    uint8_t& ref = indexReference(bitIdx);
    if (value)
        ref |= 1 << (bitIdx & 7);
    else
        ref &= ~(1 << (bitIdx & 7));
}

void Bitmap::clear(int64_t bitIdx) {
    set(bitIdx, false);
}

void Bitmap::swap(Bitmap& b) {
    std::swap(this->cyclic, b.cyclic);
    std::swap(this->sizeBits, b.sizeBits);
    container.swap(b.container);
}

void Bitmap::zeroAll(void) {
    for (size_t i = 0; i < container.size(); ++i) {
        container[i] = 0;
    }
}

uint8_t& Bitmap::indexReference(int64_t bitIdx) {
    if (cyclic) {
        if (bitIdx < 0)
            bitIdx = -(-(bitIdx + 1) % sizeBits) - 1;
        else
            bitIdx %= sizeBits;
    }
    return container.at(static_cast<size_t>(bitIdx >> 3));
}
