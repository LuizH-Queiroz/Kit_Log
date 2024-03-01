#include "utility.h"



int rangeRandom(int minimo, int maximo) {
    return minimo + (rand() % (maximo - minimo + 1));
}