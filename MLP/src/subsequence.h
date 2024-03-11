#ifndef SUBSEQUENCE_H
#define SUBSEQUENCE_H


#include <vector>
#include "grafo.h"


struct Subsequence {
    
    /**
     * Variáveis
    */

    int primeiro, ultimo;
    int custoAtraso;
    double duracao, custoAcumulado;


    /**
     * Métodos
    */

    inline static Subsequence concatenar(Subsequence& sigma_1, Subsequence& sigma_2, Grafo& grafo) {
        Subsequence novoSigma;

        double custoAresta = grafo.matriz[sigma_1.ultimo][sigma_2.primeiro];

        novoSigma.primeiro = sigma_1.primeiro;
        novoSigma.ultimo = sigma_2.ultimo;

        novoSigma.custoAtraso = sigma_1.custoAtraso + sigma_2.custoAtraso;
        novoSigma.duracao = sigma_1.duracao + custoAresta + sigma_2.duracao;
        novoSigma.custoAcumulado = (
            + sigma_1.custoAcumulado
            + sigma_2.custoAtraso * (sigma_1.duracao + custoAresta)
            + sigma_2.custoAcumulado
        );

        return novoSigma;
    }
};


#endif