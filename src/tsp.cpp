#include <algorithm>
#include <numeric>
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

std::vector<InsertionInfo> TSP::calcularInsercoes(std::vector<int> subtour, std::vector<int> CL) {
    std::vector<InsertionInfo> insercoes(CL.size() * (subtour.size() - 1));

    for (int verticeCL : CL)
    {
        for (int i = 0; i < subtour.size()-1; i++)
        {
            int v1 = subtour[i];
            int v2 = subtour[i+1];
            long double custo = grafo.matriz[verticeCL][v1] + grafo.matriz[verticeCL][v2] - grafo.matriz[v1][v2];

            insercoes[i].noInserido = verticeCL;
            insercoes[i].indiceArestaRemovida = i;
            insercoes[i].custoInsercao = custo;
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
        CL.erase(iteradorDeletar);
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
        int limite = CL.size() * alpha;

        // Insere o novo vértice na solucao
        InsertionInfo escolhido = insercoes[rangeRandom(0, limite)];
        s.caminho.insert(s.caminho.begin()+escolhido.indiceArestaRemovida, escolhido.noInserido);

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
    return Solucao();
}