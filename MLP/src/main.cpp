#include "Data.h"
#include "grafo.h"
#include "mlp.h"
#include "subsequence.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <ratio>
#include <string>
#include <cstring>
#include <unistd.h>

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

        // cout << endl << endl << endl;
        // cout << instancia.nome << endl;
        // cout << endl << endl << endl;
        ///////////////////////////////

        Grafo grafo;
        grafo.matriz.resize(n+1, vector<double>(n+1));

        // cout << "MATRIZ" << endl;
        for (int i = 1; i <= n; i++)
        {
            for (int j = 1; j <= n; j++)
            {
                grafo.matriz[i][j] = data.getDistance(i, j);
            }
        }

        MLP mlp;
        int maxIterILS = 10;
        int maxIterBuscaLocal = min(100, (int) n);


        double totalTime = 0;
        double totalCost = 0;
        for (int i = 0; i < 10; i++)
        {
            auto start = chrono::system_clock::now();

            totalCost += mlp.ILS(grafo, maxIterILS, maxIterBuscaLocal).custoMinimo;

            auto end = chrono::system_clock::now();

            chrono::duration<double> elapsedTime = end - start;
            totalTime += elapsedTime.count();
        }



        // Inserindo dados no arquivo de saida
        ofstream arquivoSaida("resultados.txt", ios_base::app);


        arquivoSaida << instancia.nome;
        for (int i = instancia.nome.size(); i <= 20; i++)
        {
            arquivoSaida << " ";
        }

        double tempoMedio = totalTime / 10.0;
        arquivoSaida << fixed << setprecision(2) << "\t\t" << tempoMedio;

        double custoMedio = totalCost / 10.0;
        arquivoSaida << fixed << setprecision(2) << "\t\t" << custoMedio;

        arquivoSaida << endl;


        arquivoSaida.close();
    }



    return 0;
}
