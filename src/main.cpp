#include "Data.h"
#include "grafo.h"
#include "tsp.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <ratio>

using namespace std;

int main(int argc, char** argv) {
    srand(time(NULL));

    auto data = Data(argc, argv[1]);
    data.read();
    size_t n = data.getDimension();
    ///////////////////////////////

    Grafo grafo;
    grafo.matriz.resize(n+1, vector<double>(n+1));

    // cout << "MATRIZ" << endl;
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            // cout << i << " " << j << endl;
            grafo.matriz[i][j] = data.getDistance(i, j);
        }
    }

    TSP tsp;
    int maxIterILS = 50;
    int maxIterBuscaLocal = (n >= 150 ? n/2 : n);


    double totalTime = 0;
    double betterCost = numeric_limits<double>::max();
    for (int i = 0; i < 1; i++)
    {
        auto start = std::chrono::system_clock::now();

        // cout << "TSP" << endl;
        betterCost = min(betterCost, tsp.ILS(grafo, maxIterILS, maxIterBuscaLocal).custoMinimo);

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsedTime = end - start;

        totalTime += elapsedTime.count();
    }

    std::cout << betterCost << std::endl;
    std::cout << fixed << setprecision(3) << (totalTime / 1.0) << std::endl;


    return 0;
}
