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

    static Subsequence concatenar(Subsequence& , Subsequence&, Grafo&);
};


#endif