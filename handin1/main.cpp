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
    for (int i = 32; i < 5000000; i *= 2)
    {
        std::vector<int> elems;
        for (int j = 0; j < i; j++)
            elems.push_back(rand());

        struct counting_lesser
        {
            int count;

            counting_lesser()
                : count(0)
            {
            }

            bool operator()(const int& left, const int& right)
            {
                count++;
                return left < right;
            }
        };

        double avg = 0;
        const int times = 1;
        for (int j = 0; j < times; j++)
        {
            double start = timestamp_ms();
            auto heap = TPriorityQueue<int, counting_lesser, null_tracker<int>>::make_heap(elems);
            while (heap.size() > 0)
                heap.delete_min();

            avg += heap.get_lesser().count;
        }
        std::cout << std::fixed;
        std::cout << "{" << log2(i) << ", " << (avg / times) << "}, " << std::flush;
    }

    std::cout << "\n";
}

/*srand(12345);
for (int i = 100000; i <= 3000000; i += 100000)
{
    std::vector<int> elems;
    for (int j = 0; j < i; j++)
        elems.push_back(rand());

    struct counting_lesser
    {
        int count;

        bool operator()(const int& left, const int& right)
        {
            return left < right;
        }
    };

    double start = timestamp_ms();
    auto heap = TPriorityQueue<int, counting_lesser, null_tracker<int>>::make_heap(elems);
    while (heap.size() > 0)
        heap.delete_min();

    double elapsed = timestamp_ms() - start;
    std::cout << "{" << i << ", " << elapsed << "}, " << std::flush;
}*/

int main()
{
    measure<binary_heap>("binary heap");
    measure<fibonacci_heap>("fibonacci heap");

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

