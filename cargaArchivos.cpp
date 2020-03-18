//Funciones para gestión de carga de archivos (instancias)
//Luis Miguel Escobar Falcón - Universidad Tecnológica de Pereira

//Librerías necesarias
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <dirent.h>
#include <string.h>

#ifndef cambioTipoDato_H
 #include "cambioTipoDato.h"
#endif

//#ifndef operacionesArreglos_H
// #include "operacionesArreglos.h"
//#endif

#ifndef mostrarArreglos_H
 #include "mostrarArreglos.h"
#endif

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

//Función que obtiene todos los archivos de un directorio
vector<string> listarArchivos(char * directorio){
	//Vector para los nombres de los archivos
	vector<string> vectorArchivos;	
	//Variable auxiliar para el manejo de las rutas al iterar
	char dir_it[100];
	strcpy(dir_it,directorio);	
	DIR *pDIR;	
	struct dirent *entry;
	if( pDIR=opendir(directorio) ){
			while(entry = readdir(pDIR)){
					if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ){
						vectorArchivos.push_back(strcat(dir_it,entry->d_name));						
						strcpy(dir_it,"");
						strcpy(dir_it,directorio);
						//cout << entry->d_name << "\n";
					}
			}
			closedir(pDIR);
	}
	return vectorArchivos;
}

//Cargar en memoria la información de la instancia
/*void obtenerInformacionInstancia(string nombreArchivo,vector< vector<float> > &informacionFlota, vector< vector<float> > &matrizCoordenadas, vector< vector<float> > &matrizCostos, float &numeroVehiculos,float &numeroClientes){
	//Declaración de variables
	int banderaPrimeraLineaClientes,banderaInformacionFlota,banderaPrimeraLineaFlota;		
	string line;	
	float dij;	
	vector < vector<float> > matriz;				//Declaración de un arreglo de dos dimensiones	
	vector < vector<float> >::iterator iter_jj;		//Declaración de iterador de dos dimensiones
	vector <float>::iterator iter_ii;				//Declaración de iterador de una dimensión
	vector <string>::iterator iter_ii_string;		//Declaración de iterador de una dimensión tipo string	
	
	//Abrir el archivo y validar éxito en el proceso	
	ifstream arVRP(nombreArchivo.c_str());
	if(!arVRP.is_open()){
		//Reportar el error y suspender la ejecución
		cout << endl << "Error al abrir el archivo de la instancia: " << nombreArchivo << endl;		
		exit(1);
	}	
		
	//Inicializar bandera para identificar la primera línea con contenido del archivo (número de clientes de la instancia)
	banderaPrimeraLineaClientes=1;
	//Inicializar bandera para identificar la primera línea que contiene el número de tipos de la flota de vehículos
	banderaPrimeraLineaFlota=1;
	//Inicializar bandera para diferenciar la información de la información de los vehículos de los datos de los clientes
	banderaInformacionFlota=0;	
	//Recorrer el archivo línea por línea
	while(arVRP.good()){
		//Obtener línea
		getline(arVRP, line);	
		//Obtener cada una de las posiciones
		vector<string> R;
		StringExplode(line, " ", &R);
		//Validar que no entren líneas vacías a los diferentes arreglos
		if(R.size()>0){		
			//Validar si se trata de la primera línea para guardar el número de clientes
			if(banderaPrimeraLineaClientes==1){				
				//Obtener el número de clientes
				numeroClientes = convertirValorStringFloat(R[0]);					
				//Indicar que la primera línea ya ha sido recorrida
				banderaPrimeraLineaClientes = 0;				
			}else{//Si no es la primera línea revisar si se trata de la información de la flota o la información de los clientes											
				std::string::size_type found = line.find("//");							
				//unsigned found = line.find("//");//Alternativa en caso de problemas de comparación al correr el código en un procesador o sistema de 32 bits							
				//Si no presenta el separador y la bandera no se ha activado se trata de la información de los clientes y el depósito
				if(found==std::string::npos && banderaInformacionFlota==0){
					matrizCoordenadas.push_back(convertirVectorNumerosStringFloat(R));//Matriz con las coordenadas donde se encuentran ubicados los clientes
				}else{//Si se presenta el separador se trata de la información de la flota
					//Se activa la bandera que indica que se está recorriendo la sección de la información de la flota
					banderaInformacionFlota=1;										
					//Alternativa evitando la primera línea
										
					////Validar si se trata de la primera línea y que el separador no sea incluído para guardar el tamaño de la flota
					//if(banderaPrimeraLineaFlota==1 && found==std::string::npos){
					//	//Obtener el número de clientes
					//	numeroVehiculos = convertirValorStringFloat(R[0]);					
					//	//Indicar que la primera línea ya ha sido recorrida
					//	banderaPrimeraLineaFlota = 0;						
					//}else{																				
					//	//Validar que el separador no sea incluído en la información de la flota
					//	if(found==std::string::npos){
					//		informacionFlota.push_back(convertirVectorNumerosStringFloat(R));							
					//	}											
					//}
					//					
					//Validar que el separador no sea incluído en la información de la flota
					if(found==std::string::npos){						
						informacionFlota.push_back(convertirVectorNumerosStringFloat(R));							
					}								
				}			
			}
		}
	}	
	//Cerrar el archivo y liberar el puntero
	arVRP.close();
	
	//Construir la matriz de costos o distancias a partir de las coordenadas obtenidas			
	matrizCostos.resize(matrizCoordenadas.size());//Redimensionar número de filas según la instancia
	for(int i=0;i<matrizCoordenadas.size();i++){
		matrizCostos[i].resize(matrizCoordenadas.size());//Redimensionar número de columnas según la instancia
		for(int j=0;j<matrizCoordenadas.size();j++){
			//Restringir asignar un costo o distancia para el mismo punto (depósito o ciudad)
			if(i==j){
				matrizCostos[i][j]=0;
			}else{
				dij = sqrtf(powf((matrizCoordenadas[i][1]-matrizCoordenadas[j][1]),2)+powf((matrizCoordenadas[i][2]-matrizCoordenadas[j][2]),2));					
				//matrizCostos[i][j]=(static_cast<int>(dij+0.5));//Distancia utilizando redondeo
				matrizCostos[i][j]=dij;//Distancia sin utilizar redondeo
			}
		}				
	}
	
	//Mostrar información cargada	
	//cout<<"Información Ciudades"<<endl;
	//mostrarMatriz(matrizCoordenadas);
	
	//cout<<"Información Flota"<<endl;
	//mostrarMatriz(informacionFlota);
	
	//cout<<"Matriz Costos"<<endl;
	//mostrarMatriz(matrizCostos);
	
	//cout<<"Número Clientes "<<numeroClientes<<endl;
	//cout<<"Número Vehículos "<<numeroVehiculos<<endl;		
	//cout<<"Número Vehículos "<<informacionFlota[0][0]<<endl;
	
}*/

