#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include "binary_heap.h"
#include "graph.h"
#include "fibonacci_heap.h"

double timestamp_ms()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count() / 1000.0;
}

template<template<typename, typename, typename> typename TPriorityQueue>
void measure(const char *name)
{
    std::cout << "Trying " << name << "\n";

    srand(12345);
    for (double e = 6; e <= 22; e += 0.25)
    {
        size_t n = static_cast<size_t>(exp2(e));
        n = 5000000;

        std::vector<int> elems;
        for (int i = 0; i < n; i++)
            elems.push_back(rand());

        double avg = 0;
        const int times = 5;
        for (int i = 0; i < times; i++)
        {
            double start = timestamp_ms();
            auto heap = TPriorityQueue<int, std::less<int>, null_tracker<int>>::make_heap(elems);
            int prev = -0x80000000;
            while (heap.size() > 0)
            {
                assert(heap.find_min() >= prev);
                prev = heap.find_min();
                heap.delete_min();
            }

            avg += timestamp_ms() - start;
        }
        std::cout << std::fixed;
        std::cout << "{" << e << ", " << (avg / times) << "}, " << std::flush;
break;
    }

    std::cout << "\n";
}

template<template<typename, typename, typename> typename TPriorityQueue>
void measure_graph(const char *name)
{
    std::cout << "\n" << name << "\n";

    for (double e = 6; e <= 14; e += 0.25)
    {
        size_t n = static_cast<size_t>(exp2(e));
        n = 20000;
        graph g;
        for (size_t i = 0; i < n; i++)
        {
            std::string name_i = std::to_string((int) i);
            g.add_node(name_i);
        }
        for (size_t i = 0; i < n; i++)
        {
            for (size_t j = i; j-- > 0; )
            //for (size_t j = 0; j < i; j++)
            {
                g.connect(j, i, (2*(i-j-1)+1));
            }
        }

        double avg = 0;
        const int times = 5;
        for (int j = 0; j < times; j++)
        {
            double start = timestamp_ms();
            dijkstra<TPriorityQueue>(g, *g.find_node("0"));
            avg += timestamp_ms()-start;
        }
        std::cout << std::fixed;
        std::cout << "{" << n << ", " << (avg / times) << "}, " << std::flush;
        break;
    }

    std::cout << std::endl;
};

int main()
{
    measure_graph<binary_heap>("binary heap");
    measure_graph<fibonacci_heap>("fibonacci heap");

    return 0;
    // fib_node<int> *x = heap.insert(6);
    // heap.print_list();
    // heap.decrease_key(x, 0);
    // heap.print_list();
    // heap.dump_list();

    graph g;
    g.add_node("s");
    g.add_node("t");
    g.add_node("x");
    g.add_node("y");
    g.add_node("z");
    g.connect("s", "t", 10);
    g.connect("s", "y", 5);
    g.connect("t", "x", 1);
    g.connect("t", "y", 2);
    g.connect("y", "t", 3);
    g.connect("y", "z", 2);
    g.connect("y", "x", 9);
    g.connect("x", "z", 4);
    g.connect("z", "s", 7);
    g.connect("z", "x", 6);

    dijkstra<fibonacci_heap>(g, *g.find_node("s"));
    for (node n : g.nodes)
    {
        std::cout
            << n.name
            << ": parent = "
            << (n.parent ? n.parent->name : "(null)")
            << ", cost = " << n.cost << "\n";
    }

    return 0;
}

