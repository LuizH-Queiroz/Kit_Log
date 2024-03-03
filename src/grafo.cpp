#include <set>
#include <algorithm>
#include "grafo.h"
#include "utility.h"



std::vector<int> Grafo::escolher3NosAleatorios() {
    std::vector<int> nos;

    for (int i = 0; nos.size() < 3; i++)
    {
        int no;
        do {
            no = rangeRandom(1, qtdVertices());
        } while (std::find(nos.begin(), nos.end(), no) != nos.end());

        // Verifica se o no escolhido ainda nao foi usado
        nos.push_back(no);
    }

    return nos;
}


void Grafo::limpar() {
    matriz.clear();
}


int Grafo::qtdVertices() {
    return matriz.size()-1;
}