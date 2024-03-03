#include <algorithm>
#include <numeric>
#include <random>
#include <chrono>
#include <iostream>
#include "tsp.h"
#include "utility.h"



/***************************** Classe Solucao *****************************/

const int Solucao::_custoInvalido = -1;


Solucao::Solucao() {
    custoMinimo = _custoInvalido;
}


Solucao::Solucao(std::vector<int> caminho, long double custoMinimo) {
    this->caminho = caminho;
    this->custoMinimo = custoMinimo;
}


/***************************** Classe InsertionInfo *****************************/

bool InsertionInfo::operator<(InsertionInfo other) const {
    return custoInsercao < other.custoInsercao;
}


/***************************** Classe TSP *****************************/

bool TSP::bestImprovement2Opt(Solucao& s) {
    long double melhorDelta = 0;
    int melhor_i, melhor_j;


    for (int i = 1; i < s.caminho.size()-1; i++)
    {
        int vi = s.caminho[i];
        int vi_prev = s.caminho[i-1];

        for (int j = i+1; j < s.caminho.size()-1; j++)
        {
            int vj = s.caminho[j];
            int vj_next = s.caminho[j+1];

            long double delta = (
                // Retirada dos vértices de suas posições
                - (grafo.matriz[vi][vi_prev])
                - (grafo.matriz[vj][vj_next])

                // Inserção dos vértices nas suas novas posições
                + (grafo.matriz[vi][vj_next])
                + (grafo.matriz[vj][vi_prev])
            );

            if (delta < melhorDelta)
            {
                melhorDelta = delta;
                melhor_i = i;
                melhor_j = j;
            }
        }
    }


    if (melhorDelta < 0)
    {
        std::reverse(s.caminho.begin() + melhor_i, s.caminho.begin() + melhor_j + 1);
        s.custoMinimo = s.custoMinimo + melhorDelta;
        return true;
    }
    return false;
}


bool TSP::bestImprovementOrOpt(Solucao& s, int tamanhoConjunto) {
    long double melhorDelta = 0;
    int inicioSegmento, novoInicio;


    for (int i = 1; (i + tamanhoConjunto) < s.caminho.size()-1; i++)
    {
        int v_inicio = s.caminho[i];
        int v_inicioPrev = s.caminho[i-1];

        int v_final = s.caminho[i + (tamanhoConjunto - 1)];
        int v_finalNext = s.caminho[i + tamanhoConjunto];

        for (int j = 1; j < s.caminho.size()-1; j++)
        {
            if (j >= i && j <= (i + tamanhoConjunto))
            {
                continue;
            }

            // Vértices que agora ficarão, respectivamente, antes e depois do segmento
            int newPrev = s.caminho[j-1];
            int newNext = s.caminho[j];

            long double delta = (
                // Retirada do segmento
                - (grafo.matriz[v_inicio][v_inicioPrev] + grafo.matriz[v_final][v_finalNext])

                // Inserindo aresta que conecta os vértices que vêm antes e depois do segmento
                + (grafo.matriz[v_inicioPrev][v_finalNext])

                /**
                 * Agora vem a parte relativa à posição de inserção do segmento
                */
                // Retirada da aresta onde será posto o segmento
                - (grafo.matriz[newPrev][newNext])

                // Inserção dos vértices nas suas novas posições
                + (grafo.matriz[v_inicio][newPrev] + grafo.matriz[v_final][newNext])
            );


            if (delta < melhorDelta)
            {
                melhorDelta = delta;
                inicioSegmento = i;
                novoInicio = j;
            }
        }
    }


    if (melhorDelta < 0)
    {
        // Realocando os vértices
        std::vector<int> segmento;
        for (int i = inicioSegmento; i < (inicioSegmento + tamanhoConjunto); i++)
        {
            segmento.push_back(s.caminho[i]);
        }

        s.caminho.erase(s.caminho.begin() + inicioSegmento, s.caminho.begin() + (inicioSegmento + tamanhoConjunto));

        // Talvez o índice do novo início precise ser atualizado (se for depois de onde estava o segmento)
        if (novoInicio > inicioSegmento)
        {
            novoInicio -= tamanhoConjunto;
        }
        s.caminho.insert(s.caminho.begin() + novoInicio, segmento.begin(), segmento.end());
        
        // Atualizando o custo mínimo
        s.custoMinimo = s.custoMinimo + melhorDelta;
        return true;
    }
    return false;
}


bool TSP::bestImprovementSwap(Solucao& s) {
    long double melhorDelta = 0;
    int melhor_i, melhor_j;

    std::cout << "swap: ";
    printVector(s.caminho);
    for (int i = 1; i < s.caminho.size()-1; i++)
    {
        int vi = s.caminho[i];
        int vi_prev = s.caminho[i-1];
        int vi_next = s.caminho[i+1];

        for (int j = i+1; j < s.caminho.size()-1; j++)
        {
            int vj = s.caminho[j];
            int vj_prev = s.caminho[j-1];
            int vj_next = s.caminho[j+1];

            long double delta = (
                // Retirada dos vértices de suas posições
                - (grafo.matriz[vi][vi_prev] + grafo.matriz[vi][vi_next])
                - (grafo.matriz[vj][vj_prev] + grafo.matriz[vj][vj_next])

                // Inserção dos vértices nas suas novas posições
                + (grafo.matriz[vi][vj_prev] + grafo.matriz[vi][vj_next])
                + (grafo.matriz[vj][vi_prev] + grafo.matriz[vj][vi_next])
            );

            if (abs(i-j) == 1)
            {
                delta += 2*grafo.matriz[vi][vj];
            }

            if (delta < melhorDelta)
            {
                melhorDelta = delta;
                melhor_i = i;
                melhor_j = j;
            }
        }
    }


    if (melhorDelta < 0)
    {
        std::swap(s.caminho[melhor_i], s.caminho[melhor_j]);
        s.custoMinimo = s.custoMinimo + melhorDelta;
        return true;
    }
    return false;
}


