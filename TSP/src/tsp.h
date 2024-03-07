#ifndef TSP_H
#define TSP_H


#include <vector>
#include "grafo.h"


class Solucao {
    public:

        std::vector<int> caminho;
        double custoMinimo;

        static const int _custoInvalido;


        Solucao();

        Solucao(std::vector<int>, double);
};


class InsertionInfo {
    public:
        int noInserido;
        int indiceArestaRemovida;
        double custoInsercao;
    
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

        void calcularInsercoes(std::vector<int>, std::vector<int>, std::vector<InsertionInfo>&);

        Solucao construcao();

        Solucao ILS(Grafo, int, int);

        Solucao perturbacao(Solucao);
};


#endif