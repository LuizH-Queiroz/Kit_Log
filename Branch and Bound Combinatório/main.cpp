#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <queue>
#include <chrono>
#include "data.h"
#include "hungarian.h"
#include "b&b.h"

#define ARQUIVO_INSTANCIAS "instanciasB&B.txt"

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

	for (DadosInstancia instancia : arquivos)
	{
        char *c_nome = new char[instancia.nome.size() + 1];
        strcpy(c_nome, instancia.nome.c_str());

		Data * data = new Data(2, c_nome);
		data->readData();

		delete[] c_nome;
		//////////////////////////////////////////////


		double **cost = new double*[data->getDimension()];
		for (int i = 0; i < data->getDimension(); i++){
			cost[i] = new double[data->getDimension()];
			for (int j = 0; j < data->getDimension(); j++){
				cost[i][j] = data->getDistance(i,j);
			}
		}

		// hungarian_problem_t p;
		// int mode = HUNGARIAN_MODE_MINIMIZE_COST;
		// hungarian_init(&p, cost, data->getDimension(), data->getDimension(), mode); // Carregando o problema

		// double obj_value = hungarian_solve(&p);
		// cout << "Obj. value: " << obj_value << endl;

		// cout << "Assignment" << endl;
		// hungarian_print_assignment(&p);

		// hungarian_free(&p);
		// continue;

		BB_TSP bb;
		chrono::_V2::system_clock::time_point start;
		chrono::_V2::system_clock::time_point end;
		chrono::duration<double> timeDFS, timeBFS, timeMLB;

		cout << endl;
		cout << instancia.nome << endl;

		start = chrono::system_clock::now();
		cout << "DFS: " << bb.solveTSP(cost, data->getDimension(), BB_TSP::BRANCHING_STRATEGY::DFS);
		end = chrono::system_clock::now();
		timeDFS = end - start;
		cout << " " << timeDFS.count() << endl;

		start = chrono::system_clock::now();
		cout << "BFS: " << bb.solveTSP(cost, data->getDimension(), BB_TSP::BRANCHING_STRATEGY::BFS);
		end = chrono::system_clock::now();
		timeBFS = end - start;
		cout << " " << timeBFS.count() << endl;

		start = chrono::system_clock::now();
		cout << "Minimum LB: " << bb.solveTSP(cost, data->getDimension(), BB_TSP::BRANCHING_STRATEGY::MINIMUM_LOWER_BOUND);
		end = chrono::system_clock::now();
		timeMLB = end - start;
		cout << " " << timeMLB.count() << endl;

		for (int i = 0; i < data->getDimension(); i++) delete [] cost[i];
		delete [] cost;
		delete data;
		////////////////////////////////////////////////////////
		ofstream arquivoSaida("resultados.txt", ios_base::app);

		arquivoSaida << fixed << setprecision(3);
		arquivoSaida << instancia.nome << " " << timeDFS.count() << " " << timeBFS.count() << " " << timeMLB.count() << endl;
	}

	return 0;
}
