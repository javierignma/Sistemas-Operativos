#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <pthread.h>
#include <mutex>

using namespace std;
#define DISPONIBLE 1
#define NO_DISPONIBLE 0

mutex mtx;
int progreso = 0;
int ganancia_maxima = 0;
vector<vector<int>> items;
int N;
int peso_maximo;
chrono::steady_clock::time_point inicioTemporizador;
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

void reinicio_items (vector<int> &mochila) {
    while (!mochila.empty()) {
        items[mochila.back()][2] = DISPONIBLE;
        mochila.pop_back(); 
    }
}

//Rúbrica ID 5
pair<int, int> agregar_item(vector<int> &mochila ,int ganancia, int peso, int N, int peso_maximo, int thread_id) {
    //cout<<"Entré a la función..."<<endl;
    pair<int, int> value;
    //cout<<"Activé el mutex..."<<endl;
    int rand_item = rand() % N;
    //Rúbrica ID 6 (Sincroniza)
    mtx.lock();
    if (items[rand_item][2] == DISPONIBLE) {
        value.first = items[rand_item][0];
        value.second = items[rand_item][1];
        if (peso + value.second > peso_maximo) {
            reinicio_items(mochila);
            mtx.unlock();
            return make_pair(0, 0);
        }
        ganancia += value.first;
        peso += value.second;
        items[rand_item][2] = NO_DISPONIBLE;
        mochila.push_back(rand_item);

    }
    //Rúbrica ID 6 (Se registra la mejor ganancia)
    if (ganancia > ganancia_maxima) {
        cout << "Thread "<<thread_id<<" -> Mejor ganancia encontrada: " << ganancia << endl;
        cout << "+" << ganancia - ganancia_maxima << " de beneficio" << endl;
        ganancia_maxima = ganancia;
        demora = (double)chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - inicioTemporizador).count()/1000.0;
    }
    mtx.unlock();
    return make_pair(ganancia, peso);
}

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

int n_threads;

void* thread_func(void* arg) {
    int thread_id = *(static_cast<int*>(arg));
    vector<int> mochila;
    int ganancia = 0;
    int peso = 0;

    auto tiempoInicio = chrono::steady_clock::now();
    while (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - tiempoInicio).count() < K * 1000) {
        //Rúbrica ID 3 (Se llama a la función)
        pair<int, int> par = agregar_item(mochila, ganancia, peso, N, peso_maximo, thread_id);
        ganancia = par.first;
        peso = par.second;
    }
    
    pthread_exit(NULL);
}

int main() {
    srand(time(0));

    inicializacion();

    //Rúbrica ID 6 (K y N son modificables por el usuario)
    cout<<"Ingrese la cantidad de segundos en los que se llamará a la función (puede tener decimales): ";
    cin>>K;
    cout<<"Ingrese el número de threads que llamarán a la función: "<<endl;
    cin>>n_threads;

    inicioTemporizador = chrono::steady_clock::now();

    //Rúbrica ID 4 (creación de threads)
    pthread_t threads[n_threads];
    int thread_ids[N];
    cout<<"INICIO"<<endl;
    
    for (int i = 0; i < n_threads; i++) {
        thread_ids[i] = i;
        if(pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]) != 0) {
            cout<<"Error al crear el hilo..."<<endl;
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < n_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            cerr << "Error al unirse al hilo " << i << endl;
            exit(EXIT_FAILURE);
        }
    }

    cout<<"FIN a los "<<(double)chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - inicioTemporizador).count()/1000.0<<" segundos."<<endl;

    cout<<"La mejor ganancia encontrada es "<< ganancia_maxima<<" a los "<<demora<<" segundos."<<endl;

    return 0;
}
