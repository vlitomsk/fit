#ifndef PIECE_H
#define PIECE_H

#include <cstddef>
#include <stdexcept>

/* Piece [from; to) */
struct Piece {
    Piece() : from(0), to(0) {}
    Piece(std::size_t from, std::size_t to)
        : from(from), to(to)
    {
        if (from > to)
            throw std::invalid_argument("Piece ctor: from > to!");
    }

    Piece(const Piece &p)
        : from(p.from), to(p.to) {}

    bool operator==(const Piece &p) const {
        return p.from == from && p.to == to;
    }

    bool operator!=(const Piece &p) const {
        return p.from != from || p.to != to;
    }

    std::size_t len() const {
        return to - from;
    }

    Piece intersect(const Piece &p) const {
        if (to <= p.from || p.to <= from) // no intersection
            return Piece(0, 0);
        if (from <= p.from && p.to <= to) // [this [p p) this)
            return Piece(p);
        if (p.from <= from && to <= p.to) // [p [this this) p)
            return Piece(*this);
        if (from >= p.from) // [p [this p) this)
            return Piece(from, p.to);
        if (to <= p.to) // [this [p this) p)
            return Piece(p.from, to);
        throw std::runtime_error("Unhandled intersection");
    }

    std::size_t from;
    std::size_t to;
};

#endif // PIECE_H
