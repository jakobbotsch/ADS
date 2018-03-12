#ifndef FIBONACCI_HEAP_H
#define FIBONACCI_HEAP_H

#include <functional>
#include <vector>
#include <iostream>
#include <cassert>
#include "binary_heap.h"

template<typename T>
class fib_node
{
public:
    fib_node<T> *p;
    T key;
    size_t rank;
    fib_node<T> *left;
    fib_node<T> *right;
    fib_node<T> *child;
    bool mark;

    fib_node(const T &value)
    : p(nullptr)
    , key(value)
    , rank(0)
    , left(this)
    , right(this)
    , child(nullptr)
    , mark(false)
    {
    }

};

template<typename T, typename Lesser = std::less<T>, typename Track = null_tracker<T>>
class fibonacci_heap
{
private:
    fib_node<T> *min;
    size_t n;

    void track(fib_node<T> *node)
    {
        heap_locator loc;
        loc.pointer = node;

        Track track;
        track(node->key, loc);
    }

    // Add x to the specified linked list.
    // If 'list' is null, x becomes the head (i.e. list == x after).
    // Otherwise x is added before the head.
    void add_to_list(fib_node<T>*& list, fib_node<T> *x)
    {
        if (list == nullptr)
        {
            list = x;
            x->left = x;
            x->right = x;
        }
        else
        {
            fib_node<T>* tail = list->left;

            x->left = tail;
            x->right = list;

            tail->right = x;
            list->left = x;
        }
    }

    // Remove x from the specified linked list.
    // If 'list' consisted only of x, it is set to null.
    void remove_from_list(fib_node<T>*& list, fib_node<T> *x)
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

    void add_child(fib_node<T> *parent, fib_node<T> *child)
    {
        child->p = parent;
        add_to_list(parent->child, child);
    }

    void cut(fib_node<T> *x)
    {
        fib_node<T> *parent = x->p;
        remove_from_list(parent->child, x);
        parent->rank--;
        x->p = nullptr;

        add_to_list(min, x);

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
        std::vector<fib_node<T>*> byDegree;
        
        Lesser lesser;
        while (min != nullptr)
        {
            fib_node<T> *adding = min;
            remove_from_list(min, min);

            while (true)
            {
                if (adding->rank >= byDegree.size())
                    byDegree.resize(adding->rank + 1, nullptr);
                
                fib_node<T>* &spot = byDegree[adding->rank];
                if (spot == nullptr)
                {
                    spot = adding;
                    break;
                }

                if (lesser(spot->key, adding->key))
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
        
        for (fib_node<T> *root : byDegree)
        {
            if (root == nullptr)
                continue;

            add_to_list(min, root);
            if (lesser(root->key, min->key))
                min = root;
        }
    }

public:
    fibonacci_heap()
        : min(nullptr)
        , n(0)
    {
    }

    fib_node<T> *insert(const T& value)
    {
        fib_node<T> *x = new fib_node<T>(value);
        track(x);
        add_to_list(min, x);

        Lesser less;
        if (less(x->key, min->key))
            min = x;

        n++;

        return x;
    }

    void print_list()
    {
        if (min != nullptr)
        {
            fib_node<T> *cur = min;
            do
            {
                std::cout
                    << "key: " << cur->key
                    << ", left: " << cur->left->key
                    << ", right: " << cur->right->key << "\n";

                cur = cur->right;
            } while(cur != min);
        }

        std::cout << "\n";
    }

    const size_t size() const { return n; }
    const T& find_min() const { return min->key; }

    void decrease_key(heap_locator loc, const T& key)
    {
        fib_node<T> *x = static_cast<fib_node<T>*>(loc.pointer);
        x->key = key;

        Lesser less;
        if (x->p != nullptr && less(x->key, x->p->key))
            cut(x);

        if (less(x->key, min->key))
            min = x;
    }

    void delete_min()
    {
        assert(min != nullptr);

        while (min->child != nullptr)
        {
            fib_node<T> *node = min->child;
            remove_from_list(min->child, node);
            add_to_list(min, node);
            node->p = nullptr;
        }      

        fib_node<T> *removed = min;
        remove_from_list(min, min);
        n--;

        delete removed;

        consolidate();
    }

    static fibonacci_heap<T, Lesser> make_heap(const std::vector<T>& elems)
    {
        fibonacci_heap<T, Lesser> heap;
        
        for (const T &t: elems)
        {
            heap.insert(t);
        }

        return heap;
    }
};

#endif
