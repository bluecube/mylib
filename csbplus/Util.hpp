#pragma once
#include <stddef.h>
#include <cassert>
#include <type_traits>
#include <iostream>

namespace CSBPlus
{
namespace detail
{

/** Maximum of two unsigned. Needed because std::max is not constexpr in C++11 */
constexpr unsigned max(unsigned a, unsigned b) { return a > b ? a : b; }

constexpr unsigned fieldCount(size_t fieldSize, size_t totalSpace, size_t usedSpace)
{
    return (totalSpace - usedSpace) / fieldSize;
}

constexpr size_t alignedStorageSize(size_t size, size_t align)
{
    return align * ((size + align - 1) / align);
}

template <typename T>
constexpr size_t alignedStorageSize()
{
    return alignedStorageSize(sizeof(T), alignof(T));
}

/** Holds either a pointer to an object or the object by itself if it is small and simple.
 *
 * This general version holds a pointer. */
template <typename T_, typename Simple = void>
class Holder
{
public:
    typedef T_ T;

private:
    T* value;
    static_assert(alignof(T*) <= sizeof(T*),
                  "Pointer needs too much alignment, I didn't expect that.");
public:
    static constexpr bool direct = false;
    static constexpr size_t paddingSize = 0;

    Holder() { this->value = new T(); }
    ~Holder() { this->clear(); }
    T& ref() { return *this->value; }
    void clear() { delete this->value; this->value = NULL; }
};

/** Holds either a pointer to an object or the object by itself if it is small and simple.
 *
 * This specialised version holds an object directly.
 *
 * TODO: relax the requirement for trivial type, use move semantics. */
template <typename T_>
class Holder<T_, typename std::enable_if<alignedStorageSize<T_>() <= sizeof(void*) &&
                                        std::is_trivial<T_>::value>::type>
{
public:
    typedef T_ T;

private:
    T value;

public:
    static constexpr bool direct = true;

    Holder() {
        assert(&this->value % alignof(T) == 0);
    }
    T& ref() { return this->value; }
    void clear() {}
};

template <typename H>
void printHolderStats()
{
    typedef typename H::T T;
    std::cout << "    size: " << sizeof(T) << " B; alignment: " << alignof(T) << " B" << std::endl;
    std::cout << "    allocation: " << (H::direct ? "direct" : "pointer") << std::endl;
    std::cout << "    holder size: " << sizeof(H) << " B; holder alignment: " << alignof(H) << " B" << std::endl;
}

template <typename KH, typename VH, unsigned keyCount, typename Dummy = void>
struct LeafHelper
{
    KH keys[keyCount];
    VH values[keyCount];
};

template <typename KH, typename VH, unsigned keyCount>
struct LeafHelper<KH, VH, keyCount,
                  typename std::enable_if<alignof(KH) < alignof(VH)>::type>
{
    VH values[keyCount];
    KH keys[keyCount];
};

}
}