string mostrarMenuInstancias(char * directorio){	
	//Declaración de variables
	int instancia, subindice;	
	//Variables para el manejo de los archivos	
	vector<string> vectorDirectorios; 				//Vector para los nombres de los directorios	
	vector<string>::iterator iteradorDirectorios; 	//Iterador para el vector de directorios
		
	vector<string> lineaSalida; 				//Vector para las líneas del archivo de salida
	vector<string>::iterator iteradorSalida; 	//Iterador para el vector del archivo de salida	
	
	//Elegir cuál de las instancias se va a correr	
	vectorDirectorios = listarArchivos(directorio);	
	subindice = 0;	
	for(iteradorDirectorios = vectorDirectorios.begin(); 
		iteradorDirectorios != vectorDirectorios.end();
		iteradorDirectorios++)
	{
		cout<<subindice<<" - "<<*iteradorDirectorios<<endl;		
		subindice++;		
	}		
	cout << "Ingresar el código de la instancia que se va a correr: ";	
	cin >> instancia;
	
	//Validar que la instancia elegida sea válida	
	if(instancia>=0 && instancia<subindice){
		//Cargar instancia		
		//matrizInformacion = infoInstanciaHifi(vectorDirectorios[instancia]);		
		return vectorDirectorios[instancia];		
	}else{
		cout<<"Código de instancia inválido";
		exit(1);//Emitir código de error
	}	
}



