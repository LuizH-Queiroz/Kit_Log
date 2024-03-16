#include "b&b.h"
#include <iostream>
#include <limits>



/******************************************* Node Class *******************************************/

bool Node::operator<(Node const& other) const {
    return lower_bound < other.lower_bound;
}
            
bool Node::operator>(Node const& other) const {
    return lower_bound > other.lower_bound;
}


/******************************************* BB_TSP Class *******************************************/

/**
 * Variables
*/

double BB_TSP::ARC_INFINITY = 99999999;


/**
 * Methods
*/

Node BB_TSP::getNextNode() {
    Node next;

    switch(branching_strategy)
    {
        case BRANCHING_STRATEGY::DFS:
            next = listTree.back();
            listTree.pop_back();
            break;

        case BRANCHING_STRATEGY::BFS:
            next = listTree.front();
            listTree.pop_front();
            break;

        case BRANCHING_STRATEGY::MINIMUM_LOWER_BOUND:
            next = pqTree.top();
            pqTree.pop();
            break;

        default:
            std::cout << ">> getNextNode() --> Invalid Branching Strategy!" << std:: endl;
            exit(EXIT_FAILURE);
            break;
    }

    return next;
}


void BB_TSP::insertNode(Node& node) {

    switch(branching_strategy)
    {
        case BRANCHING_STRATEGY::DFS:
            listTree.push_back(node);
            break;

        case BRANCHING_STRATEGY::BFS:
            listTree.push_back(node);
            break;

        case BRANCHING_STRATEGY::MINIMUM_LOWER_BOUND:
            pqTree.push(node);
            break;

        default:
            std::cout << ">> insertNode() --> Invalid Branching Strategy!" << std:: endl;
            exit(EXIT_FAILURE);
            break;
    }
}


void BB_TSP::setSubtours(Node& node, int **assignment) {
    // True when a vertex has already been inserted in a subtour.
    // False otherwise
    std::vector<bool> used(dimension, false);

    for (int startVertex = 0; startVertex < dimension; startVertex++)
    {
        if (!used[startVertex])
        {
            std::vector<int> newSubtour = {startVertex};
            used[startVertex] = true;


            int currVertex = startVertex;
            for (int i = 0; i < dimension; i++)
            {
                if (assignment[currVertex][i] == 1)
                {
                    newSubtour.push_back(i);

                    // If "currVertex" is assigned to an already used vertex "i",
                    // then "i" is the beginning of the cycle.
                    // Subtour completed.
                    if (used[i])
                    {
                        break;
                    }
                    used[i] = true; // "i" got included in a subtour
                    currVertex = i;
                    i = -1;
                }
            }


            // Adding the new subtour to the "node" vector
            if ((node.subtours.size() == 0) || (newSubtour.size() < node.subtours[node.smallest_subtour_index].size()))
            {
                node.smallest_subtour_index = node.subtours.size();
            }
            node.subtours.push_back(newSubtour);
        }
    }
}


void BB_TSP::solveNode(Node& node) {
    // static int counter = 0;
    // std::cout << "solveNode() -> in " << ++counter << std::endl;
    // Vector to store the original forbidden arcs values, so it can be
    // undone at the end of this method
    std::vector<double> prevValues(node.forbidden_arcs.size());

    for (int i = 0; i < node.forbidden_arcs.size(); i++)
    {
        int first = node.forbidden_arcs[i].first;
        int second = node.forbidden_arcs[i].second;

        prevValues[i] = graph[first][second];
        graph[first][second] = ARC_INFINITY;
    }

    // Using the Hungarian Algorithm to solve the Assigning Problem on the
    // modified graph/matrix
    hungarian_problem_t p;
    hungarian_init(&p, graph, dimension, dimension, HUNGARIAN_MODE_MINIMIZE_COST);

    node.lower_bound = hungarian_solve(&p);
    setSubtours(node, p.assignment);
    node.feasible = (node.subtours.size() == 1);

    hungarian_free(&p);


    // Reverting the modifications made on the graph's matrix
    for (int i = 0; i < node.forbidden_arcs.size(); i++)
    {
        int first = node.forbidden_arcs[i].first;
        int second = node.forbidden_arcs[i].second;

        graph[first][second] = prevValues[i];
    }
    // std::cout << "solveNode() -> out" << std::endl;
}


double BB_TSP::solveTSP(double **matrix, int size, BRANCHING_STRATEGY strategy) {
    // Initializing the BB_TSP variables
    graph = matrix;
    dimension = size;
    branching_strategy = strategy;
    ///////////////////////

    double upper_bound = std::numeric_limits<double>::infinity();

    // Solving the Assignment Problem from the original instance
    Node root;
    insertNode(root);

    while(!treeIsEmpty())
    {
        // std::cout << std::endl;
        // std::cout << "upper_bound = " << upper_bound << std::endl;
        // std::cout << "treeSize = " << listTree.size() << " " << pqTree.size() << std::endl;
        Node currNode = getNextNode();
        solveNode(currNode);

        if (currNode.lower_bound >= upper_bound)
        {
            continue;
        }

        if (currNode.feasible)
        {
            upper_bound = currNode.lower_bound;
        }
        else // currNode.feasible == false
        {
            std::vector<int>& subtour = currNode.subtours[currNode.smallest_subtour_index];

            // As currNode is not feasible, lets add new restrictions
            // and solve it again until it becomes feasible (or too expensive)
            for (int i = 0; i < subtour.size()-1; i++)
            {
                Node newNode;
                
                newNode.forbidden_arcs = currNode.forbidden_arcs;
                newNode.forbidden_arcs.push_back(std::make_pair(
                    subtour[i],
                    subtour[i+1]
                ));

                insertNode(newNode);
            }
        }
    }

    // At the end, "upper_bound" stores the minimum cost
    return upper_bound;
}


bool BB_TSP::treeIsEmpty() {
    bool ans;

    switch(branching_strategy) {
        case BRANCHING_STRATEGY::DFS:
            ans = listTree.empty();
            break;

        case BRANCHING_STRATEGY::BFS:
            ans = listTree.empty();
            break;

        case BRANCHING_STRATEGY::MINIMUM_LOWER_BOUND:
            ans = pqTree.empty();
            break;

        default:
            std::cout << ">> BB_TSP::treeIsEmpty() --> Invalid Branching Strategy!" << std::endl;
            exit(EXIT_FAILURE);
            break;
    }

    return ans;
}