//Librería de funciones para salida de arreglos n-dimensionales (pantalla y/o archivos)
//Luis Miguel Escobar Falcón - Universidad Tecnológica de Pereira

//Librerías necesarias
#include <vector>
#include <iostream>

//#ifndef operacionesArreglos_H
// #include "operacionesArreglos.h"
//#endif

#ifndef cambioTipoDato_H
 #include "cambioTipoDato.h"
#endif

using namespace std;

//Función para mostrar el contenido de un vector int en pantalla
void mostrarVectorEntero(vector<int> vectorRecibido){
	cout.precision(10);
	cout<<endl;
	for(int i=0;i<vectorRecibido.size();i++){
		cout<<vectorRecibido[i]<<" ";
		//cout<<vectorRecibido[i]<<endl;
	}
	cout<<endl;	
}

//Función para mostrar el contenido de un vector float en pantalla
void mostrarVector(vector<float> vectorRecibido){
	cout.precision(10);
	cout<<endl;
	for(int i=0;i<vectorRecibido.size();i++){
		cout<<vectorRecibido[i]<<" ";
		//cout<<vectorRecibido[i]<<endl;
	}
	cout<<endl;	
}

//Función para mostrar el contenido de un vector string en pantalla
void mostrarVectorString(vector<string> vectorRecibido){
	cout.precision(10);
	cout<<endl;
	for(int i=0;i<vectorRecibido.size();i++){
		//cout<<vectorRecibido[i]<<" ";
		cout<<vectorRecibido[i]<<endl;
	}
	cout<<endl;	
}

//Función para mostrar el contenido de una matriz en pantalla
void mostrarMatriz(vector< vector<float> > matrizRecibida){
	cout<<endl;
	for(int i=0;i<matrizRecibida.size();i++){
		for(int j=0;j<matrizRecibida[i].size();j++){
			//cout<<matrizRecibida[i][j]<<"\t";
			cout<<matrizRecibida[i][j]<<" ";
		}
		cout<<endl;
	}	
}

//Función para mostrar el contenido de una matriz en pantalla
void mostrarMatrizEntera(vector< vector<int> > matrizRecibida){
	cout<<endl;
	for(int i=0;i<matrizRecibida.size();i++){
		for(int j=0;j<matrizRecibida[i].size();j++){
			//cout<<matrizRecibida[i][j]<<"\t";
			cout<<matrizRecibida[i][j]<<" ";
		}
		cout<<endl;
	}	
}

//Función para mostrar el contenido de una matriz tipo string en pantalla
void mostrarMatrizString(vector< vector<string> > matrizRecibida){
	cout<<endl;
	for(int i=0;i<matrizRecibida.size();i++){
		for(int j=0;j<matrizRecibida[i].size();j++){
			cout<<matrizRecibida[i][j]<<" ";
		}
		cout<<endl;
	}	
}

//Función para mostrar el contenido del cuadro simplex
void mostrarCuadroSimplex(vector< vector<float> > matrizRecibida){
	cout<<endl;
	for(int i=0;i<matrizRecibida.size();i++){
		for(int j=0;j<matrizRecibida[i].size();j++){
			cout<<matrizRecibida[i][j]<<" ";
		}
		cout<<endl;
	}	
}

//Función para mostrar el contenido de una matriz que contiene en sus arreglos ciclos hamiltonianos con costo adjuntado
void mostrarMatrizCiclosHamiltonianos(vector< vector<float> > matrizRecibida){
	cout<<endl;
	for(int i=0;i<matrizRecibida.size();i++){
		for(int j=0;j<matrizRecibida[i].size();j++){
			if(j==matrizRecibida[i].size()-1){
				//cout<<matrizRecibida[i][j]<<"\t";			
				cout<<matrizRecibida[i][j]<<" ";
			}else{
				cout<<matrizRecibida[i][j]+1<<" ";
			}			
		}
		cout<<endl;
	}	
}

//Función para mostrar el contenido de la matriz de emparejamiento mínimo
void mostrarMatrizEmparejamientoMinimo(vector< vector<float> > matrizRecibida){
	cout<<endl;
	for(int i=0;i<matrizRecibida.size();i++){
		for(int j=0;j<matrizRecibida[i].size();j++){
			if(j==matrizRecibida[i].size()-1){
				//cout<<matrizRecibida[i][j]<<"\t";			
				cout<<matrizRecibida[i][j]<<" ";
			}else{
				cout<<matrizRecibida[i][j]+1<<" ";
			}			
		}
		cout<<endl;
	}	
}

//Función para mostrar el contenido de una lista de adyacencia de un grafo
void mostrarListaAdyacencia(vector< vector<float> > matrizRecibida){
	cout<<endl;
	for(int i=0;i<matrizRecibida.size();i++){
		if(matrizRecibida[i].size()==0){
			cout<<i+1<<" - ";
		}else{
			for(int j=0;j<matrizRecibida[i].size();j++){
				if(j==0){
					cout<<i+1<<" - "<<matrizRecibida[i][j]+1<<" ";
				}else{
					cout<<matrizRecibida[i][j]+1<<" ";				
				}			
			}
		}		
		cout<<endl;
	}	
}

//Función para mostrar el contenido de una lista de adyacencia de un grafo codificado en string
void mostrarListaAdyacenciaString(vector< vector<string> > matrizRecibida){
	cout<<endl;
	for(int i=0;i<matrizRecibida.size();i++){
		if(matrizRecibida[i].size()==0){
			cout<<i+1<<" - ";
		}else{
			for(int j=0;j<matrizRecibida[i].size();j++){
				if(j==0){
					cout<<i+1<<" - "<<convertirValorStringFloat(matrizRecibida[i][j])+1<<" ";
				}else{
					cout<<convertirValorStringFloat(matrizRecibida[i][j])+1<<" ";				
				}			
			}
		}		
		cout<<endl;
	}	
}

//Función para mostrar el contenido de un vector que almacena el listado de nodos de un grafo
void mostrarNodos(vector<float> listadoNodos){
	//cout.precision(10);
	cout<<endl;
	for(int i=0;i<listadoNodos.size();i++){
		cout<<listadoNodos[i]+1<<" ";		
	}
	cout<<endl;	
}

