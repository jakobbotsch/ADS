#ifndef GRAPH_H
#define GRAPH_H

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
    heap_locator heap_loc;

    bool operator <(const node& rhs) { return cost < rhs.cost; }
};

class graph
{
public:
    std::vector<node> nodes;

    void print_graph()
    {
        std::cout << "digraph graphname {\n";
        for(node &n: nodes){
            //std::cout << &n << "\n";
            //std::cout << n.name << "\n";
            //std::cout << n.edges.size() << "\n";
            for(edge &e: n.edges){
                //std::cout << e.target << "\n";
                std::cout << "\"" << n.name << "\" -> \"" << e.target->name << "\" [label=" << e.weight << "];\n";
            }  
        } 
        std::cout << "}\n";
    }

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

    void connect( size_t index_A, size_t index_B, double weight)
    {
        node *a = &nodes[index_A];
        node *b = &nodes[index_B];
        a->edges.push_back(edge { weight, b });
    }
};

struct node_pointer_compare
{
    bool operator()(node *left, node *right) { return left->cost < right->cost; }
};

struct node_pointer_track
{
    void operator()(node*& n, heap_locator loc) { n->heap_loc = loc; }
};

template<template<typename, typename, typename> typename TPriorityQueue>
void dijkstra(graph& graph, node& start)
{
    for (node& n : graph.nodes)
    {
        //std::cout << "Set to inf\n";
        n.cost = INFINITY;
        //std::cout << "Was set to: " << n.cost <<"\n";
    }

    TPriorityQueue<node*, node_pointer_compare, node_pointer_track> queue;
    start.cost = 0;
    queue.insert(&start);

    while (queue.size() > 0)
    {
        node *min = queue.find_min();
        queue.delete_min();

        for (edge& e : min->edges)
        {
            //std::cout << "edge to node with cost: " << e.target->cost << "\n";
            double newCost = min->cost + e.weight;
            if (newCost >= e.target->cost)
                continue;

            bool inQueue = !std::isinf(e.target->cost);
            e.target->cost = newCost;
            e.target->parent = min;


            if (inQueue)
            {
                //std::cout << "key " << e.target->name << " decreased\n";
                queue.decrease_key(e.target->heap_loc, e.target);
            }
            else
            {
                //std::cout << "new " << e.target->name << " added\n";
                queue.insert(e.target);
            }
        }
    }
}

#endif
