#ifndef mostrarArreglos_H
#define mostrarArreglos_H

#include <vector> 
using namespace std;

/////////////////////////
//Listado de prototipos//
/////////////////////////

//Salida general de arreglos
void mostrarVectorEntero(vector<int>);
void mostrarVector(vector<float>);
void mostrarVectorString(vector<string>);
void mostrarMatriz(vector< vector<float> >);
void mostrarMatrizEntera(vector< vector<int> > );
void mostrarMatrizString(vector< vector<string> >);

//Salida de arreglos algoritmo SIMPLEX
void mostrarCuadroSimplex(vector< vector<float> >);

//Salidas de arreglos empleados en funciones que operan grafos
void mostrarMatrizCiclosHamiltonianos(vector< vector<float> >);
void mostrarMatrizEmparejamientoMinimo(vector< vector<float> >);
void mostrarListaAdyacencia(vector< vector<float> >);
void mostrarListaAdyacenciaString(vector< vector<string> >);
void mostrarNodos(vector<float>);

#endif
