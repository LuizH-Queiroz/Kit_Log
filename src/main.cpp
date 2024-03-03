#include "Data.h"
#include "grafo.h"
#include "tsp.h"
#include <iostream>
#include <algorithm>

using namespace std;

int main(int argc, char** argv) {
    srand(time(NULL));

    auto data = Data(argc, argv[1]);
    data.read();
    size_t n = data.getDimension();
    ///////////////////////////////

    Grafo grafo;
    grafo.matriz.resize(n+1, vector<long double>(n+1));

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
    int maxIterILS = (n < 150 ? n : ceil(n/2));
    int maxIterBuscaLocal = 50;

    cout << "TSP" << endl;
    cout << tsp.ILS(grafo, maxIterILS, maxIterBuscaLocal).custoMinimo << endl;


    return 0;
}