//Función para cargar un grafo de un archivo de texto y representarlo con una lista de adyacencia:
/*
1 2 3 4 5 6 (Nodos)
2 3 (Arista 1)
5 1 (Arista 2)
.
.
.
x y (Arista n)
*/
/*vector< vector<string> > cargarGrafoListaAdyacencia(string nombreArchivo){		
	//Declaración de variables	
	int banderaPrimeraLinea;	
	string line;
	float i,j;
	vector< vector<string> > grafo;
	vector <string>::iterator it_string;			//Declaración de iterador de una dimensión tipo string
	vector < vector<float> >::iterator iter_jj;		//Declaración de iterador de dos dimensiones
	vector <float>::iterator iter_ii;				//Declaración de iterador de una dimensión
	
		
	//Abrir el archivo y validar éxito en el proceso	
	ifstream arGrafo(nombreArchivo.c_str());
	if(!arGrafo.is_open()){
		//Reportar el error y suspender la ejecución
		cout << endl << "Error al abrir el archivo que contiene la información del grafo: " << nombreArchivo << endl;		
		exit(1);
	}
		
	//Inicializar bandera para identificar la primera línea con contenido del archivo
	banderaPrimeraLinea=1;
	
	//Recorrer el archivo línea por línea
	while(arGrafo.good()){
		//Obtener línea
		getline(arGrafo, line);	
		//Obtener cada una de las posiciones
		vector<string> R;
		StringExplode(line," ", &R);
		//Validar que no entren líneas vacías a los diferentes arreglos
		if(R.size()>0){		
			//Validar si se trata de la primera línea para guardar los coeficientes de la función objetivo
			if(banderaPrimeraLinea==1){								
				//Validar que no se trate de una línea vacía
				if(R.size()!=0){
					//Reservar espacio para la lista de adyacencia del grafo
					grafo.resize(R.size());//Lista con el tamaño de todos los nodos																						
					//Indicar que la primera línea ya ha sido recorrida
					banderaPrimeraLinea = 0;
				}		
			}else{//Si no es la primera línea se trata de las aristas del grafo			
				//Almacenar cada una de las aristas			
				i = convertirValorStringFloat(R[0])-1;//En ambos casos se resta 1 para presentar concordancia con los subíndices de c
				j = convertirValorStringFloat(R[1])-1;				
				grafo[i].push_back(convertirValorFloatString(j));
				grafo[j].push_back(convertirValorFloatString(i));
				//Ordenar nodos adyacentes a ambos nodos de la arista
				sort(grafo[i].begin(),grafo[i].end());
				it_string = unique(grafo[i].begin(), grafo[i].end());//Quitar las repeticiones 
				grafo[i].resize(it_string - grafo[i].begin());//Redimensionar el arreglo después de extraer los nodos repetidos				
				sort(grafo[j].begin(),grafo[j].end());
				it_string = unique(grafo[j].begin(), grafo[j].end());//Quitar las repeticiones 
				grafo[j].resize(it_string - grafo[j].begin());//Redimensionar el arreglo después de extraer los nodos repetidos								
			}
		}
	}	
	//Cerrar el archivo y liberar el puntero
	arGrafo.close();	
	
	//Retorna la lista de adyacencia que representa al grafo
	return grafo;	
}*/

