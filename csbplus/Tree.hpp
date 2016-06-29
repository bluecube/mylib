#pragma once
#include <stddef.h>
#include <iostream>

namespace CSBPlus
{

namespace detail
{

/** Cache Sensitive B+ tree.
 * Should be mostly interface compatible with std::map. */
template <typename K, typename V>
class Tree
{
public:
    typedef K KeyType;
    typedef V ValueType;

    Iterator<K, V> begin() noexcept
    {
        return Iterator<K, V>(this->firstLeaf);
    }

    Iterator<K, V> end() noexcept
    {
        return Iterator<K, V>(invalidNeighbor);
    }

    ReverseIterator<K, V> rbegin() noexcept
    {
        return ReverseIterator<K, V>(this->lastLeaf);
    }

    ReverseIterator<K, V> rend() noexcept
    {
        return ReverseIterator<K, V>(invalidNeighbor, 0);
    }

private:
    Node<K, V> *root;
    Node<K, V> *firstLeaf;
    Node<K, V> *lastLeaf;

    friend class Iterator<K, V>;
};

template <typename K, typename V>
void Tree<K, V>::printLayoutStats()
{
#define X(name) std::cout << "        " #name ": " << offsetof(Node, name) << " .. " << \
    (offsetof(Node, name) + sizeof(Node::name) - 1) << std::endl;

    std::cout << "Keys:" << std::endl;
    detail::printHolderStats<KH>();

    std::cout << "Values:" << std::endl;
    detail::printHolderStats<VH>();

    std::cout << "Nodes:" << std::endl;
    std::cout << "    Size: " << sizeof(Node) << " B" << std::endl;

    std::cout << "Inner nodes:" << std::endl;
    std::cout << "    Max key count: " << innerNodeKeyCount << std::endl;
    std::cout << "    Space used: " <<
        sizeof(Node::pointer) + sizeof(Node::inner.keys) <<
        " B / " << cachelineSize << " B" << std::endl;
    std::cout << "    Layout:" << std::endl;
    X(pointer);
    X(inner.keys);

    std::cout << "Leaf nodes:" << std::endl;
    std::cout << "    Max key count: " << leafNodeKeyCount << std::endl;
    std::cout << "    Space used: " <<
        sizeof(Node::pointer) + sizeof(Node::leaf.keys) + sizeof(Node::leaf.values) <<
        " B / " << cachelineSize << " B" << std::endl;
    std::cout << "    Layout:" << std::endl;
    X(pointer);
    X(leaf.keys);
    X(leaf.values);

    std::cout << "Node groups:" << std::endl;
    std::cout << "    Node count: " << nodeGroupNodeCount << std::endl;
    std::cout << "    Size: " << sizeof(NodeGroup) << " B" << std::endl;

#undef X
}

}
