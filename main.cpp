#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <vector>
#include <cmath>
#include <chrono>

using namespace std;

typedef pair <double, double>* pares;
const double EARTH_RADIUS_KM = 6371.0;
const double PI = 3.14;

struct resposta {
    double distancia;
    double maior_aresta;

    resposta() {
        distancia = 0;
        maior_aresta = -1;
    }
};

double calcula_maior_aresta(double**& matriz, vector<int>& rota) {

    double maior_aresta = -1;
    int n = rota.size();
    for (int i = 1; i < n; i++) {
        if (matriz[rota[i - 1]][rota[i]] > maior_aresta) {
            maior_aresta = matriz[rota[i - 1]][rota[i]];
        }
    }
    return maior_aresta;
}

void imprimi(double**& matriz, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        for (int j = 0; j < tamanho; j++) {
            cout << matriz[i][j] << " ";
        }
        cout << endl;
    }
}

double toRadians(double degree) {
    return degree * (PI / 180.0);
}

int extractNumber(const std::string& str) {
    std::string numberString = "";

    for (char c : str) {
        if (std::isdigit(c)) { // Verifica se o caractere é um dígito
            numberString += c; // Adiciona o dígito à string temporária
        }
        else if (!numberString.empty()) {
            // Para a extração se já temos um número formado (caso haja números separados por texto)
            break;
        }
    }

    // Converte a string do número para int, se possível
    return numberString.empty() ? 0 : std::stoi(numberString);
}

std::string getAfterColon(const std::string& input) {
    // Encontra a posição do caractere ':'
    size_t colonPos = input.find(':');

    if (colonPos != std::string::npos) { // Se os ':' forem encontrados
        // Retorna o substring após o ':', ignorando espaços em branco
        return input.substr(colonPos + 1); // Pega tudo após ':'
    }
}

double pitagoras(const double& x1, const double& x2, const double& y1, const double& y2) {

    double x = x2 - x1;
    double y = y1 - y2;
    double hip = x * x + y * y;
    return sqrt(hip);
}

void calcula_distancias(const pares& cord, double**& matriz, const int& n_vertices) {
    // preenche a matriz de adjacencia com o peso dos vertices;
    for (int i = 0; i < n_vertices; i++) {
        for (int j = 0; j < n_vertices; j++) {
            if (i == j) {
                matriz[i][j] = 0;
            }
            else {
                matriz[i][j] = pitagoras(cord[i].first, cord[j].first, cord[i].second, cord[j].second);
            }
        }
    }
}

