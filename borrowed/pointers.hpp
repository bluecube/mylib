#pragma once

#include <cassert>
#include <cstddef>

template <class T>
class BorrowedPtr
{
public:
    using Type = T;

    BorrowedPtr() : detail::PtrBase<T>(nullptr) {}
    BorrowedPtr(std::nullptr_t) : detail::PtrBase<T>(nullptr) {}
    explicit BorrowedPtr(Type* ptr) : detail::PtrBase<T>(ptr) {}
    BorrowedPtr(BorrowedPtr&) = delete;

    ~BorrowedPtr() { this->clear(); }

    void operator=(BorrowedPtr<T>&) = delete;
    BorrowedPtr<T> operator=(BorrowedPtr<T>&& other) { this->ptr = other.ptr; other.ptr = nullptr; }

private:
    Type* ptr;
};

template <class T>
class OwnedPtr
{
public:
    using Type = T;

    OwnedPtr() : ptr(nullptr) {}
    OwnedPtr(std::nullptr_t) : OwnedPtr() {}
    OwnedPtr(OwnedPtr&) = delete;
    OwnedPtr(OwnedPtr&& other) : OwnedPtr(other.ptr) { other.ptr = nullptr; }
    explicit OwnedPtr(Type* ptr) : ptr(ptr) {}

    ~OwnedPtr() { this->clear(); }

    void operator=(OwnedPtr<T>&) = delete;
    OwnedPtr<T> operator=(OwnedPtr<T>&& other) { this->ptr = other.ptr; other.ptr = nullptr; }

    Type* operator->() { assert(this->ptr); return this->ptr; }
    Type* operator*() { assert(this->ptr); return this->ptr; }

    bool operator==(void* other) const { return this->ptr == other; }
    bool operator bool() const { return this->ptr != nullptr; }

    Type*& raw() { return this->ptr; }
    void clear() { delete this->ptr; this->ptr = nullptr; }
    BorrowedPtr<Type> borrow() { return BorrowedPtr(this->ptr); }

private:
    Type* ptr;
};
