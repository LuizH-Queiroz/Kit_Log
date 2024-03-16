#ifndef BB_TSP_H
#define BB_TSP_H


#include <vector>
#include <list>
#include <queue>
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
    static double ARC_INFINITY;


    /**
     * Variables
    */

    // TSP's instance graph (matrix)
    double **graph;
    int dimension; // graph's number of vertices

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

    // Returns the next node to be processed, depending on the "branching_strategy"
    Node    getNextNode();

    // Insert the Node at the right Data Structure
    void    insertNode(Node&);

    // Modify the input node so it contains all the subtours obtained from the
    // Hungarian Algorithm
    void    setSubtours(Node&, int**);

    // Uses the Hungarian Algorithm to solve the Node and returns
    // the obtained cost
    void    solveNode(Node&);

    double  solveTSP(double**, int, BRANCHING_STRATEGY);

    // If there are no more Nodes, return true.
    // Returns false otherwise.
    bool    treeIsEmpty();
};


#endif