double haversineDistance(double lat1, double lon1, double lat2, double lon2) {
    // Converte as coordenadas para radianos
    lat1 = toRadians(lat1);
    lon1 = toRadians(lon1);
    lat2 = toRadians(lat2);
    lon2 = toRadians(lon2);

    // Calcula a diferença das coordenadas
    double deltaLat = lat2 - lat1;
    double deltaLon = lon2 - lon1;

    // Aplica a fórmula de Haversine
    double a = pow(sin(deltaLat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(deltaLon / 2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    // Calcula a distância final
    return EARTH_RADIUS_KM * c;
}

void calcula_distancias_geografica(const pares& cord, double**& matriz, const int& n_vertices) {
    // preenche a matriz de adjacencia com o peso dos vertices;
    for (int i = 0; i < n_vertices; i++) {
        for (int j = 0; j < n_vertices; j++) {
            if (i == j) {
                matriz[i][j] = 0;
            }
            else {
                matriz[i][j] = haversineDistance(cord[i].first, cord[i].second, cord[j].first, cord[j].second);
            }
        }
    }
}

resposta vizinho_proximo(double**& matriz, const int& origem, const int& tamanho) {

    double distancia_total = 0;

    vector<bool> visitados(tamanho, false);
    vector<int> ordem;

    resposta dados;

    ordem.push_back(origem);
    visitados[origem] = true;
    int atual = origem;
    double distancia;
    int noh;

    for (int i = 0; i < tamanho - 1; i++) {
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

void OPT(double**& matriz, vector<int>& rota, int tamanho) {

    bool melhorou = true;
    double valor_antigo, valor_novo;
    valor_antigo = calcula_maior_aresta(matriz, rota);

    while (melhorou) {

        melhorou = false;

        for (int i = 1; i < tamanho - 1; i++) {
            for (int j = i + 1; j < tamanho; j++) {
                vector<int> nova_rota = rota;
                reverse(nova_rota.begin() + i, nova_rota.begin() + j + 1);
                valor_novo = calcula_maior_aresta(matriz, nova_rota);

                if (valor_novo < valor_antigo) {
                    rota = nova_rota;
                    valor_antigo = valor_novo;
                    melhorou = true;
                    //cout << "Melhorou pra :" << valor_novo << endl;
                }
            }
        }
    }
    cout << "Apos aplicacao algoritmo a maior aresta foi com o valor : " << valor_antigo << endl;
    cout << "A rota final encontrada foi: ";
    for (int i : rota) {
        cout << "v_" << i << " ";
    }
    cout << endl;

    ofstream arquivo_saida("rota_final.txt"); // Cria um arquivo para salvar a rota

    if (arquivo_saida.is_open()) {
        arquivo_saida << "Rota final encontrada:\n";
        for (int i : rota) {
            arquivo_saida << "v_" << i << " ";
        }
        arquivo_saida << endl;
        arquivo_saida.close();
        cout << "A rota foi salva no arquivo 'rota_final.txt'." << endl;
    }
    else {
        cerr << "Erro ao criar o arquivo para salvar a rota." << endl;
    }
}

resposta insercao(double**& matriz, const int& origem, int tamanho) {

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

    OPT(matriz, rota, tamanho);

    return resultado;
}

int main() {

    string arquivo;
    int n_vertices;


    do {
        auto start = chrono::high_resolution_clock::now();

        bool escolha;
        cout << "Deseja ler os dados de um arquivo? (1 - Sim, 0 - Nao): ";
        cin >> escolha;

        if (!escolha) {

            string tipoStr;
            cout << "Digite o tipo de coordenada (GEO ou EUC_2D): ";
            cin >> tipoStr;

            cout << "Digite o numero de vertices: ";
            cin >> n_vertices;

            pares cord = new pair<double, double>[n_vertices]; // cria um vetor com as coordenadas dos pontos;
            double** matriz = new double* [n_vertices];

            cout << "Digite as coordenadas (x y) de cada vértice:" << endl;
            for (int i = 0; i < n_vertices; i++) {
                matriz[i] = new double[n_vertices];
                cin >> cord[i].first >> cord[i].second; // preenche o vetor com as cordenadas dos pontos
            } // aloca a matriz dinamicamente

            calcula_distancias(cord, matriz, n_vertices);
            delete[] cord;
            resposta resultado = insercao(matriz, 0, n_vertices);
        }

        else {

            cout << "Digite o caminho do arquivo : "; //Nao suporta arquivos com caracteres especiais
            cin.ignore();
            getline(cin, arquivo);
            ifstream file(arquivo);

            if (!file.is_open()) { // Verifica se o arquivo foi aberto com sucesso
                cerr << "Erro ao abrir o arquivo. Verifique o caminho e tente novamente." << endl;
                return 1;
            }

            string temp;
            getline(file, temp);
            getline(file, temp);
            n_vertices = extractNumber(temp);
            getline(file, temp);
            string tipo = getAfterColon(temp);
            getline(file, temp);

            if (tipo == " GEO") {
                getline(file, temp);
            }


            int lixo;
            pares cord = new pair<double, double>[n_vertices]; // cria um vetor com as coordenadas dos pontos;
            double** matriz = new double* [n_vertices];

            for (int i = 0; i < n_vertices; i++) {
                matriz[i] = new double[n_vertices];
                file >> lixo;
                file >> cord[i].first >> cord[i].second; // preenche o vetor com as cordenadas dos pontos
                //cout << cord[i].first << " " << cord[i].second << endl;
            } // aloca a matriz dinamicamente

            if (tipo == " EUC_2D") {
                calcula_distancias(cord, matriz, n_vertices);
            }
            else if (tipo == " GEO") {
                calcula_distancias_geografica(cord, matriz, n_vertices);
            }
            else {
                cerr << "Tipo incompativel";
            }


            delete[] cord;

            resposta resultado = vizinho_proximo(matriz, 0, n_vertices);
            cout << "Usando o vizinho mais proximo temos como caminho minimo : " << resultado.distancia << " e com a maior aresta sendo de peso : " << resultado.maior_aresta << endl;
            resposta resultado2 = insercao(matriz, 0, n_vertices);

            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
            cout << "O código demorou " << duration.count() << " ms para rodar." << endl;

            // Fecha o arquivo
            file.close();
            cout << "Arquivo fechado com sucesso." << endl << endl;
            cout << " -------------------------------------------- " << endl;
        }


    } while (true);

    return 0;
}
