#ifndef FIBONACCI_INLINE_HEAP_H
#define FIBONACCI_INLINE_HEAP_H

#include <functional>
#include <vector>
#include <iostream>
#include <cassert>
#include <cstdint>
#include "binary_heap.h"

template<typename T, typename Lesser = std::less<T>, typename Track = null_tracker<T>>
class fibonacci_inline_heap
{
private:
    struct node
    {
        T key;
        size_t rank;
        size_t parent;
        size_t left;
        size_t right;
        size_t child;
        bool mark;

        node(const T& key)
            : key(key)
            , rank(0)
            , parent(SIZE_MAX)
            , left(SIZE_MAX)
            , right(SIZE_MAX)
            , child(SIZE_MAX)
            , mark(false)
        {
        }
    };

    std::vector<node> m_nodes;
    size_t m_min;
    size_t m_free;
    size_t m_size;

    void track(size_t index)
    {
        heap_locator loc;
        loc.index = index;

        Track track;
        track(m_nodes[index].key, loc);
    }

    // Add x to the specified linked list.
    // If 'list' is null, x becomes the head (i.e. list == x after).
    // Otherwise x is added before the head.
    void add_to_list(size_t& list, size_t x)
    {
        node& xn = m_nodes[x];

        if (list == SIZE_MAX)
        {
            list = x;
            xn.left = x;
            xn.right = x;
        }
        else
        {
            node& listNode = m_nodes[list];
            size_t tail = listNode.left;

            xn.left = tail;
            xn.right = list;

            m_nodes[tail].right = x;
            listNode.left = x;
        }
    }

    // Remove x from the specified linked list.
    // If 'list' consisted only of x, it is set to null.
    void remove_from_list(size_t& list, size_t x)
    {
        node& node = m_nodes[x];
        // Head and only element
        if (list == x && node.right == x)
        {
            assert(node.left == x);
            list = SIZE_MAX;
            return;
        }

        assert(node.right != x && node.left != x);

        // Head but not only element
        if (list == x)
            list = node.right;

        // Unlink
        m_nodes[node.left].right = node.right;
        m_nodes[node.right].left = node.left;
        node.left = x;
        node.right = x;
    }

    void add_child(size_t parent, size_t child)
    {
        m_nodes[child].parent = parent;
        add_to_list(m_nodes[parent].child, child);
        m_nodes[parent].rank++;
    }

    void cut(size_t x)
    {
        node& xn = m_nodes[x];

        size_t parent = xn.parent;
        node& parentNode = m_nodes[parent];

        remove_from_list(parentNode.child, x);
        parentNode.rank--;
        xn.parent = SIZE_MAX;

        add_to_list(m_min, x);

        xn.mark = false;
        if (parentNode.parent == SIZE_MAX)
            return;
        
        if (parentNode.mark)
            cut(parent);
        else
            parentNode.mark = true;
    }

    void consolidate()
    {
        size_t byDegree[128];
        size_t maxDegree = 0;
        std::fill(std::begin(byDegree), std::end(byDegree), SIZE_MAX);
        
        Lesser lesser;
        while (m_min != SIZE_MAX)
        {
            size_t adding = m_min;
            remove_from_list(m_min, m_min);

            while (true)
            {
                node& addingNode = m_nodes[adding];
                assert(addingNode.rank < 128);
                size_t& spot = byDegree[addingNode.rank];

                if (spot == SIZE_MAX)
                {
                    spot = adding;
                    maxDegree = std::max(maxDegree, addingNode.rank + 1);
                    break;
                }

                if (lesser(m_nodes[spot].key, addingNode.key))
                {
                    add_child(spot, adding);
                    adding = spot;
                }
                else
                {
                    add_child(adding, spot);
                }

                spot = SIZE_MAX;
            }
        }
        
        for (size_t i = 0; i < maxDegree; i++)
        {
            size_t root = byDegree[i];
            if (root == SIZE_MAX)
                continue;

            add_to_list(m_min, root);
            if (lesser(m_nodes[root].key, m_nodes[m_min].key))
                m_min = root;
        }
    }

public:
    fibonacci_inline_heap()
        : m_min(SIZE_MAX)
        , m_free(SIZE_MAX)
        , m_size(0)
    {
    }

    void insert(const T& value)
    {
        size_t x;
        if (m_free == SIZE_MAX)
        {
            m_nodes.emplace_back(value);
            x = m_nodes.size() - 1;
        }
        else
        {
            x = m_free;
            m_free = m_nodes[x].right;

            m_nodes[x].key = value;
            m_nodes[x].rank = 0;
            m_nodes[x].parent = SIZE_MAX;
            m_nodes[x].child = SIZE_MAX;
        }

        track(x);
        add_to_list(m_min, x);

        Lesser less;
        if (less(value, m_nodes[m_min].key))
            m_min = x;

        m_size++;
    }

    void print_list()
    {
        if (m_min != SIZE_MAX)
        {
            size_t cur = m_min;
            do
            {
                std::cout
                    << "key: " << m_nodes[cur].key
                    << ", left: " << m_nodes[m_nodes[cur].left].key
                    << ", right: " << m_nodes[m_nodes[cur].right].key << "\n";

                cur = m_nodes[cur].right;
            } while(cur != m_min);
        }

        std::cout << "\n";
    }

    const size_t size() const { return m_size; }
    const T& find_min() const { return m_nodes[m_min].key; }

    void decrease_key(heap_locator loc, const T& key)
    {
        size_t x = loc.index;
        node& node = m_nodes[x];

        node.key = key;

        Lesser less;
        if (node.parent != SIZE_MAX &&
            less(node.key, m_nodes[node.parent].key))
        {
            cut(x);
        }

        if (less(node.key, m_nodes[m_min].key))
            m_min = x;
    }

    void delete_min()
    {
        assert(m_min != SIZE_MAX);

        node& minNode = m_nodes[m_min];
        while (minNode.child != SIZE_MAX)
        {
            size_t node = minNode.child;
            remove_from_list(minNode.child, node);
            add_to_list(m_min, node);
            m_nodes[node].parent = SIZE_MAX;
        }      

        size_t removed = m_min;
        remove_from_list(m_min, m_min);
        m_size--;

        m_nodes[removed].right = m_free;
        m_free = removed;

        consolidate();
    }

    static fibonacci_inline_heap<T, Lesser, Track> make_heap(const std::vector<T>& elems)
    {
        fibonacci_inline_heap<T, Lesser, Track> heap;
        
        for (const T& t: elems)
        {
            heap.insert(t);
        }

        return heap;
    }
};

#endif
