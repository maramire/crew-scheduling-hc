#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <tuple>
#include <ctime>
#include <random>
#include <numeric>
#include <set>
#include <chrono>


const int NUM_RESTARTS = 5;
const int MAX_GENERATED_PAIRINGS = 100;
const int MAX_FLIGHT_TIME = 8*60;
const int MAX_PAIRING_TIME = 12*60;
const float MIN_REST_TIME = 0.5*60;
const int MAX_REST_TIME = 4*60;
const int FLIGHT_COST = 1;
const float WAIT_COST = 0.75;


using namespace std;
using namespace std::chrono;

void read_legs();
void generate_pairings();

//estructura para los vuelos
struct leg{ 

    int flight_id;
    string origin;
    string destination;
    int departure_time;
    int arrival_time;

};


//funcion que calcula un intervalo de tiempo dados dos tiempos.
int difftime(int time_1, int time_2) 
{

    int suma;
	int hr_1 = time_1 / 100;
	int hr_2 = time_2 / 100;


	hr_2 = (hr_1 > hr_2) ? (hr_2 + 24) : hr_2;

	int min_1 = time_1 % 100;
	int min_2 = time_2 % 100;


	int diffmin, diffhr;

	if (min_2 > min_1)
	{
		diffmin = min_2 - min_1;
	}
	else
	{
		diffmin = (min_2 + 60) - min_1;
		hr_2--;

	}

	diffhr = hr_2 - hr_1;

    suma = diffhr*60 + diffmin;
    return suma;

}

//retorna veradero si hora2 le sigue cronologicamente a hora1
bool isGreater(int hora1, int hora2){ 
    if (hora1>1700 && hora2<200){
        return true;
    }
    int suma = hora2 - hora1;
    if (suma<0){
        return false;
    } else{
        return true;
    }

}

//ordena un vector estructura de acuerdo a ul tiempo
bool compareByLength(const leg &a, const leg &b)  
{
    if (a.departure_time == b.departure_time){
        return a.departure_time < b.departure_time;
    }else{
        return a.departure_time < b.departure_time;
    }
}


//leer el archivo de instancias, y guarda  lo esencial
void read_legs(string file_path, int n_legs, vector<leg> &vuelos){ 

    string time;
    vector<leg> temp;

    ifstream file(file_path);
    if (!file.is_open())
    {
        exit(EXIT_FAILURE);
    }
    string str;
    getline(file, str);

    while (getline(file, str))
    {
        leg l;
        istringstream iss(str);
        string token;

        getline(iss, token, ',');
        l.flight_id = stoi(token);

        getline(iss, l.origin, ',');

        getline(iss, l.destination, ',');

        getline(iss, token, ',');
        replace(token.begin(), token.end(), ':', ' ');

        vector<string> array1;
        stringstream ss(token);
        string temp;
        while (ss >> temp)
            array1.push_back(temp);

        string fin = array1[0].append(array1[1]);
        l.departure_time = stoi(fin);

        getline(iss, token, ',');
        replace(token.begin(), token.end(), ':',' ');

        vector<string> array2;
        stringstream ss2(token);
        string temp2;
        while (ss2 >> temp2)
            array2.push_back(temp2);

        string fin2 = array2[0].append(array2[1]);
        l.arrival_time = stoi(fin2);

        vuelos.push_back(l);
    }

    file.close();
}

//elige una base random desde los vuelos
int random_base (vector<leg> &vuelos, int n_legs){ 

    random_device rd;
    default_random_engine engine{rd()};
    shuffle(begin(vuelos), end(vuelos), engine);


    vector<int> flights_from_base;

    for (int i=0; i<vuelos.size();i++){
        if (vuelos[i].origin=="IST" || vuelos[i].origin=="ANK"){
            return i;
        }
    }

}

