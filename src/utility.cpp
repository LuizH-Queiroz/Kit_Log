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