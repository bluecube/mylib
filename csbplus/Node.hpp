#include "Util.hpp"

namespace CSBPlus
{

namespace detail
{

template <typename K, typename V>
struct NodeGroup;

template <typename K, typename V>
struct alignas(cachelineSize) Node
{
    typedef detail::Holder<K> KH;
    typedef detail::Holder<V> VH;

    static constexpr unsigned cachelineSizeShift = 6;
    static constexpr size_t cachelineSize = 1 << cachelineSizeShift;
    static constexpr Node* invalidNeighbor = 1 << cachelineSizeShift;

    static constexpr uintptr_t nodePtrMask = ~(cachelineSize - 1);
    static constexpr uintptr_t leafFlag = 1 << (cachelineSizeShift - 1);
    static constexpr uintptr_t innerKeyCountMask = leafFlag - 1;
    static constexpr uintptr_t firstInBlockFlag = 1 << (cachelineSizeShift - 2);
    static constexpr uintptr_t leafKeyCountMask = firstInBlockFlag - 1;

    static constexpr unsigned innerNodeKeyCount = detail::fieldCount(sizeof(KH),
                                                                     cachelineSize,
                                                                     sizeof(uintptr_t));

    static constexpr unsigned leafNodeKeyCount = detail::fieldCount(sizeof(KH) + sizeof(VH),
                                                                    cachelineSize,
                                                                    sizeof(uintptr_t));
    static constexpr unsigned nodeGroupNodeCount = innerNodeKeyCount + 1;

    static_assert(innerNodeKeyCount / 2 + 1 < innerKeyCountMask - 1,
                  "There can be more inner node keys than we can fit into its info field");
    static_assert(leafNodeKeyCount / 2 + 1 < leafKeyCountMask - 1,
                  "There can be more inner node keys than we can fit into its info field"
                  " (set of uint8_t is not supported, use a bit mask");
    //static_assert(sizeof(Node) == cachelineSize,
    //              "Nodes must be exactly one cachelineSize large");

    Node()
    {
        assert(uintptr_t(this) & ~nodePtrMask == 0);
    }

    bool isLeaf()
    {
        return bool(this->pointer & leafFlag);
    }

    Node* getNextLeaf()
    {
        assert(this->isLeaf());
        // This function only works correctly if the node isn't the last in the tree

        Node* neighbor = this->pointer & nodePtrMask;
        bool firstInBlock = this->pointer & firstInBlock;

        if (!neighbor || firstInBlock)
            return this + 1;
        else
            return neighbor;
    }

    Node* getPrevLeaf()
    {
        assert(this->isLeaf());

        Node* neighbor = this->pointer & nodePtrMask;
        bool firstInBlock = this->pointer & firstInBlock;

        if (!neighbor || !firstInBlock)
            return this - 1;
        else
            return neighbor;
    }

    unsigned getKeyCount()
    {
        if (this->isLeaf())
            return this->pointer & leafKeyCountMask + leafNodeKeyCount / 2;
        else
            return this->pointer & innerKeyCountMask + innerNodeKeyCount / 2;
    }

    Node* getChild(unsigned n)
    {
        assert(!this->isLeaf());
        assert(n <= this->getKeyCount());

        NodeGroup* children = this->pointer & ~nodePtrMask;
        return children.nodes[n];
    }

    uintptr_t pointer;

    union
    {
        struct // inner node
        {
            KH keys[innerNodeKeyCount];
        } inner;
        detail::LeafHelper<KH, VH, leafNodeKeyCount> leaf;
    };
};

template <typename K, typename V>
struct NodeGroup
{
    Node nodes[nodeGroupNodeCount];
};

}
}