// Funcion que elige el siguiente vuelo del pairing
vector<leg> searchNext(vector<leg> &vuelos_sorted,int base_index, string destination_previous, vector<int> &times, vector<int> candidato){
    
    bool found = false;
    vector<leg> sol(1);

    for (int i=base_index; i<vuelos_sorted.size(); i++){

        int new_flight_time = difftime(vuelos_sorted[i].departure_time,vuelos_sorted[i].arrival_time);


        int new_rest_time = difftime(times[3],vuelos_sorted[i].departure_time);


        if ((destination_previous== vuelos_sorted[i].origin) && (new_rest_time >= MIN_REST_TIME)
                                                            && (new_rest_time <= MAX_REST_TIME)
                                                            && ((times[0] + new_flight_time) <= MAX_FLIGHT_TIME)
                                                            && ((times[2] + new_rest_time + new_flight_time)<=MAX_PAIRING_TIME)
                                                            && (isGreater(times[3], vuelos_sorted[i].departure_time)) ){
            times[0] = times[0] + new_flight_time;
            times[1] = times[1] + new_rest_time;
            times[2] = times[2] + new_rest_time + new_flight_time;
            times[3] = vuelos_sorted[i].arrival_time;

            sol[0].flight_id = vuelos_sorted[i].flight_id;

            sol[0].origin = vuelos_sorted[i].origin;
            sol[0].destination = vuelos_sorted[i].destination;


            sol[0].departure_time = vuelos_sorted[i].departure_time;

            sol[0].arrival_time = vuelos_sorted[i].arrival_time;

            return sol;
        }

    }


}

// Funcion que retorna verdadero si el pairing termina en la mism base que comenzo
bool finishInBase(vector<int> candidates, vector<leg> vuelos){ 
    if (candidates.size()<2){
        return false;
    }else{
        string start;
        string finish;
        for (int i=0; i<vuelos.size(); i++){
            if (vuelos[i].flight_id == candidates.front()){
                start = vuelos[i].origin;
            }
        }
        for (int i=0; i<vuelos.size(); i++){
            if (vuelos[i].flight_id == candidates.back()){
                finish = vuelos[i].destination;
            }
        }
        if (start==finish){
            return true;

        }else{
            return false;
        }
    }
}

//obtiene el indice del vuelo especifico
int getIndex(vector<leg> tramo, vector<leg> vuelos){ 
    for (int i=0; i<vuelos.size(); i++){
        if (tramo[0].flight_id == vuelos[i].flight_id){
            return i;
        }
    }

}

// funcion que genera pairings
vector<int> generate_pairings(vector<leg> &vuelos, int n_legs, vector<vector<int>> &pairings){ 
    //Las bases corresponden a IST y ANK.
    //Tiempo mínimo entre vuelos 0.5 horas
    //Tiempo máximo entre vuelos 4 horas
    //Largo máximo de un duty 12 horas
    //Tiempo máximo de vuelo 8 horas


    vector<int> candidate_route;

    int base_index;

    int departure_time;
    int arrival_time;

    string destination;

    int flight_time;
    int rest_time;
    int pairing_time;

    bool first = true;

    vector<int> times;
    vector<leg> next_flight;

    vector<leg> vuelos_sorted = vuelos;
    sort(vuelos_sorted.begin(), vuelos_sorted.end(), compareByLength);


    //base_index = 0;
    base_index = random_base(vuelos_sorted, n_legs);
    destination = vuelos_sorted[base_index].destination; // destino del primer vuelo

    departure_time = vuelos_sorted[base_index].departure_time;

    arrival_time = vuelos_sorted[base_index].arrival_time;


    flight_time = difftime(departure_time, arrival_time);
    rest_time=0;
    pairing_time = flight_time + rest_time;

    times.push_back(flight_time);
    times.push_back(rest_time);
    times.push_back(pairing_time);
    times.push_back(arrival_time);

    int j=0;
    do {

        candidate_route.clear();
        //vuelos_sorted = vuelos;

        if (first==true){
            // aqui se agrega el primer vuelo del pairing
            destination = vuelos_sorted[base_index].destination; // destino del primer vuelo

            departure_time = vuelos_sorted[base_index].departure_time;

            arrival_time = vuelos_sorted[base_index].arrival_time;


            flight_time = difftime(departure_time, arrival_time);
            rest_time=0;
            pairing_time = flight_time + rest_time;

            times.push_back(flight_time);
            times.push_back(rest_time);
            times.push_back(pairing_time);
            times.push_back(arrival_time);

            candidate_route.push_back(vuelos_sorted[base_index].flight_id);

            vuelos_sorted.erase(vuelos_sorted.begin()); // elimino el vuelo desde los vuelos.

            first = false;

        }


        do {
            next_flight = searchNext(vuelos_sorted, base_index, destination, times, candidate_route);

            if (!(next_flight[0].flight_id==0)){

                destination = next_flight[0].destination;

                candidate_route.push_back(next_flight[0].flight_id);

                if(find(pairings.begin(), pairings.end(), candidate_route) != pairings.end()) {
                    continue;
                } else {
                    if (finishInBase(candidate_route,vuelos)){
                    pairings.push_back(candidate_route);
                    }

                }


                int index = getIndex(next_flight, vuelos_sorted);

                vuelos_sorted.erase(vuelos_sorted.begin()+index);



            }else{

                if(find(pairings.begin(), pairings.end(), candidate_route) != pairings.end()) {
                    continue;
                } else {
                    if (finishInBase(candidate_route,vuelos)){
                    pairings.push_back(candidate_route);
                    first=true;
                    times[0] = 0;
                    times[1] = 0;
                    times[2] = 0;
                    times[3] = 0;
                    times[4] = 0;

                    }

                }

            }

            j++;

        } while(!(next_flight[0].flight_id==0));



    } while(j < 1);

    return candidate_route;


}


