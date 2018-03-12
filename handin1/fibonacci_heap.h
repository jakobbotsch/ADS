#include <functional>
#include <vector>
#include <iostream>

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

template<typename T, typename Lesser = std::less<T>>
class fibonacci_heap
{
private:
    fib_node<T> *min;
    size_t n;

    void concat(fib_node<T> *x){
        if (min == nullptr)
        {
            min = x;
            return;
        }

        x->left = min;
        x->right = min->right;
        min->right->left = x;
        min->right = x;
    }

    void add_child(fib_node<T> *parent, fib_node<T> *child)
    {
        child->p = parent;
        if (parent->child == nullptr)
        {
           parent->child = child;
        }
        child->left->right = child->right;
        child->right->left = child->left;

        child->left = parent->child;
        child->right = parent->child->right;
        parent->child->right->left = child;
        parent->child->right = child;
    }

    void cut(fib_node<T> *x)
    {
        fib_node<T> *p = x->p;
        unlink(x);

        concat(x);

        x->mark = false;
        if(p->p == nullptr)
            return;
        
        if(p->mark)
            cut(p);
        else
            p->mark = true;
    }

    void consolidate()
    {
        //make A
        std::vector<fib_node<T>*> byDegree;
        
        fib_node<T> *cur = min;
        Lesser lesser;
        do
        {
            fib_node<T> *adding = cur; 
            fib_node<T> *next = cur->right;
            while (true)
            {
                if (adding->rank >= byDegree.size())
                {
                    byDegree.resize(adding->rank +1, nullptr);
                }
                
                fib_node<T>* &spot =  byDegree[adding->rank];
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
            cur = next;
        } while(cur != min);
        
        for (fib_node<T> *root : byDegree)
        {   
            if(root == nullptr)
                continue;
            if(lesser(root->key, min->key))
                min = root;
        }

    }

    void unlink(fib_node<T> *x)
    {

        x->left->right = x->right;
        x->right->left = x->left;
        if(x->p != nullptr)
        {
            if(x->p->child == x)
                x->p->child = x->right == x ? nullptr : x->right;
            x->p->rank--;
            x->p = nullptr;
        }  
    }

public:
    fibonacci_heap()
        : min(nullptr)
        , n(0)
    {
    }

    fib_node<T> *insert(const T &value)
    {
        fib_node<T> *x = new fib_node<T>(value);

        concat(x);

        Lesser less;
        if (less(x->key, min->key))
        {
            min = x;
        }

        n++;

        return x;
    }

    void print_list()
    {
        fib_node<T> *cur = min;
        do
        {
            std::cout << "key: " << cur->key << ", left: " << cur->left->key << ", right: " << cur->right->key << "\n";
            cur = cur->right;
        }while(cur != min);

        std::cout << "\n";
    }

    const T& find_min()
    {
        std::cout << min <<" ";
        return min->key;
    }

    void decrease_key(fib_node<T> *x, const T& key){
        Lesser less;
        x->key = key;
        if(x->p != nullptr && less(x->key, x->p->key))
        {
            cut(x);
        }

        if(less(x->key, min->key))
        {
            min = x;
        }
    }

    void delete_min()
    {
        if(min == nullptr)
            return;

        fib_node<T> *cur = min->child;
        if(cur != nullptr)
        {
            do
            {
                concat(cur);
                cur->p = nullptr;
                cur = cur->right;
            }
            while(cur != min->child);
        }      

        unlink(min);
        
        if(min->right == min)
            min = nullptr;
        else
        {
            min = min->right;
            consolidate();
        }

        n--;
    }

    static fibonacci_heap<T, Lesser> make_heap(const std::vector<T>& elems)
    {
        fibonacci_heap<T, Lesser> heap;
        
        for(const T &t: elems)
        {
            heap.insert(t);
        }

        return heap;
    }
};