//Función para cargar las instancias para ruteamiento con empaquetamiento en el formato correspondiente
void cargarInstanciaCVRP_CLP(string nombreArchivo, instancia &instanciaVRPCLP){
		
	//Declaración de variables	
	int contadorLineas = 0;	
	string line;	
		
	//Abrir el archivo y validar éxito en el proceso	
	ifstream arInstancia(nombreArchivo.c_str());
	if(!arInstancia.is_open()){
		//Reportar el error y suspender la ejecución
		cout << endl << "Error al abrir el archivo que contiene la información de la instancia: " << nombreArchivo << endl;		
		exit(1);
	}
	
	//Recorrer el archivo línea por línea
	while(arInstancia.good()){		
		
		//Obtener línea
		getline(arInstancia, line);
		//Incrementar el contador de líneas
		contadorLineas++;
		//Obtener cada una de las posiciones
		vector<string> R;
		StringExplode(line," ", &R);
		
		//Cargar el nombre de la instancia
		if(contadorLineas==1){
			instanciaVRPCLP.nombreInstancia = R[1];
		}
		
		//Número de clientes
		if(contadorLineas==3){
			instanciaVRPCLP.numeroClientes = convertirValorStringFloat(R[0]);
			instanciaVRPCLP.demandaClientes.resize(instanciaVRPCLP.numeroClientes+1);
		}
		
		//Número de vehículos
		if(contadorLineas==4){
			instanciaVRPCLP.numeroVehiculos = convertirValorStringFloat(R[0]);
		}
		
		//Número de ítems
		if(contadorLineas==5){
			instanciaVRPCLP.numeroItems = convertirValorStringFloat(R[0]);
		}
		
		//Dimensiones del contenedor del vehículo
		if(contadorLineas==7){
			instanciaVRPCLP.capacidadVehiculo = convertirValorStringFloat(R[0]);			
			instanciaVRPCLP.altoContenedor = convertirValorStringFloat(R[1]);
			instanciaVRPCLP.anchoContenedor = convertirValorStringFloat(R[2]);
			instanciaVRPCLP.largoContenedor = convertirValorStringFloat(R[3]);			
			instanciaVRPCLP.volumenContenedor = instanciaVRPCLP.altoContenedor * instanciaVRPCLP.anchoContenedor * instanciaVRPCLP.largoContenedor;			
		}
		
		//Almacenar las coordenadas y la demanda asociada a los puntos correspondientes de la instancia
		if(contadorLineas>=9 && contadorLineas<=(9 + instanciaVRPCLP.numeroClientes)){
			vector < float > auxiliar;
			for(int i=1;i<R.size();i++){
				auxiliar.push_back(convertirValorStringFloat(R[i]));
			}
			instanciaVRPCLP.coordenadas.push_back(auxiliar);
		}
		
		//Almacenar los detalles de la demanda de cada uno de los clientes (parte final del archivo que contiene la instancia, por eso no se valida el límite superior)
		if(contadorLineas>=(9 + instanciaVRPCLP.numeroClientes + 2)){
			//Recorrer toda la línea cargada del archivo correspondiente a la demanda de cada uno de los clientes
			int cliente;  
			int contadorPiezasCliente;
			vector < float > auxiliar;
			for(int i=0;i<R.size();i++){
								
				//Asociar el cliente o ubicación con el listado de piezas
				if(i==0){
					cliente = convertirValorStringFloat(R[i]);
				}
				
				//Validar si se trata del número de piezas para redimensionar el listado correspondiente
				if(i==1){
					//instanciaVRPCLP.demandaClientes[cliente].resize( (int)convertirValorStringFloat(R[i]) );						
				}				
								
				//Cargar la información de la pieza del cliente en el listado
				if(i>=2){
					if((i-1)%4 == 0){
						//Adicionar la variable de fragilidad de la pieza
						auxiliar.push_back(convertirValorStringFloat(R[i]));
						//Adicionar las banderas para el control de fragilidad (banderas de rotación)						
						if(auxiliar.back()==1){
							auxiliar.push_back(0);						
							auxiliar.push_back(0);
							auxiliar.push_back(1);
						}else{
							auxiliar.push_back(1);						
							auxiliar.push_back(1);
							auxiliar.push_back(1);
						}
						instanciaVRPCLP.demandaClientes[cliente].listadoItems.push_back(auxiliar);
						auxiliar.clear();
					}else{
						auxiliar.push_back(convertirValorStringFloat(R[i]));						
					}									
				}
								
			}
		}
	}	
	//Cerrar el archivo y liberar el puntero
	arInstancia.close();	
		
}

//Función para mostrar una estructura con la información de la instancia CVRP-CLP cargada en el formato correspondiente
void mostrarInstanciaCVRP_CLP(instancia casoEstudio){	
	cout<<endl<<"Nombre de la instancia: "<<casoEstudio.nombreInstancia<<endl;		
	cout<<endl<<"Número de clientes: "<<casoEstudio.numeroClientes<<endl;	
	cout<<endl<<"Número de ítems: "<<casoEstudio.numeroItems<<endl;	
	cout<<endl<<"Número de vehículos: "<<casoEstudio.numeroVehiculos<<endl;			
	cout<<endl<<"Alto del contenedor: "<<casoEstudio.altoContenedor<<endl;
	cout<<endl<<"Ancho del contenedor: "<<casoEstudio.anchoContenedor<<endl;
	cout<<endl<<"Largo del contenedor: "<<casoEstudio.largoContenedor<<endl;
	cout<<endl<<"Capacidad del contenedor: "<<casoEstudio.volumenContenedor<<endl;
	cout<<endl<<"Capacidad del contenedor (peso): "<<casoEstudio.capacidadVehiculo<<endl;				
	cout<<endl<<"Coordenadas de todos los puntos: ";
	mostrarMatriz(casoEstudio.coordenadas);		
	cout<<endl<<"Listado de Ítems: ";
	for(int i=0;i<casoEstudio.demandaClientes.size();i++){//Por cada uno de los clientes darle salida a la demanda
		cout<<endl<<"Demanda del cliente "<<i<<":"<<endl;
		mostrarMatriz(casoEstudio.demandaClientes[i].listadoItems);
	}	
	for(size_t i=0;i<casoEstudio.coordenadas.size();i++){
		cout<<endl<<"Peso de la demanda del cliente "<<i<<":"<<casoEstudio.coordenadas[i][2];		
	}
		
}