//agrega elementos a un set de vuelos
void func (vector<vector<int>> pairings, int index, set<int>& a){ 

    for (vector<int>::size_type j = 0; j < pairings[index].size(); j++){

        a.insert(pairings[index][j]);
    }

}

//retorna verdadero si la solucion tiene todos los vuelos
bool coverAllFlights(vector<int> current_solution,vector<vector<int>> pairings, int n_legs, set<int>& coveredByPairing){
    
    coveredByPairing = {};

    for (int i=0; i<current_solution.size(); i++){
        if (current_solution[i]==1){
            func(pairings,i, coveredByPairing);

        }
    }

    if (coveredByPairing.size()==n_legs){
        return true;
    }else{


        return false;

    }

}

vector<int> generate_initial_solution(vector<vector<int>> pairings, int n_legs, set<int> coveredByPairing){
    //genero una solucion inicial
    int pairings_size = pairings.size();
    vector<int> unos(pairings_size,1);

    vector<int> solution(pairings_size);

    //genero una probabilidad que se llenen con ceros la solucion
    constexpr double dropout =0.2;
    constexpr size_t size= 1000;
    random_device rd;
    mt19937 gen(rd());
    bernoulli_distribution dist(1-dropout);

    generate(solution.begin(),solution.end(),  [&]{return dist(gen);});


    return solution;




}

// el movimiento hill-climbing
vector<int> bitFlip(vector<int> current_solution, vector<vector<int>> pairings){ 

    int index = rand()%pairings.size();

    if (current_solution[index] == 0){
        current_solution[index] = 1;
    } else {
        current_solution[index] = 0;

    }


    return current_solution;

}







float evaluationFunction(vector<int> current_solution, vector<vector<int>> pairings, int n_legs, set<int> coveredByPairing){
    float total_flight_cost = 0;
    float total_rest_cost = 0;
    float extra_cost = 0;
    float costNotCover = 0;
    for (int i=0; i<current_solution.size(); i++){
        if (current_solution[i]==1){
            total_flight_cost =  total_flight_cost +  pairings[i].size()*FLIGHT_COST;
            total_rest_cost = total_rest_cost + (pairings[i].size()-1)*WAIT_COST;
            extra_cost = extra_cost + (1/pairings[i].size()); //penalizacion
        }
    }
    if (!coverAllFlights(current_solution,pairings, n_legs, coveredByPairing)){
        costNotCover = 20;  //penalizacion
    }
    float sum_costs=total_flight_cost + total_rest_cost + extra_cost + costNotCover;


    return sum_costs;

}

//lo que retorna la solucion final
float objectiveFunction(vector<int> current_solution, vector<vector<int>> pairings){ 
    float total_flight_cost = 0;
    float total_rest_cost = 0;
    float extra_cost = 0;
    float costNotCover = 0;
    for (int i=0; i<current_solution.size(); i++){
        if (current_solution[i]==1){
            total_flight_cost =  total_flight_cost +  pairings[i].size()*FLIGHT_COST;
            total_rest_cost = total_rest_cost + (pairings[i].size()-1)*WAIT_COST;
        }
    }

    float sum_costs=total_flight_cost + total_rest_cost;

    return sum_costs;


}

