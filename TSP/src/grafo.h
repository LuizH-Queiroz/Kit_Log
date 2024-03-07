#ifndef GRAFO_H
#define GRAFO_H


#include <vector>
#include <string>


class Grafo {
    public:

        /**
         * Variáveis
        */
 
        std::vector<std::vector<double>> matriz;


        /**
         * Métodos
        */
 
        std::vector<int> escolher3NosAleatorios();

        void limpar();

        int qtdVertices();
};


#endif