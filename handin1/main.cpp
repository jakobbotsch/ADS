#include <iostream>
#include <vector>
#include "binary_heap.h"
#include "graph.h"
#include "fibonacci_heap.h"

int main()
{
    std::vector<int> elems { 5, 4, 2, 3, 1 };
    auto heap = fibonacci_heap<int>::make_heap(elems);
    for(int i = 0; i <3 ; i++)
    {
        std::cout << heap.find_min() << " ";
        heap.delete_min();
    }

    // fib_node<int> *x = heap.insert(6);
    // heap.print_list();
    // heap.decrease_key(x, 0);
    // heap.print_list();
    /*heap.dump_list();

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

    dijkstra<binary_heap>(g, *g.find_node("s"));
    for (node n : g.nodes)
    {
        std::cout
            << n.name
            << ": parent = "
            << (n.parent ? n.parent->name : "(null)")
            << ", cost = " << n.cost << "\n";
    }*/

    return 0;
}

