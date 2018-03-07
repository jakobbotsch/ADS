#include <string>
#include <vector>
#include <cmath>
#include <cfloat>

class node;

struct edge
{
    double weight;
    node *target;
};

class node
{
public:
    node(const std::string& name)
        : name(name)
        , parent(nullptr)
        , cost(0)
    {
    }

    std::string name;
    std::vector<edge> edges;
    node *parent;
    double cost;
    size_t heapIndex;

    bool operator <(const node& rhs) { return cost < rhs.cost; }
};

class graph
{
public:
    std::vector<node> nodes;

    void add_node(const std::string& name)
    {
        nodes.push_back(node(name));
    }

    node *find_node(const std::string& name)
    {
        for (node& n : nodes)
        {
            if (n.name == name)
                return &n;
        }

        return nullptr;
    }

    void connect(const std::string& nameA, const std::string& nameB, double weight)
    {
        node *a = find_node(nameA);
        node *b = find_node(nameB);
        a->edges.push_back(edge { weight, b });
    }
};

struct node_pointer_compare
{
    bool operator()(node *left, node *right) { return left->cost < right->cost; }
};

struct node_pointer_track
{
    void operator()(node *& n, size_t index) { n->heapIndex = index; }
};

template<template<typename, typename, typename> typename TPriorityQueue>
void dijkstra(graph& graph, node& start)
{
    for (node& n : graph.nodes)
        n.cost = INFINITY;

    TPriorityQueue<node*, node_pointer_compare, node_pointer_track> queue;
    queue.insert(&start);
    start.cost = 0;

    while (queue.size() > 0)
    {
        node *min = queue.find_min();
        queue.delete_min();

        for (edge& e : min->edges)
        {
            double newCost = min->cost + e.weight;
            if (newCost >= e.target->cost)
                continue;

            bool inQueue = !std::isinf(e.target->cost);
            e.target->cost = newCost;
            e.target->parent = min;

            if (inQueue)
                queue.decrease_key(e.target->heapIndex, e.target);
            else
                queue.insert(e.target);
        }
    }
}
