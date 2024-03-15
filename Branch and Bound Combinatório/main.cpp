#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <queue>
#include "data.h"
#include "hungarian.h"

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

		hungarian_problem_t p;
		int mode = HUNGARIAN_MODE_MINIMIZE_COST;
		hungarian_init(&p, cost, data->getDimension(), data->getDimension(), mode); // Carregando o problema

		double obj_value = hungarian_solve(&p);
		cout << "Obj. value: " << obj_value << endl;

		cout << "Assignment" << endl;
		hungarian_print_assignment(&p);

		hungarian_free(&p);
		for (int i = 0; i < data->getDimension(); i++) delete [] cost[i];
		delete [] cost;
		delete data;
	}

	return 0;
}
