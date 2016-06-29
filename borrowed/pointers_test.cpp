#include "pointers.hpp"
#include <string>
#include <cassert>

std::string events;

class C
{
    public:
    C(int value) : value(value)
    {
        events += "(";
    }
    ~C()
    {
        events += ")";
    }
    int value;
};

int main()
{
    OwnedPtr<C> a(new C(1));
    assert(events == "(");
    OwnedPtr<C> b(new C(2))
    assert(events == "((");
    {
        //OwnedPtr<C> c(b); // Owned pointers cannot be copied
        OwnedPtr<C> d(std::move(b));
        assert(events == "((");
    }
}
