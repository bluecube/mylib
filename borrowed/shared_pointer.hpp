#pragma once

#include "pointers.hpp"
#include <cassert>
#include <cstddef>

template <class T>
class SharedPtr
{
public:
    using Type = T;

    SharedPtr() : ptr(nullptr), refCountPtr(nullptr) {}
    SharedPtr(std::nullptr_t) : SharedPtr() {}
    SharedPtr(SharedPtr& other)
        : PtrBase<T>(other.ptr), refCountPtr(other.refCountPtr)
    {
        if (this->refCountPtr)
        {
            assert(this->ptr);
            (*this->refCountPtr)++;
        }
    }
    SharedPtr(SharedPtr&& other)
        : detail::PtrBase<T>(nullptr), refCountPtr(other.refCountPtr)
    {
        this->refCountPtr = other.refCountPtr;
        other.refCountPtr = nullptr;
    }
    explicit SharedPtr(Type* ptr)
        : ptr(ptr)
    {
        if (ptr)
            this->refCountPtr = new size_t(1);
        else
            this->refCountPtr = nullptr;
    }
    explicit SharedPtr(OwnedPtr<T>&& other): SharedPtr(other.ptr) { other.ptr = nullptr; }

    ~SharedPtr() { this->clear(); }

    void operator=(SharedPtr<T>& other) { SharedPtr<T> tmp(other); std::swap(tmp, other); }
    SharedPtr<T> operator=(SharedPtr<T>&& other)
    {
        this->ptr = other.ptr;
        this->refCountPtr = other.refCountPtr;
        other.ptr = nullptr;
        other.refCountPtr = nullptr;
    }

    Type* operator->() { assert(this->ptr); return this->ptr; }
    Type* operator*() { assert(this->ptr); return this->ptr; }

    bool operator==(void* other) const { return this->ptr == other; }
    bool operator bool() const { return this->ptr != nullptr; }

    Type*& raw() { return this->ptr; }
    void clear()
    {
        assert(this->ptr);
        assert(this->refCountPtr);
        assert(this->refCountPtr > 0);

        if (this->refcount == 1)
        {
            delete this->ptr;
            this->ptr = nullptr;
            delete this->refCountPtr;
            this->refCountPtr = nullptr;
        }
        else
            (*this->refcount)--;
    }
    size_t refCount()
    {
        if (this->refCountPtr)
            return *this->refCountPtr;
        else
            return 0;
    }

    BorrowedPtr<Type> borrow() { return BorrowedPtr(this->ptr); }
    OwnedPtr<Type> claim()
    {
        assert(this->refCount() <= 1);

        OwnedPtr ret(this->ptr);
        this->ptr = nullptr;

        delete this->refCountPtr;
        this->refCountPtr = nullptr;

        return ret;
    }

private:
    Type* ptr;
    size_t *refCountPtr;
};

