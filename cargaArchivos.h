#ifndef cargaArchivos_H
#define cargaArchivos_H

#include <vector> 
#include <string> 
using namespace std;

//Estructura para almacenar la demanda de un cliente
struct demanda{
	vector < vector < float > > listadoItems;	
};

//Estructura para cargar la información de las instancias
struct instancia{	
	string nombreInstancia;
	float largoContenedor;		
	float anchoContenedor;		
	float altoContenedor;	
	float volumenContenedor;
	float capacidadVehiculo;	
	int numeroClientes;
	int numeroVehiculos;
	int numeroItems;
	vector < vector < float > > coordenadas;
	vector <demanda> demandaClientes;	
};

/////////////////////////
//Listado de prototipos//
/////////////////////////

//Carga de instancias
void obtenerInformacionInstancia(string,vector< vector<float> > &, vector< vector<float> > &, vector< vector<float> > &, float &,float &);

//Carga de grafos en archivos de texto plano (formato establecido en la definición de la función
vector< vector<string> > cargarGrafoListaAdyacencia(string);

//Exploración de directorios
vector<string> listarArchivos(char *);

//Menús
string mostrarMenuInstancias(char *);

//Función para cargar instancias de CVRP_CLP correspondientes
void cargarInstanciaCVRP_CLP(string, instancia &);

//Función para dar salida a instancias del tipo correspondiente
void mostrarInstanciaCVRP_CLP(instancia);


#endif
