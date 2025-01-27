#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <queue>
#include <algorithm>
#include <chrono>


struct resposta {
	double distancia;
	double maior_aresta;

	resposta() {
		distancia = 0;
		maior_aresta = -1;
	}
};

using namespace std;

typedef pair <double, double>* pares;

double calcula_maior_aresta(double** matriz, vector<int> rota) {

	double maior_aresta = -1;
	int n = rota.size();
	for (int i = 1; i < n; i++) {
		if (matriz[rota[i - 1]][rota[i]] > maior_aresta) {
			maior_aresta = matriz[rota[i - 1]][rota[i]];
		}
	}
	return maior_aresta;
}

vector<int> OPT(double** matriz, vector<int> rota, int tamanho) {
	
	bool melhorou = true;
	double valor_antigo, valor_novo;
	valor_antigo = calcula_maior_aresta(matriz, rota);

	while (melhorou) {

		melhorou = false;

		for (int i = 1; i < tamanho - 1; i++) {
			for (int j = i + 1;j < tamanho; j++) {
				vector<int> nova_rota = rota;
				reverse(nova_rota.begin() + i, nova_rota.begin() + j + 1);
				valor_novo = calcula_maior_aresta(matriz, nova_rota);

				if (valor_novo < valor_antigo) {
					rota = nova_rota;
					valor_antigo = valor_novo;
					melhorou = true;
					cout << "Melhorou pra :" << valor_novo << endl;
				}
			}
		}
	}
	cout << "Apos aplica a OPT a maior aresta ficou com o valor : " << valor_antigo << endl;
	return rota;
}

double pitagoras(const double &x1, const double& x2, const double& y1, const double& y2) {

	double x = x2 - x1;
	double y = y1 - y2;
	double hip = x * x + y * y;
	return sqrt(hip);
}

void calcula_distancias(const pares &cord, double** &matriz, const int &n_vertices) {
	// preenche a matriz de adjacencia com o peso dos vertices;
	for (int i = 0; i < n_vertices; i++) {
		for (int j = 0; j < n_vertices; j++) {
			if (i == j) {
				matriz[i][j] = 0;
			}
			else {
				matriz[i][j] = pitagoras(cord[i].first,cord[j].first,cord[i].second,cord[j].second);
			}
		}
	}
}

void imprimi(double**& matriz) {
	for (int i = 0; i < 5;i++) {
		for (int j = 0; j < 5; j++) {
			cout << matriz[i][j] << " ";
		}
		cout << endl;
	}
}

resposta vizinho_proximo(double** &matriz,const int &origem,const int &tamanho) {
	 
	double distancia_total = 0;

	vector<bool> visitados(tamanho, false);
	vector<int> ordem;
	
	resposta dados;

	ordem.push_back(origem);
	visitados[origem] = true;
	int atual = origem;
	double distancia;
	int noh;

	for (int i = 0; i < tamanho-1; i++) {
		noh = -1;
		distancia = DBL_MAX;

		for (int j = 0; j < tamanho; j++) {
			if ((!visitados[j]) and (matriz[atual][j] < distancia)) {
				distancia = matriz[atual][j];
				noh = j;
			}
		}
		ordem.push_back(noh);
		visitados[noh] = true;
		dados.distancia += distancia;
		atual = noh;

		if (distancia > dados.maior_aresta) {
			dados.maior_aresta = distancia;
		}
	}

	ordem.push_back(origem);
	dados.distancia += matriz[atual][origem];

	return dados;
}

resposta insercao(double**& matriz,const int& origem, int tamanho) {

	vector<bool> visitado(tamanho, false);
	vector<int> rota;
	resposta resultado;

	visitado[origem] = true;
	rota.push_back(origem);

	double maior_dist = -1;
	int maior;

	for (int i = 0; i < tamanho; i++) {
		if (matriz[origem][i] > maior_dist) {
			maior_dist = matriz[origem][i];
			maior = i;
		}
	}

	
	double distancia = maior_dist * 2; //Ida e volta   0 - 1 - 0
	visitado[maior] = true;
	rota.push_back(maior);
	rota.push_back(origem);

	double menor_dist;
	int proximo_no;

	while (rota.size() < tamanho + 1) {
		for (int i = 0; i < tamanho; i++) {

			//Encontra o no com a maior menor distancia dos nos da rota 
			if (!visitado[i]) {
				menor_dist = DBL_MAX;
				maior_dist = -1;
				for (int j = 0; j < rota.size(); j++) {
					if (matriz[i][rota[j]] < menor_dist) {
						menor_dist = matriz[i][rota[j]];
					}
				}
				if (maior_dist < menor_dist) {
					maior_dist = menor_dist;
					proximo_no = i;
				}
			}
		}


		//Verifica o local de menor impacto para adicionar o novo no

		double menor_impacto = DBL_MAX;
		int primeiro_no;

		for (int k = 0; k < rota.size() - 1; k++) {
			int l = k + 1;

			double impacto = matriz[rota[k]][proximo_no] + matriz[proximo_no][rota[l]] - matriz[rota[k]][rota[l]];
			if (menor_impacto > impacto) {
				primeiro_no = k;
				menor_impacto = impacto;
			}
		}
		rota.insert(rota.begin() + primeiro_no + 1, proximo_no);
		resultado.distancia += menor_impacto;
		visitado[proximo_no] = true;
	}

	// Procura a maior aresta na rota definida

	resultado.maior_aresta = calcula_maior_aresta(matriz, rota);

	cout << "Usando a inserção  temos como caminho minimo : " << resultado.distancia << " e com a maior aresta sendo de peso : " << resultado.maior_aresta << endl;

	OPT(matriz, rota,tamanho);
	
	return resultado;
}


int main() {

	auto start = chrono::high_resolution_clock::now();

	ifstream arquivo("C:\\Users\\arthu\\OneDrive\\Área de Trabalho\\texto.txt");
	string info;
	int n_vertices;
		
	if (!arquivo.is_open()) {
		cerr << "Erro ao abrir o arquivo." << endl;
		return 1;
	}
	
	getline(arquivo, info);
	arquivo >> info;
	arquivo >> info;
	arquivo >> n_vertices;

	arquivo.ignore();
	getline(arquivo, info);
	arquivo.ignore();
	getline(arquivo, info);

	int lixo;
	
	pares cord = new pair<double, double>[n_vertices]; // cria um vetor com as coordenadas dos pontos;
	double** matriz = new double* [n_vertices];

	for (int i = 0; i < n_vertices; i++) {
		matriz[i] = new double[n_vertices];
		arquivo >> lixo;
		arquivo >> cord[i].first >> cord[i].second; // preenche o vetor com as cordenadas dos pontos
	//cout << cord[i].first << " " << cord[i].second << endl;
	} // aloca a matriz dinamicamente
	
	calcula_distancias(cord, matriz, n_vertices);
	delete[] cord;
	
	resposta resultado = vizinho_proximo(matriz, 0, n_vertices);
	cout << "Usando o vizinho mais proximo temos como caminho minimo : " << resultado.distancia << " e com a maior aresta sendo de peso : " << resultado.maior_aresta << endl;
	resposta resultado2 = insercao(matriz, 0, n_vertices);

	
	auto end = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
	cout << "O código demorou " << duration.count() << " ms para rodar." << endl;

	return 0;
}
