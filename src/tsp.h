#ifndef TSP_H
#define TSP_H


#include <vector>
#include "grafo.h"


class Solucao {
    public:

        std::vector<int> caminho;
        long double custoMinimo;

        static const int _custoInvalido;


        Solucao();

        Solucao(std::vector<int>, long double);
};


class InsertionInfo {
    public:
        int noInserido;
        int indiceArestaRemovida;
        long double custoInsercao;
    
        bool operator<(InsertionInfo) const;
};


class TSP {
    public:

        /**
         * 
         * Variáveis
         * 
        */

        Grafo grafo;


        /**
         * 
         * Métodos
         * 
        */

        bool bestImprovement2Opt(Solucao&);

        bool bestImprovementOrOpt(Solucao&, int);

        bool bestImprovementSwap(Solucao&);

        void buscaLocal(Solucao&);

        std::vector<InsertionInfo> calcularInsercoes(std::vector<int>, std::vector<int>);

        Solucao construcao();

        Solucao ILS(Grafo, int, int);

        Solucao perturbacao(Solucao);
};


#endif