void showPairings(vector<vector<int>> pairings){
    for (vector<vector<int>>::size_type i = 0; i < pairings.size(); i++ ){
        cout << i << " :" ;
       for (vector<int>::size_type j = 0; j < pairings[i].size(); j++ )
       {
          cout << pairings[i][j] << "->";
       }
       cout << endl;

    }


}


void showSolution(vector<int> current_solution, vector<vector<int>> pairings){
    for (int i=0; i<current_solution.size(); i++){
        if (current_solution[i]==1){
            for (vector<int>::size_type j = 0; j < pairings[i].size(); j++){
                cout << " -> " << pairings[i][j];
            }
            cout << endl;
        }
    }

}

int main (int argc, const char *argv[]){

    high_resolution_clock::time_point t1 = high_resolution_clock::now();



    vector<vector<int>> pairings;

    /*  Se obtienen los argumentos que se pasan por consola    */
    string file_path = argv[1];
    int max_iterations = stoi(argv[2]);

    /*  Obtención de numero de vuelos a partir del nombre del archivo     */
    unsigned int a = file_path.rfind(' ');
    unsigned int b = file_path.rfind(' ',a-1);
    string legs = file_path.substr(b+1,a-b-1);
    int n_legs = stoi(legs);



    /*  Inicialización de la estructura de vuelos     */

    vector<leg> vuelos(n_legs);
/*
 *       Lee el archivo tipo .csv y guarda los datos
 */

    read_legs(file_path, n_legs, vuelos);

/*      Genero los pairings iniciales factibles     */



    for (int i = 0; i<NUM_RESTARTS; i++) {
        set<int> coveredByPairing = {};
        bool cubiertos = true;
        while(cubiertos){

            pairings = {};
            for (int i=0; i<MAX_GENERATED_PAIRINGS ; i++){
                generate_pairings(vuelos, n_legs,pairings);
            }

            int pairings_size = pairings.size();
            vector<int> unos(pairings_size,1);

            if (coverAllFlights(unos, pairings, n_legs,coveredByPairing)){
                cubiertos =false;
            }

        }


    /*      Se elige orden aleatorio de los pairings   */

        random_device rd;
        default_random_engine engine{rd()};
        shuffle(begin(pairings), end(pairings), engine);

    /*
     *       Imprime por pantalla los pairings
     */


        cout << "Pairings iniciales OK!" <<endl;

    /*      Inicio de Algoritmo Hill-Climbing   */

        vector<int> current_solution;
        vector<int> new_solution;
        int iterations = 0;
        bool optimo;
        current_solution = generate_initial_solution(pairings, n_legs, coveredByPairing );

        optimo = false;

        while(iterations < max_iterations){

            new_solution = bitFlip(current_solution, pairings);
            float cur_sol_eval = evaluationFunction(current_solution,pairings, n_legs, coveredByPairing);
            float new_sol_eval = evaluationFunction(new_solution, pairings,n_legs, coveredByPairing);

            if (new_sol_eval<cur_sol_eval){
                current_solution = new_solution;

            }
            iterations++;
        }

        cout << endl;
        cout << "-----------------------------------------------------------------------------" << endl;

        cout << "Mejor solución "<< i+1 <<": F(X)= " << objectiveFunction(current_solution,pairings) << endl;
        cout << "-----------------------------------------------------------------------------" << endl;

        cout << "Pairings : "<<endl;
        showSolution(current_solution,pairings);


        cout << endl;
        cout << "-----------------------------------------------------------------------------" << endl;

        bool covered = coverAllFlights(current_solution,pairings,n_legs,coveredByPairing);

        if (covered){
            cout << "Todos los vuelos cubiertos" << endl;

        }else{
            cout << "No todos los vuelos cubiertos" << endl;
        }
        cout << "-----------------------------------------------------------------------------" << endl;

        cout << "Iteraciones: "<<iterations << endl;



        cout << endl;
    }


    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    auto duration = duration_cast<seconds>( t2 - t1 ).count();
    cout << "Tiempo de ejecucion: "<<duration <<" segundos." << endl;
    return 0;

}
