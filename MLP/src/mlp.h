#ifndef MLP_H
#define MLP_H


#include <vector>
#include <set>
#include "grafo.h"
#include "subsequence.h"


struct Solucao {
    std::vector<int> caminho;
    double custoMinimo;

    static const int _custoInvalido;


    Solucao();

    Solucao(std::vector<int>, double);
};


struct InsertionInfo {
    int noInserido;
    double custoInsercao;

    bool operator<(InsertionInfo) const;
};


struct MLP {

    /**
     * 
     * Variáveis
     * 
    */

    Grafo grafo;
    std::vector<std::vector<Subsequence>> matriz_subseq;


    /**
     * 
     * Métodos
     * 
    */

    void atualizarMatriz(Solucao&, int, int);

    bool bestImprovement2Opt(Solucao&);

    bool bestImprovementOrOpt(Solucao&, int);

    bool bestImprovementSwap(Solucao&);

    void buscaLocal(Solucao&);

    void calcularInsercoes(std::vector<int>&, std::set<int>&, std::vector<InsertionInfo>&);

    Solucao construcao();

    void criarMatrizCompleta(Solucao&);

    Solucao ILS(Grafo, int, int);

    Solucao perturbacao(Solucao);
};


#endif