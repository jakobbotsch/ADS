#ifndef FIBONACCI_HEAP_H
#define FIBONACCI_HEAP_H

#include <functional>
#include <vector>
#include <iostream>
#include <cassert>
#include "binary_heap.h"

template<typename T, typename Lesser = std::less<T>, typename Track = null_tracker<T>>
class fibonacci_heap
{
private:
    struct heap_node
    {
        T key;
        size_t rank;
        heap_node *p;
        heap_node *left;
        heap_node *right;
        heap_node *child;
        bool mark;

        heap_node(const T& key)
            : key(key)
            , rank(0)
            , p(nullptr)
            , left(this)
            , right(this)
            , child(nullptr)
            , mark(false)
        {
        }

    };

    heap_node *m_min;
    size_t m_size;
    Lesser m_less;
    Track m_track;
    int m_numDKs;

    void track(heap_node *node)
    {
        heap_locator loc;
        loc.pointer = node;

        m_track(node->key, loc);
    }

    // Add x to the specified linked list.
    // If 'list' is null, x becomes the head (i.e. list == x after).
    // Otherwise x is added before the head.
    void add_to_list(heap_node*& list, heap_node *x)
    {
        if (list == nullptr)
        {
            list = x;
            x->left = x;
            x->right = x;
        }
        else
        {
            heap_node* tail = list->left;

            x->left = tail;
            x->right = list;

            tail->right = x;
            list->left = x;
        }
    }

    // Remove x from the specified linked list.
    // If 'list' consisted only of x, it is set to null.
    void remove_from_list(heap_node*& list, heap_node *x)
    {
        // Head and only element
        if (list == x && x->right == x)
        {
            assert(x->left == x);
            list = nullptr;
            return;
        }

        assert(x->right != x && x->left != x);

        // Head but not only element
        if (list == x)
            list = x->right;

        // Unlink
        x->left->right = x->right;
        x->right->left = x->left;
        x->left = x;
        x->right = x;
    }

    void add_child(heap_node *parent, heap_node *child)
    {
        child->p = parent;
        add_to_list(parent->child, child);
    }

    void cut(heap_node *x)
    {
        heap_node *parent = x->p;
        remove_from_list(parent->child, x);
        parent->rank--;
        x->p = nullptr;

        add_to_list(m_min, x);

        x->mark = false;
        if (parent->p == nullptr)
            return;
        
        if (parent->mark)
            cut(parent);
        else
            parent->mark = true;
    }

    void consolidate()
    {
        heap_node *byDegree[64] = {nullptr};
        size_t maxDegree = 0;
        
        while (m_min != nullptr)
        {
            heap_node *adding = m_min;
            remove_from_list(m_min, m_min);

            while (true)
            {
                heap_node*& spot = byDegree[adding->rank];
                if (spot == nullptr)
                {
                    spot = adding;
                    maxDegree = std::max(maxDegree, adding->rank + 1);
                    break;
                }

                if (m_less(spot->key, adding->key))
                {
                    add_child(spot, adding);
                    adding = spot;
                }
                else
                {
                    add_child(adding, spot);
                }

                spot = nullptr;
                adding->rank++;
            }
        }
        
        for (size_t i = 0; i < maxDegree; i++)
        {
            heap_node *root = byDegree[i];
            if (root == nullptr)
                continue;

            add_to_list(m_min, root);
            if (m_less(root->key, m_min->key))
                m_min = root;
        }
    }

public:
    fibonacci_heap()
        : m_min(nullptr)
        , m_size(0)
        , m_numDKs(0)
    {
    }

    int get_num_decrease_keys() { return m_numDKs; }

    void insert(const T& value)
    {
        heap_node *x = new heap_node(value);
        track(x);
        add_to_list(m_min, x);

        if (m_less(x->key, m_min->key))
            m_min = x;

        m_size++;
    }

    void print_list()
    {
        if (m_min != nullptr)
        {
            heap_node *cur = m_min;
            do
            {
                std::cout
                    << "key: " << cur->key
                    << ", left: " << cur->left->key
                    << ", right: " << cur->right->key << "\n";

                cur = cur->right;
            } while(cur != m_min);
        }

        std::cout << "\n";
    }

    const size_t size() const { return m_size; }
    const T& find_min() const { return m_min->key; }
    Lesser& get_lesser() { return m_less; }

    void decrease_key(heap_locator loc, const T& key)
    {
        m_numDKs++;

        heap_node *x = static_cast<heap_node*>(loc.pointer);
        x->key = key;

        if (x->p != nullptr && m_less(x->key, x->p->key))
            cut(x);

        if (m_less(x->key, m_min->key))
            m_min = x;
    }

    void delete_min()
    {
        assert(m_min != nullptr);

        while (m_min->child != nullptr)
        {
            heap_node *node = m_min->child;
            remove_from_list(m_min->child, node);
            add_to_list(m_min, node);
            node->p = nullptr;
        }      

        heap_node *removed = m_min;
        remove_from_list(m_min, m_min);
        m_size--;

        delete removed;

        consolidate();
    }

    static fibonacci_heap<T, Lesser, Track> make_heap(const std::vector<T>& elems)
    {
        fibonacci_heap<T, Lesser, Track> heap;
        
        for (const T& t : elems)
        {
            heap.insert(t);
        }

        return heap;
    }
};

#endif
