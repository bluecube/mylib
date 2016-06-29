#include <Node.hpp>

namespace CSBPlus
{

namespace detail
{

/** Iterator type for Tree<K, V> */
template <typename K, typename V>
class Iterator
{
    using T = Tree<K, V>;
public:
    bool operator==(const Iterator<K, V> other)
    {
        return this->node == other.node && this->keyIndex == other.keyIndex;
    }

    bool operator!=(const Iterator<K, V> other)
    {
        return this->node != other.node || this->keyIndex != other.keyIndex;
    }

    PairProxy<K, V> operator*()
    {
        assert(this->node != T::invalidNeighbor);
        return PairProxy<K, V>(this->node->keys[this->keyIndex],
                               this->node->values[this->keyIndex]);
    }

    PairProxy<K, V> operator->()
    {
        return *this;
    }

    Iterator<K, V> operator++()
    {
        assert(this->node != T::invalidNeighbor);
        if (this->keyIndex < this->node->getKeyCount())
            ++this->keyIndex;
        else
        {
            this->node = this->node->getNextLeaf();
            this->keyIndex = 0;
        }
    }

    Iterator<K, V> operator++(int)
    {
        Iterator<K, V> ret = *this;
        ++(*this);
        return ret;
    }

    Iterator<K, V> operator--()
    {
        assert(this->node != T::invalidNeighbor);
        if (this->keyIndex > 0)
            --this->keyIndex;
        else
        {
            this->node = this->node->getPrevLeaf();
            if (this->node == Tree<K, V>::invalidNeighbor)
                this->keyIndex = 0;
            this->keyIndex = this->node->getKeyCount() - 1;
        }
    }

    Iterator<K, V> operator--(int)
    {
        Iterator<K, V> ret = *this;
        --(*this);
        return ret;
    }

private:
    Iterator(typename Tree<K, V>::Node* node, unsigned keyIndex = 0)
        : node(node)
        , keyIndex(keyIndex) {}

    typename Tree<K, V>::Node* node = nullptr;
    unsigned keyIndex = 0;
};

/** Reverse iterator type for Tree<K, V> */
template <typename K, typename V>
class ReverseIterator
{
public:
    bool operator==(const Iterator<K, V> other)
    {
        return this->it == other.it;
    }

    bool operator!=(const Iterator<K, V> other)
    {
        return this->it != other.it;
    }

    PairProxy<K, V> operator*()
    {
        return *(this->it);
    }

    PairProxy<K, V> operator->()
    {
        return *this;
    }

    ReverseIterator<K, V> operator++()
    {
        --(this->it);
        return *this;
    }

    ReverseIterator<K, V> operator++(int)
    {
        ReverseIterator<K, V> ret = *this;
        --(this->it);
        return ret;
    }

    ReverseIterator<K, V> operator--()
    {
        ++(this->it);
        return *this;
    }

    ReverseIterator<K, V> operator--(int)
    {
        ReverseIterator<K, V> ret = *this;
        ++(this->it);
        return ret;
    }
private:
    ReverseIterator(typename Tree<K, V>::Node* n)
      : it(n, n->getKeyCount() - 1) {}

    ReverseIterator(typename Tree<K, V>::Node* n, unsigned keyIndex)
      : it(n, keyIndex) {}

    Iterator<K, V> it;
};

}
}
