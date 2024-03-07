#include "utility.h"
#include <iostream>
#include <vector>



int rangeRandom(int minimo, int maximo) {
    return minimo + (rand() % (maximo - minimo + 1));
}


void printVector(std::vector<int> vec) {

    for (int i = 0; i < vec.size(); i++)
    {
        std::cout << vec[i] << " ";
    }
    std::cout << std::endl;
}


bool temIntersecao(int l_1, int r_1, int l_2, int r_2) {
    return (
        (l_1 >= l_2 && l_1 <= r_2)
        ||
        (l_2 >= l_1 && l_2 <= r_1)
    );
}