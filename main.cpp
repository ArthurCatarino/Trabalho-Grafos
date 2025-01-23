#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <queue>
#include <stack>

using namespace std;

typedef pair <double, double>* pares;

double pitagoras(const double &x1, const double& x2, const double& y1, const double& y2) {

	double x = x2 - x1;
	double y = y1 - y2;
	double hip = x * x + y * y;
	return sqrt(hip);
}

void calcula_distancias(const pares &cord, double** &matriz, const int &n_vertices, double &low, double &high) {
	// preenche a matriz de adjacencia com o peso dos vertices;
	for (int i = 0; i < n_vertices; i++) {
		for (int j = 0; j < n_vertices; j++) {
			if (i == j) {
				matriz[i][j] = 0;
			}
			else {
				matriz[i][j] = pitagoras(cord[i].first,cord[j].first,cord[i].second,cord[j].second);
				if (matriz[i][j] > high) {
					high = matriz[i][j];
				}
				if (matriz[i][j] < low and matriz[i][j] != 0) {
					low = matriz[i][j];
				}
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

double vizinho_proximo(double** &matriz,const int &origem,const int &tamanho) {
	 
	double distancia_total = 0;

	vector<bool> visitados(tamanho, false);
	vector<int> ordem;
	
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
		distancia_total += distancia;
		atual = noh;
	}

	ordem.push_back(origem);
	distancia_total += matriz[atual][origem];

	return distancia_total;
}

double insercao(double**& matriz,const int& origem, int tamanho) {

	vector<bool> visitado(tamanho, false);
	vector<int> rota;

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
		distancia += menor_impacto;
		visitado[proximo_no] = true;
	}

	return distancia;
}

bool dfs(double** &matriz, int tamanho, double filtro) {

	vector<bool> visitado(tamanho, false);
	stack<int> pilha;
	pilha.push(0);
	visitado[0] = true;

	int atual;
	int contador = 0;

	while (!pilha.empty()) {
		atual = pilha.top();
		pilha.pop();
		contador++;

		for (int i = 0; i < tamanho; i++) {
			if (matriz[atual][i] < filtro and matriz[atual][i] > 0 and !visitado[i]) {
				pilha.push(i);
				visitado[i] = true;
			}
		}
	}

	return tamanho == contador;

}

double 	encontra_menor_distancia_maxima(double** & matriz,double low,double high,int tamanho) {

	double epsilon = 1e-6; // Precisão da busca binária
	double mid;
	int contador = 0;

	while (contador < 100 and high - low > epsilon) {
		contador++;
		mid = (high + low) / 2;
		cout << "HIGH: " << high << " LOW: " << low << " MID: " << mid << endl;

		if (dfs(matriz, 5, mid)) {
			high = mid;
		}
		else {
			low = mid;
		}
	}
	return mid;
}


int main() {

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


	double low = DBL_MAX;
	double high = -1 ;
	
	calcula_distancias(cord, matriz, n_vertices,low,high);
	delete[] cord;
	double menor = encontra_menor_distancia_maxima(matriz, low, high, n_vertices);
	
	//cout << endl << "O menor caminho com a origem : " << vizinho_proximo(matriz, 0, n_vertices) << endl;
	//cout << endl << "O menor caminho com a origem : " << insercao(matriz, 0, n_vertices) << endl;
	
	
	return 0;
}
