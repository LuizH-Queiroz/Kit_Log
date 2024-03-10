#include <algorithm>
#include <numeric>
#include <random>
#include <chrono>
#include <iostream>
#include <set>
#include "mlp.h"
#include "utility.h"



/***************************** Classe Solucao *****************************/

const int Solucao::_custoInvalido = -1;


Solucao::Solucao() {
    custoMinimo = _custoInvalido;
}


Solucao::Solucao(std::vector<int> caminho, double custoMinimo) {
    this->caminho = caminho;
    this->custoMinimo = custoMinimo;
}


/***************************** Classe InsertionInfo *****************************/

bool InsertionInfo::operator<(InsertionInfo other) const {
    return custoInsercao < other.custoInsercao;
}


/***************************** Classe MLP *****************************/


/**
 * inicio: inicio do segmento que foi alterado
 * final: final do segmento que foi alterado
*/
void MLP::atualizarMatriz(Solucao& s, int inicio, int final) {
    int qtdVertices = s.caminho.size();

    // Atualizando as subsequências de vértices únicos (tamanho 1)
    // que foram modificadas
    for (int i = inicio; i <= final; i++)
    {
        matriz_subseq[i][i].primeiro = s.caminho[i];
        matriz_subseq[i][i].ultimo = s.caminho[i];
        
        matriz_subseq[i][i].custoAtraso = (i > 0);
        matriz_subseq[i][i].duracao = 0;
        matriz_subseq[i][i].custoAcumulado = 0;
    }


    // Atualizando as subsequências não invertidas
    for (int i = 0; i <= final; i++)
    {
        for (int j = std::max(inicio, i+1); j < qtdVertices; j++)
        {
            matriz_subseq[i][j] = Subsequence::concatenar(
                matriz_subseq[i][j-1],
                matriz_subseq[j][j],
                grafo
            );
        }
    }

    // Atualizando as subsequências invertidas
    for (int i = qtdVertices-1; i >= inicio; i--)
    {
        for (int j = std::min(final, i-1); j >= 0; j--)
        {
            matriz_subseq[i][j] = Subsequence::concatenar(
                matriz_subseq[i][j+1],
                matriz_subseq[j][j],
                grafo
            );
        }
    }
}


