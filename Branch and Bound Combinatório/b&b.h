#ifndef BB_H
#define BB_H


#include <vector>
#include <list>
#include <queue>
#include "grafo.h"
#include "hungarian.h"



/**
 * Structure to represent the Branch and Boud Tree's nodes
*/
struct Node {

    /**
     * Variables
    */
    std::vector<std::pair<int, int>>    forbidden_arcs;
    std::vector<std::vector<int>>       subtours;

    double  lower_bound;
    int     smallest_subtour_index;
    bool    feasible;


    /**
     * Operators
    */

    bool operator<(Node const &) const;
                
    bool operator>(Node const &) const;
};


/**
 * Structure meant to use the Branch and Bound algorithm to solve any TSP instance
*/
struct BB_TSP {

    // Defining the Branching Strategies available
    enum BRANCHING_STRATEGY {
        DFS,
        BFS,
        MINIMUM_LOWER_BOUND
    };

    // "numeric_limits<double>::infinity()" might cause problems in the Hungarian
    // Algorithm implementation. So this one shall be used instead
    static double MATRIX_INFINITY;


    /**
     * Variables
    */

    // TSP's instance graph
    Grafo graph;

    // Branching Strategy that must be used
    BRANCHING_STRATEGY branching_strategy;


    /*
        Structures to represent the Branch and Bound Tree.
        Which one to use will depend on the "branching_strategy"
    */
    std::list<Node> listTree;

    // "priority_queue" is a Maximum Binary Heap by default. "std::greater" turns it
    // into a Minimum Binary Heap instead
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pqTree;


    /**
     * Methods
    */

    // Deletes the last returned Node by the "getNextNode" method
    void deleteNode();

    // Modify the input vector so it contains all the subtours from the last
    // Assigning Problem solved by the Hungarian Algorithm
    void    getSubtours(std::vector<std::vector<int>>&);

    // Returns the next node to be processed, depending on the "branching_strategy"
    Node&   getNextNode();

    double  solveTSP(Grafo const &, BRANCHING_STRATEGY);

    // If there are no more Nodes, return true.
    // Returns false otherwise.
    bool    treeIsEmpty();
};


#endif