#include "Tree.hpp"

int main()
{
    struct Weirdo { char a, b, c; };
    typedef CSBPlus::Tree<char, uint64_t> X;
    X::printLayoutStats();

    /*std::cout << std::endl;
    typedef CSBPlus::map<uint32_t, uint64_t> Y;
    Y::printLayoutStats();

    std::cout << std::endl;
    typedef CSBPlus::map<uint8_t, uint8_t> Z;
    Z::printLayoutStats();*/

    return 0;
}
