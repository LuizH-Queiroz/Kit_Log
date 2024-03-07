#include "Data.h"
#include "grafo.h"
#include "tsp.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <ratio>
#include <string>
#include <cstring>

#define ARQUIVO_INSTANCIAS "src/instancias.txt"

using namespace std;


struct DadosInstancia {
    string nome;
    double tempoBenchmark;
    double custoBenchmark;
};


vector<DadosInstancia> lerArquivos(string nomeArquivo) {
    ifstream arquivoLeitura(nomeArquivo);
    vector<DadosInstancia> instancias;
    string nome;
    double tempo, custo;


    while(arquivoLeitura >> nome >> tempo >> custo)
    {
        // cout << nome << " " << tempo << " " << custo << endl;
        nome = "instances/" + nome;

        DadosInstancia dados = {nome, tempo, custo};
        instancias.push_back(dados);
    }

    return instancias;
}



int main() {
    srand(time(NULL));
    vector<DadosInstancia> arquivos = lerArquivos(string(ARQUIVO_INSTANCIAS));

    // cout << arquivos.size() << endl;
    for (DadosInstancia instancia : arquivos)
    {
        char *c_nome = new char[instancia.nome.size() + 1];
        strcpy(c_nome, instancia.nome.c_str());
 
        auto data = Data(2, c_nome);
        data.read();
        size_t n = data.getDimension();

        delete[] c_nome;

        // cout << endl;
        // cout << instancia.nome << endl;
        ///////////////////////////////

        Grafo grafo;
        grafo.matriz.resize(n+1, vector<double>(n+1));

        // cout << "MATRIZ" << endl;
        for (int i = 1; i <= n; i++)
        {
            for (int j = 1; j <= n; j++)
            {
                // cout << i << " " << j << endl;
                grafo.matriz[i][j] = data.getDistance(i, j);
            }
        }

        TSP tsp;
        int maxIterILS = 50;
        int maxIterBuscaLocal = (n >= 150 ? n/2 : n);


        double totalTime = 0;
        double totalCost = 0;
        for (int i = 0; i < 10; i++)
        {
            auto start = chrono::system_clock::now();

            // cout << "TSP" << endl;
            totalCost += tsp.ILS(grafo, maxIterILS, maxIterBuscaLocal).custoMinimo;

            auto end = chrono::system_clock::now();

            chrono::duration<double> elapsedTime = end - start;
            totalTime += elapsedTime.count();
        }



        // cout << totalCost << endl;
        // cout << fixed << setprecision(3) << (totalTime / 10.0) << endl;

        // Inserindo dados no arquivo de saida
        ofstream arquivoSaida("resultados.txt", ios_base::app);


        arquivoSaida << instancia.nome;
        for (int i = instancia.nome.size(); i <= 25; i++)
        {
            arquivoSaida << " ";
        }
        arquivoSaida << "\t\t";

        double tempoMedio = totalTime / 10.0;
        arquivoSaida << fixed << setprecision(3) << "\t\t" << instancia.tempoBenchmark << "\t\t" << tempoMedio;
        arquivoSaida << "\t\t" << (tempoMedio - instancia.tempoBenchmark);

        double custoMedio = custoMedio = totalCost / 10.0;
        arquivoSaida << fixed << setprecision(1) << "\t\t" << instancia.custoBenchmark << "\t\t" << custoMedio;
        arquivoSaida << "\t\t" << (custoMedio - instancia.custoBenchmark);

        arquivoSaida << endl;


        arquivoSaida.close();
    }



    return 0;
}
