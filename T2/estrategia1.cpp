#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

using namespace std;
#define DISPONIBLE 1
#define NO_DISPONIBLE 0

int progreso = 0;
int ganancia_maxima = 0;
vector<vector<int>> items;
int N;
chrono::steady_clock::time_point inicioTemporizador;
int peso_maximo;
double demora = 0;

string read_file(string filename) {
    ifstream input_file(filename, ios::binary);
    string retorno = "";
    if (input_file.is_open()) {
        string line;
        while (getline(input_file, line)) {
            retorno += line + '\n';
        }
        input_file.close();
    } else {
        cout << "No se pudo abrir el archivo: " << filename << endl;
    }
    return retorno;
}

vector<string> split(const string& row, char delimiter) {
    vector<string> datos;
    stringstream ss(row);
    string item;
    while (getline(ss, item, delimiter)) {
        datos.push_back(item);
    }
    return datos;
}

double K;

void reinicio_items () {
    for (int i = 0; i < N; i++) {
        items[i][2] = DISPONIBLE;
    }
}

pair<int, int> agregar_item(int ganancia, int peso, int N, int peso_maximo) {
    pair<int, int> value;
    //Rúbrica ID 2
    int rand_item = rand() % N;
    if (items[rand_item][2] == DISPONIBLE) {
        value.first = items[rand_item][0];
        value.second = items[rand_item][1];
        if (peso + value.second > peso_maximo) {
            reinicio_items();
            return make_pair(0, 0);
        }
        ganancia += value.first;
        peso += value.second;
        items[rand_item][2] = NO_DISPONIBLE;

    }
    //Rúbrica ID 3 (Reporta nueva ganancia)
    if (ganancia > ganancia_maxima) {
        cout << "Mejor ganancia encontrada: " << ganancia << endl;
        cout << "+" << ganancia - ganancia_maxima << " de beneficio" << endl;
        ganancia_maxima = ganancia;
        demora = (double)chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - inicioTemporizador).count()/1000.0;
    }
    return make_pair(ganancia, peso);
}

//Rúbrica ID 1 (Toda la función de inicialización)
void inicializacion() {
    string datos = read_file("mochila.txt");
    istringstream iss(datos);
    string line;
    getline(iss, line);
    vector<string> valores = split(line, ' ');
    if (valores.size() != 2) {
        cout << "Error: formato de línea incorrecto" << endl;
        return;
    }
    N = stoi(valores[0]);
    peso_maximo = stoi(valores[1]);

    items.resize(N, vector<int>(2));

    for (int i = 0; i < N; i++) {
        getline(iss, line);
        valores = split(line, ' ');
        if (valores.size() != 2) {
            cout << "Error: formato de línea incorrecto" << endl;
            return;
        }
        items[i][0] = stoi(valores[0]);
        items[i][1] = stoi(valores[1]);
        items[i][2] = DISPONIBLE;
    }
}

int main() {
    srand(time(0));

    inicializacion();
    
    cout<<"Ingrese la cantidad de segundos en los que se llamará a la función (puede tener decimales): ";
    cin>>K;

    inicioTemporizador = chrono::steady_clock::now();

    cout<<"INICIO"<<endl;
    
    int ganancia = 0;
    int peso = 0;

    auto tiempoInicio = chrono::steady_clock::now();
    //Rúbrica ID 3 (se llama por K segundos)
    while (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - tiempoInicio).count() < K * 1000) {
        pair<int, int> par = agregar_item(ganancia, peso, N, peso_maximo);
        ganancia = par.first;
        peso = par.second;
    }

    cout<<"FIN a los "<<(double)chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - inicioTemporizador).count()/1000.0<<" segundos."<<endl;

    cout<<"La mejor ganancia encontrada es "<< ganancia_maxima<<" a los "<<demora<<" segundos."<<endl;

    return 0;
}