bool MLP::bestImprovement2Opt(Solucao& s) {
    double melhorDelta = 0;
    int melhor_i, melhor_j;


    for (int i = 1; i < s.caminho.size()-1; i++)
    {
        int vi = s.caminho[i];
        int vi_prev = s.caminho[i-1];

        for (int j = i+1; j < s.caminho.size()-1; j++)
        {
            int vj = s.caminho[j];
            int vj_next = s.caminho[j+1];

            double delta = (
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


bool MLP::bestImprovementOrOpt(Solucao& s, int tamanhoConjunto) {
    double melhorDelta = 0;
    int inicioSegmento, novoInicio;

    for (int i = 1; (i + tamanhoConjunto) < s.caminho.size()-1; i++)
    {
        int v_inicio = s.caminho[i];
        int v_inicioPrev = s.caminho[i-1];

        int v_final = s.caminho[i + (tamanhoConjunto - 1)];
        int v_finalNext = s.caminho[i + tamanhoConjunto];

        for (int j = 1; j < s.caminho.size(); j++)
        {
            if (j >= i && j <= (i + tamanhoConjunto))
            {
                continue;
            }

            // Vértices que agora ficarão, respectivamente, antes e depois do segmento
            int newPrev = s.caminho[j-1];
            int newNext = s.caminho[j];

            double delta = (
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
        if (novoInicio < inicioSegmento)
        {
            std::rotate(
                s.caminho.begin() + novoInicio,
                s.caminho.begin() + inicioSegmento,
                s.caminho.begin() + inicioSegmento + tamanhoConjunto
            );
        }
        else // inicioSegmento < novoInicio
        {
            std::rotate(
                s.caminho.begin() + inicioSegmento,
                s.caminho.begin() + inicioSegmento + tamanhoConjunto,
                s.caminho.begin() + novoInicio
            );
        }

        // Atualizando o custo mínimo
        s.custoMinimo = s.custoMinimo + melhorDelta;
        return true;
    }
    return false;
}


bool MLP::bestImprovementSwap(Solucao& s) {
    double melhorDelta = s.custoMinimo;
    int melhor_i, melhor_j;

    for (int i = 1; i < s.caminho.size()-1; i++)
    {
        for (int j = i+1; j < s.caminho.size()-1; j++)
        {
            Subsequence sigma = Subsequence::concatenar(matriz_subseq[0][i-1], matriz_subseq[j][j], grafo);

            if (j != i+1)
            {
                sigma = Subsequence::concatenar(sigma, matriz_subseq[i+1][j-1], grafo);
            }
            sigma = Subsequence::concatenar(sigma, matriz_subseq[i][i], grafo);
            sigma = Subsequence::concatenar(sigma, matriz_subseq[j+1][s.caminho.size()-1], grafo);

            if (sigma.custoAcumulado < melhorDelta)
            {
                melhorDelta = sigma.custoAcumulado;
                melhor_i = i;
                melhor_j = j;
            }
        }
    }


    if (melhorDelta < s.custoMinimo)
    {
        std::swap(s.caminho[melhor_i], s.caminho[melhor_j]);
        s.custoMinimo = melhorDelta;

        atualizarMatriz(s, melhor_i, melhor_j);
        return true;
    }
    return false;
}


void MLP::buscaLocal(Solucao& s) {
    std::vector<int> NL = {1};

    while(!NL.empty())
    {
        int n = rangeRandom(0, NL.size()-1);
        bool improved = false;

        switch(NL[n]) {
            case 1:
                improved = bestImprovementSwap(s);
                // if (improved)
                // {
                //     std::cout << "Swap: " << std::endl;
                //     printVector(s.caminho);
                // }
                // std::cout << "improved (1) = " << improved << std::endl;
                break;

            case 2:
                improved = bestImprovement2Opt(s);
                // if (improved)
                // {
                //     std::cout << "2Opt: " << std::endl;
                //     printVector(s.caminho);
                // }
                // std::cout << "improved (2) = " << improved << std::endl;
                break;

            case 3:
                improved = bestImprovementOrOpt(s, 1);
                // if (improved)
                // {
                //     std::cout << "OrOpt UM: " << std::endl;
                //     printVector(s.caminho);
                // }
                // std::cout << "improved (3) = " << improved << std::endl;
                break;

            case 4:
                improved = bestImprovementOrOpt(s, 2);
                // if (improved)
                // {
                //     std::cout << "OrOpt DOIS: " << std::endl;
                //     printVector(s.caminho);
                // }
                // std::cout << "improved (4) = " << improved << std::endl;
                break;

            case 5:
                improved = bestImprovementOrOpt(s, 3);      
                // if (improved)
                // {
                //     std::cout << "OrOpt TRES: " << std::endl;
                //     printVector(s.caminho);
                // }
                // std::cout << "improved (5) = " << improved << std::endl;
                break;
            
            default:
                std::cout << "Valor invalido! (Switch)" << std::endl;
                break;
        }


        if (improved)
        {
            NL = {1};
        }
        else
        {
            NL.erase(NL.begin() + n);
        }
    }
}


void MLP::calcularInsercoes(std::vector<int>& subtour, std::set<int>& CL, std::vector<InsertionInfo>& insercoes) {
    // std::cout << "START calcularInsercoes?" << std::endl;
    insercoes.resize(CL.size());
    int indiceInsercao = 0;
    int ultimoInserido = subtour.back();
    // std::cout << "START calcularInsercoes ok" << std::endl;

    for (int verticeCL : CL)
    {
        double custo = grafo.matriz[ultimoInserido][verticeCL];

        insercoes[indiceInsercao].noInserido = verticeCL;
        insercoes[indiceInsercao].custoInsercao = custo;
        indiceInsercao++;
    }
}


Solucao MLP::construcao() {
    Solucao s;
    s.caminho = {1}; // Inicializando o caminho da solução com o vértice 1

    // Conjunto dos vértices candidatos para inserção (inicialmente todos)
    std::set<int> CL;

    int qtdVertices = grafo.qtdVertices();
    for (int i = 2; i < qtdVertices; i++)
    {
        // Inserção do vértice.
        // Complexidade constante amortizada: https://cplusplus.com/reference/set/set/insert/
        CL.insert(CL.end(), i);
    }


    // A cada iteração um dos vértices de CL será inserido na solução
    while(!CL.empty())
    {
        // Vetor com todas as inserções possíveis
        std::vector<InsertionInfo> insercoes;
        calcularInsercoes(s.caminho, CL, insercoes);

        // Ordena o vetor de inserções em ordem crescente de custo de inserção
        std::sort(insercoes.begin(), insercoes.end());

        // Porcentagem das melhores inserções que serão consideradas para inserção
        double alpha = (double) rangeRandom(1, 25) / 100.0;

        // Valor mais alto de um índice que pode ser escolhido para inserção
        int limite = (CL.size() - 1) * alpha;

        // Insere o novo vértice na solucao
        InsertionInfo escolhido = insercoes[rangeRandom(0, limite)];

        // Como o nó agora faz parte da solução, ele deve ser incluído no caminho
        // e excluído de CL
        s.caminho.push_back(escolhido.noInserido);

        CL.erase(escolhido.noInserido);
    }

    // Inserindo o primeiro vértice no final do caminho, de forma
    // a criar uma sequência circular
    s.caminho.push_back(s.caminho.front());

    // Criação da matriz de subsequências e
    // definição do custo total do caminho construído
    criarMatrizCompleta(s);
    s.custoMinimo = matriz_subseq[0][s.caminho.size()-1].custoAcumulado;

    return s;
}


void MLP::criarMatrizCompleta(Solucao& s) {
    int qtdVertices = s.caminho.size();
    matriz_subseq = std::vector<std::vector<Subsequence>>(qtdVertices, std::vector<Subsequence>(qtdVertices));

    // Definindo as subsequências de tamanho 1
    for (int i = 0; i < qtdVertices; i++)
    {
        matriz_subseq[i][i].primeiro = s.caminho[i];
        matriz_subseq[i][i].ultimo = s.caminho[i];

        matriz_subseq[i][i].custoAtraso = (i > 0);
        matriz_subseq[i][i].duracao = 0;
        matriz_subseq[i][i].custoAcumulado = 0;
    }

    // Definindo as subsequências não invertidas
    for (int i = 0; i < qtdVertices; i++)
    {
        for (int j = i+1; j < qtdVertices; j++)
        {
            matriz_subseq[i][j] = Subsequence::concatenar(
                matriz_subseq[i][j-1],
                matriz_subseq[j][j],
                grafo
            );
        }
    }

    // Definindo as subsequências invertidas
    for (int i = qtdVertices-1; i >= 0; i--)
    {
        for (int j = i-1; j >= 0; j--)
        {
            matriz_subseq[i][j] = Subsequence::concatenar(
                matriz_subseq[i][j+1],
                matriz_subseq[j][j],
                grafo
            );
        }
    }
}


Solucao MLP::ILS(Grafo grafo, int maxIterILS, int maxIterBuscaLocal) {
    this->grafo = grafo;
    Solucao melhorSolucao;

    for (int i = 0; i < maxIterILS; i++)
    {
        Solucao s = construcao();
        Solucao melhorIter = s;
        // std::cout << "construcao: ";
        // printVector(s.caminho);

        int contador = 0;
        for (int j = 0; j < maxIterBuscaLocal; j++)
        {
            // std::cout << "Busca Local " << ++contador << " | " << j << std::endl;
            buscaLocal(s);
            if (s.custoMinimo < melhorIter.custoMinimo)
            {
                melhorIter = s;
                j = (j == 0 ? -1 : 0);
            }

            s = perturbacao(melhorIter);
            // std::cout << "Perturbacao:" << std::endl;
            // printVector(s.caminho);
        }


        if (melhorIter.custoMinimo < melhorSolucao.custoMinimo || melhorSolucao.custoMinimo == Solucao::_custoInvalido)
        {
            melhorSolucao = melhorIter;
        }
    }

    return melhorSolucao;
}


Solucao MLP::perturbacao(Solucao s) {
    // Tamanhos mínimo e máximo para os segmentos que serão trocados
    int tamanhoMinimo = 2;
    int tamanhoMaximo = ceil(s.caminho.size() / 10.0);

    // Definindo o tamanho e inicio dos segmentos
    int tamanho_1, inicio_1;
    int tamanho_2, inicio_2;
    do {

        tamanho_1 = rangeRandom(tamanhoMinimo, tamanhoMaximo);
        tamanho_2 = rangeRandom(tamanhoMinimo, tamanhoMaximo);

        inicio_1 = rangeRandom(1, s.caminho.size() - (1 + tamanho_1));
        inicio_2 = rangeRandom(1, s.caminho.size() - (1 + tamanho_2));

    } while(temIntersecao(inicio_1, inicio_1+tamanho_1-1, inicio_2, inicio_2+tamanho_2-1));

    // O segmento 1 deve ser o mais a esquerda para realizar a rotacao
    if (inicio_2 < inicio_1)
    {
        std::swap(inicio_1, inicio_2);
        std::swap(tamanho_1, tamanho_2);
    }


    // Recalculando o custo
    int seg1_first = s.caminho[inicio_1],
        seg1_prev = s.caminho[inicio_1 - 1],
        seg1_last = s.caminho[inicio_1 + (tamanho_1 - 1)],
        seg1_next = s.caminho[inicio_1 + tamanho_1];

    int seg2_first = s.caminho[inicio_2],
        seg2_prev = s.caminho[inicio_2 - 1],
        seg2_last = s.caminho[inicio_2 + (tamanho_2 - 1)],
        seg2_next = s.caminho[inicio_2 + tamanho_2];
    
    double delta;
    if ((tamanho_1 + tamanho_2) < ((inicio_2 + tamanho_2 - 1) - inicio_1 + 1))
    {
        delta = (
            // Retirando arestas
            - (grafo.matriz[seg1_first][seg1_prev] + grafo.matriz[seg1_last][seg1_next])
            - (grafo.matriz[seg2_first][seg2_prev] + grafo.matriz[seg2_last][seg2_next])

            // Inserindo as novas arestas
            + (grafo.matriz[seg1_prev][seg2_first] + grafo.matriz[seg2_last][seg1_next])
            + (grafo.matriz[seg2_prev][seg1_first] + grafo.matriz[seg1_last][seg2_next])
        );
    }
    else // (tamanho_1 + tamanho_2) == ((inicio_2 + tamanho_2 - 1) - inicio_1 + 1)
    {
        delta = (
            // Retirando arestas
            - (grafo.matriz[seg1_prev][seg1_first])
            - (grafo.matriz[seg1_last][seg2_first])
            - (grafo.matriz[seg2_last][seg2_next])

            // Inserindo as novas arestas
            + (grafo.matriz[seg1_prev][seg2_first])
            + (grafo.matriz[seg2_last][seg1_first])
            + (grafo.matriz[seg1_last][seg2_next])
        );
    }

    s.custoMinimo = s.custoMinimo + delta;


    // Realizando a troca dos segmentos
    std::rotate(
        s.caminho.begin() + inicio_1,
        s.caminho.begin() + inicio_2,
        s.caminho.begin() + inicio_2 + tamanho_2
    );

    if ((tamanho_1 + tamanho_2) < ((inicio_2 + tamanho_2 - 1) - inicio_1 + 1))
    {
        std::rotate(
            s.caminho.begin() + inicio_1 + tamanho_2,
            s.caminho.begin() + inicio_1 + tamanho_2 + tamanho_1,
            s.caminho.begin() + inicio_2 + tamanho_2
        );
    }


    // Atualizando a matriz de subsequências
    atualizarMatriz(s, inicio_1, inicio_1 + (tamanho_2 - 1));

    // std::cout << "inicio_1 = " << inicio_1 << "; tamanho_1 = " << tamanho_1 << std::endl
    //           << "inicio_2 = " << inicio_2 << "; tamanho_2 = " << tamanho_2 << std::endl;


    return s;
}