void TSP::buscaLocal(Solucao& s) {
    std::vector<int> NL = {1, 2};

    while(!NL.empty())
    {
        int n = rangeRandom(0, NL.size()-1);
        bool improved = false;

        switch(NL[n]) {
            case 1:
                improved = bestImprovementSwap(s);
                // std::cout << "improved (1) = " << improved << std::endl;
                break;

            case 2:
                improved = bestImprovement2Opt(s);
                // std::cout << "improved (2) = " << improved << std::endl;
                break;

            case 3:
                improved = bestImprovementOrOpt(s, 1);
                // std::cout << "improved (3) = " << improved << std::endl;
                break;

            case 4:
                improved = bestImprovementOrOpt(s, 2);
                // std::cout << "improved (4) = " << improved << std::endl;
                break;

            case 5:
                improved = bestImprovementOrOpt(s, 3);                
                // std::cout << "improved (5) = " << improved << std::endl;
                break;
        }


        if (improved)
        {
            NL = {1, 2};
        }
        else
        {
            NL.erase(NL.begin() + n);
        }
    }
}


std::vector<InsertionInfo> TSP::calcularInsercoes(std::vector<int> subtour, std::vector<int> CL) {
    // std::cout << "START calcularInsercoes?" << std::endl;
    std::vector<InsertionInfo> insercoes(CL.size() * (subtour.size() - 1));
    int indiceInsercao = 0;
    // std::cout << "START calcularInsercoes ok" << std::endl;

    for (int verticeCL : CL)
    {
        for (int i = 0; i < subtour.size()-1; i++)
        {
            int v1 = subtour[i];
            int v2 = subtour[i+1];
            long double custo = grafo.matriz[verticeCL][v1] + grafo.matriz[verticeCL][v2] - grafo.matriz[v1][v2];

            insercoes[indiceInsercao].noInserido = verticeCL;
            insercoes[indiceInsercao].indiceArestaRemovida = i;
            insercoes[indiceInsercao].custoInsercao = custo;
            indiceInsercao++;
        }
    }

    return insercoes;
}


Solucao TSP::construcao() {
    Solucao s;

    // Caminho de solução inicial
    s.caminho = grafo.escolher3NosAleatorios();
    s.caminho.push_back(s.caminho.front());

    // Conjunto dos vértices (CL) na verdade é inicializado com todos os vértices
    std::vector<int> CL(grafo.qtdVertices());
    std::iota(CL.begin(), CL.end(), 1);

    // Retira do conjunto CL os vértices que já fazem parte da solução
    for (int vertice : s.caminho)
    {
        std::vector<int>::iterator iteradorDeletar = std::find(CL.begin(), CL.end(), vertice);

        if (iteradorDeletar != CL.end())
        {
            CL.erase(iteradorDeletar);
        }
    }

    // A cada iteração um dos vértices de CL será inserido na solução
    while(!CL.empty())
    {
        // Vetor com todas as inserções possíveis
        std::vector<InsertionInfo> insercoes = calcularInsercoes(s.caminho, CL);

        // Ordena o vetor de inserções em ordem crescente de custo de inserção
        std::sort(insercoes.begin(), insercoes.end());

        // Porcentagem das melhores inserções que serão consideradas para inserção
        long double alpha = (long double) rangeRandom(1, 100) / 100.0;

        // Valor mais alto de um índice que pode ser escolhido para inserção
        int limite = (CL.size() - 1) * alpha;

        // Insere o novo vértice na solucao
        InsertionInfo escolhido = insercoes[rangeRandom(0, limite)];
        s.caminho.insert(s.caminho.begin()+(escolhido.indiceArestaRemovida+1), escolhido.noInserido);

        // Como o nó agora faz parte da solução, ele deve ser excluído de CL
        std::vector<int>::iterator iteradorDeletar = std::find(CL.begin(), CL.end(), escolhido.noInserido);
        CL.erase(iteradorDeletar);
    }

    // Calcula o custo total do caminho construído
    s.custoMinimo = 0;
    for (int i = 0; i < s.caminho.size()-1; i++)
    {
        int v1 = s.caminho[i];
        int v2 = s.caminho[i+1];

        s.custoMinimo += grafo.matriz[v1][v2];
    }

    return s;
}


Solucao TSP::ILS(Grafo grafo, int maxIterILS, int maxIterBuscaLocal) {
    this->grafo = grafo;
    Solucao melhorSolucao;

    for (int i = 0; i < maxIterILS; i++)
    {
        Solucao s = construcao();
        Solucao melhorIter = s;
        // std::cout << "CONSTRUCAO " << (i+1) << ", caminho: ";
        // printVector(s.caminho);

        int contador = 0;
        for (int j = 0; j < maxIterBuscaLocal; j++)
        {
            std::cout << "Busca Local " << ++contador << std::endl;
            buscaLocal(s);
            if (s.custoMinimo < melhorIter.custoMinimo)
            {
                melhorIter = s;
                j = 0;
            }

            s = perturbacao(melhorIter);
        }


        if (melhorIter.custoMinimo < melhorSolucao.custoMinimo || melhorSolucao.custoMinimo == Solucao::_custoInvalido)
        {
            melhorSolucao = melhorIter;
        }
    }

    return melhorSolucao;
}


Solucao TSP::perturbacao(Solucao s) {
    return s;
}