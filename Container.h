#ifndef CONTAINER_H
#define CONTAINER_H
//lectura y escritura
#pragma warning(disable: 4786)
#define MAX_IMPORTANCIA 99999;
#define get_random(min,max) ((rand() %(int) (((max)+1)-(min)))+(min))
#include <sys/io.h>
#include <fcntl.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

//#include <iostream>
//#include <fstream>
#include <list>
#include <map>
#include <vector>
#include <set>
#include <utility>
#include <algorithm>
#include <math.h>
#include <assert.h>
#include <string>
#include <sstream>
#ifndef cargaArchivos_H
 #include "cargaArchivos.h"
#endif
//Para que pueda funcionar con las mfc
//#include <afxwin.h>

using namespace std;


class PIECE;
class BLOQUE;
class SPACE;
class CONTAINER;
class CONFIGURACAO;
//class MOV_AUMENTAR;
class PLANOSOPORTE;
//************
//Clase ORIG
//
class PLANOSOPORTE
{
	//
	int xi;
	int yi;
	int xf;
	int yf;
	int z;
	int soporte;

//	std::vector <short int> m_orig;
public:
	//constructor
	PLANOSOPORTE(int xi,int yi,int xf, int yf, int z, int soporte);
	PLANOSOPORTE();
	~PLANOSOPORTE(){};
	
	int Getxi(){return xi;};
	int Getyi(){return yi;};
	int Getz(){return z;};
	void Setxi(int a){xi=a;};
	void Setyi(int a){yi=a;};
	void Setz(int a){z=a;};

	int Getxf(){return xf;};
	int Getyf(){return yf;};
	
	void Setxf(int a){xf=a;};
	void Setyf(int a){yf=a;};
	

	int Getsoporte(){return soporte;};
	void Setsoporte(int a){soporte=a;};
	//	short int &ORIG::operator [](int i){return m_orig[i];};
};

class ORIG
{
	//
	short int m_primero;
	short int m_segundo;
	short int m_tercero;
//	std::vector <short int> m_orig;
public:
	//constructor
	ORIG();
	~ORIG(){};
	ORIG(int a,int b,int c);
	short int GetPrimero(){return m_primero;};
	short int GetSegundo(){return m_segundo;};
	short int GetTercero(){return m_tercero;};
	void SetPrimero(short int a){m_primero=a;};
	void SetSegundo(short int a){m_segundo=a;};
	void SetTercero(short int a){m_tercero=a;};

	//	short int &ORIG::operator [](int i){return m_orig[i];};
};

//****************
//Clase Espacio 
//****************
class SPACE{
	// origen
	int m_x1; int m_y1; 	int m_z1; 
	// fin 
	int m_x2; int m_y2; 	int m_z2; 
	// largo, ancho, alto
	int m_dx; int m_dy; 	int m_dz; 

		int r_y1; int r_y2;     int r_L;

	bool m_Flag; // true si cabe algo, false en el otro sentido
	int m_Volumen; //Volumen de la pieza
	int m_val_dist;
	ORIG m_distancia_origen; //Distancia al origen
	//es a que esquina es la que esta más cercana
	// 1.(0,0,0) , 2.(0,Y,0)  3.(0,0,Z)  4.(0,Y,Z)  5.(X,0,0)  6.(X,Y,0)  7.(X,0,Z)  8.(X,Y,Z) 
	short int m_origen_distancia; 
	//Puntero estatico al Contenedor
	static CONTAINER *m_Container;
	int m_soporta_peso;	//----------------------------------------------
	int m_soporta_peso_arriba;//-----------------------------------------
	float m_peso;
	int m_peso_espacio;//----------------------------------------
public:
	//constructor
	//Por coordenadas
	SPACE(int,int, int , int , int,int , CONTAINER *a );
	SPACE(int,int, int , int , int,int, int , int,int,int,CONTAINER *a );
	//Por vertice
	SPACE( CONTAINER *a ,int,int, int , int , int,int );
	SPACE( CONTAINER *a ,int,int, int , int , int,int,int, int, int, bool );
	SPACE(){};
	//destructor
	~SPACE(){};
	void Peso(float a){m_peso=a;};

	//Funciones Get
	float Get_Peso(){return m_peso;};
	int Get_x1(){return m_x1;};
	int Get_y1(){return m_y1;};
	int Get_z1(){return m_z1;};
	int Get_x2(){return m_x2;};
	int Get_y2(){return m_y2;};
	int Get_z2(){return m_z2;};
	int Get_dx(){return m_dx;};
	int Get_dy(){return m_dy;};
	int Get_dz(){return m_dz;};
	int Get_r_y1(){return r_y1;};
	int Get_r_y2(){return r_y2;};
	int Get_r_L(){return r_L;};
	int Get_m_soporta_peso(){return m_soporta_peso;};//--------------
	int Get_m_soporta_peso_arriba(){return m_soporta_peso_arriba;};//--------------
	bool Get_Flag(){return m_Flag;};
	short int Get_origen_distancia(){return m_origen_distancia;};
	int Get_Dist_Euclidea(){return m_val_dist;};
	ORIG & Get_Distancia(){return m_distancia_origen;};
//Funciones set
	void Set_m_soporta_peso(int a){m_soporta_peso=a;};//---------------------
	void Set_m_soporta_peso_arriba(int a){m_soporta_peso_arriba=a;};//---------------------
	void Set_Flag(bool a){m_Flag=a;};
	void Cambiar(ORIG &orig,int a,int b,int c);
	void ModificarSpace(int x1,int y1, int z1, int x2, int y2, int z2);
	void ModificarSpace(int x1,int y1, int z1, int x2, int y2, int z2, int soporte);//----------
	void ModificarSpacePeso(int x1,int y1, int z1, int x2, int y2, int z2, int soporte, int, int,int);
	void RecortarSpace(int x1,int y1, int z1, int x2, int y2, int z2, int, int, int);
	int Get_Volumen(){return m_Volumen;};
	//Calcula la minima distancia al origen con muchas podas
	void CalcularDistanciaOrigen();
	//Calcula la minima distancia al origen de la forma original
	void CalcularDistanciaOrigenOriginal();
	//Funcion de distancia 2 que solamente hace para cumplir la gravedad
	void CalcularDistanciaOrigenDos();
	void CalcularDistanciaOrigenEuclidea();
	void CalcularDistanciaOrigenFullSupported();
	void CalcularDistanciaOrigenCuatro();
	void CambiarEsquina(SPACE & );
	//Funcion para comparar dos set
//	bool Comparar_DistA_DistB(ORIG &A, int VolA,ORIG &B, int VolB);
	//Me dice si este espacio esta incluido en E
	bool Incluido(SPACE &E);

	void Set_r_y1(int a){r_y1=a;};
	void Set_r_y2(int a){r_y2=a;};
	void Set_r_L(int a){r_L=a;};

};

//*********************
// CLASE CONFIGURACAO
//********************
class CONFIGURACAO{
	int m_Id; //numero para identificar la pieza
	//Este es de las seis posibles maneras que se puede colocar
	//1.- apoyada cara x , 2.- rotada eje x
	//3.- apoyada cara y, 4.- rotada eje y
	//5.- apoyada cara z, 6.- rotada eje z
	short int m_tipo; 
	short int m_origen_distancia;
	ORIG m_set_distancias;
	int m_x1; //Posicion inicio
	int m_y1; //Posicion inicio 
	int m_z1; //Posicion inicio
	int m_x2; //Posicion fin
	int m_y2; //Posicion fin
	int m_z2; //Posicion fin
	int m_dx; //Numero de piezas en la dirección x
	int m_dy; //Numero de piezas en la dirección y
	int m_dz; //Numero de piezas en la dirección z
	int m_Volumen; //Volumen de la pieza
	int m_Costo;
	int m_num; //numero de piezas utilizado
	int m_idcliente;
	static CONTAINER *m_Container;
	float m_peso;
	int m_soporte_peso;
	int m_soporte_peso_arriba;
	int m_cliente_config;

public:
	//destructor
	~CONFIGURACAO(){};
	//constructor
	CONFIGURACAO(){};
	CONFIGURACAO(CONTAINER *a ,int Id, int x1,int y1, int z1, int x2, int y2, int z2,short int tipo , int num,short int orig);

	CONFIGURACAO(CONTAINER *a ,int Id, int x1,int y1, int z1, int x2, int y2, int z2,short int tipo , int num);
	CONFIGURACAO(CONTAINER *a ,ORIG sm, int num);
	CONFIGURACAO(CONTAINER *a ,int Id, int x1,int y1, int z1, int tx, int ty, int tz,int dx, int dy, int dz,  short int tipo , int num);
	CONFIGURACAO(CONTAINER *a ,int Id, int x1,int y1, int z1, int tx, int ty, int tz,int dx, int dy, int dz,  short int tipo , int num,short int orig);

	//Funciones Get
	//Funciones Get
	float Get_Peso(){return m_peso;};
	int Get_m_soporte_peso(){return m_soporte_peso;};
	int Get_m_soporte_peso_arriba(){return m_soporte_peso_arriba;};
	int Get_Id(){return m_Id;};
	int Get_x1(){return m_x1;};
	int Get_y1(){return m_y1;};
	int Get_z1(){return m_z1;};
	int Get_x2(){return m_x2;};
	int Get_y2(){return m_y2;};
	int Get_z2(){return m_z2;};
	void Set_Id(int a){m_Id=a;};
	void Set_x1(int a){m_x1=a;};
	void Set_y1(int a){m_y1=a;};
	void Set_z1(int a){m_z1=a;};
	void Set_x2(int a){m_x2=a;};
	void Set_y2(int a){m_y2=a;};
	void Set_z2(int a){m_z2=a;};
	void Set_dx(int a){m_dx=a;};
	void Set_dy(int a){m_dy=a;};
	void Set_dz(int a){m_dz=a;};
	void Set_Vol(int a){m_Volumen=a;};
	void Set_Costo(int a){m_Costo=a;};
	void Set_Num(int a){m_num=a;};
	void Set_m_cliente_config(int a){m_cliente_config=a;};
	short int Get_Tipo(){return m_tipo;};
	int Get_dx(){return m_dx;};
	int Get_dy(){return m_dy;};
	int Get_dz(){return m_dz;};
	int Get_Num(){return m_num;};
	int Get_Volumen(){return m_Volumen;};
	int Get_Costo(){return m_Costo;};
	int Get_m_cliente_config(){return m_cliente_config;};
	ORIG & Get_Distancia(){return m_set_distancias;};
	void CambiarEsquina(SPACE &E);
	short int Get_orig(){return m_origen_distancia;};

	//


	//Operaciones sencillas de la configuraca
	bool Mejor_DistA_Que_DistB_Configuracaos(ORIG &A, int NumA,ORIG &B, int NumB,int Vol,int ,int);
	bool Mejor_DistA_Que_DistB_Configuracaos(CONFIGURACAO &C);
	void PonerMedidas2(int x1,int y1, int z1, int x2, int y2, int z2);

	//Para crear columnas
	void MejorColumnaDeUnaPieza(SPACE &,PIECE &);


	void CompararConColumnas(short int tipo,SPACE &Espacio, int x, int y,int z, int Id,int );
	//Para crear capas


	void MejorCapaDeUnaPieza_Liu(SPACE &,PIECE &);
	void MejorCapaDeUnaPieza_Junqueira(SPACE &,PIECE &);
	void MejorCapaDeUnaPiezaPeso(SPACE &,PIECE &);
	void MejorCapaDeUnaPiezaPeso_Ceschia(SPACE &,PIECE &);
	void MejorCapaDeUnaPiezaPeso_Pesos_Sin_Multidrop(SPACE &,PIECE &);
	//void CompararConCapasPeso(short int tipo,SPACE &Espacio, int x, int y,int z, int Id,int piezas_maximas,int fuerza,int densidad,int soporte,int Vol);
	void CompararConCapasPeso(short int tipo,SPACE &Espacio, int x, int y,int z, int Id,int piezas_maximas,int fuerza,int densidad,int soporte,int Volumen, int cliente);
	void CompararConCapasPeso_Ceschia(short int tipo,SPACE &Espacio, int x, int y,int z, int Id,int piezas_maximas,int fuerza,int densidad,int soporte,int Volumen, int cliente, int Costo);
	void CompararConCapasPeso_Pesos_Sin_Multidrop(short int tipo,SPACE &Espacio, int x, int y,int z, int Id,int piezas_maximas,int fuerza,int densidad,int soporte,int Volumen);

	void CompararConCapas(short int tipo,SPACE &Espacio, int x, int y,int z, int Id,int piezas_maximas,int soporte,int Vol);
	void CompararConCapas_Daneses(short int tipo,SPACE &Espacio, int x, int y,int z, int Id,int piezas_maximas,int soporte,int Vol);
	void CompararConCapas_Liu(short int tipo,SPACE &Espacio, int x, int y,int z, int Id,int piezas_maximas,int soporte,int Vol);
	void CompararConCapas_Junqueira(short int tipo,SPACE &Espacio, int x, int y,int z, int Id,int piezas_maximas,int soporte,int Vol);
	//Para crear bloque
	void MejorBloqueDeUnaPieza(SPACE &,PIECE &);

	void CompararConBloques(short int tipo,SPACE &Espacio, int x, int y,int z, int Id,int piezas_maximas);
	//Esta funcion me actualiza la configuracicao
	void CambiarConfiguracao(int numpiezas, int ca,int cb, int cc, int a,int b,int c, int id, short int tipo_rota, SPACE &Espacio,int );
	void CambiarConfiguracao_Daneses(int numpiezas, int ca,int cb, int cc, int a,int b,int c, int id, short int tipo_rota, SPACE &Espacio,int,int );
	void CambiarConfiguracao_Liu(int numpiezas, int ca,int cb, int cc, int a,int b,int c, int id, short int tipo_rota, SPACE &Espacio,int,int );
	void CambiarConfiguracao_Junqueira(int numpiezas, int ca,int cb, int cc, int a,int b,int c, int id, short int tipo_rota, SPACE &Espacio,int,int );
	void CambiarConfiguracaoPeso(int numpiezas, int ca,int cb, int cc, int a,int b,int c, int id, short int tipo_rota, int fuerza, SPACE &Espacio,int peso_soporta,int Vol, int Cliente );
	void CambiarConfiguracaoPeso_Pesos_Sin_Multidrop(int numpiezas, int ca,int cb, int cc, int a,int b,int c, int id, short int tipo_rota, int fuerza, SPACE &Espacio,int peso_soporta,int Vol );
	void CambiarConfiguracaoPeso_Ceschia(int numpiezas, int ca,int cb, int cc, int a,int b,int c, int id, short int tipo_rota, int fuerza, SPACE &Espacio,int peso_soporta,int Vol, int Cliente, int Cos );
	//void CambiarConfiguracaoPeso(int numpiezas, int ca,int cb, int cc, int a,int b,int c, int id, short int tipo_rota, int fuerza, SPACE &Espacio,int peso_soporta,int Vol, int Costo, int Cliente );
	//Esta función es creada para que  ponga las medidas en la configuración
	//pero la función de arriba las pone ahora directamente
	void InsertarOrdenadaRCL(CONFIGURACAO &Actual);
	void InsertarOrdenadaRCL_Daneses(CONFIGURACAO &Actual);
	void InsertarOrdenadaRCL_Ceschia(CONFIGURACAO &Actual);
	void InsertarOrdenadaRCL_Pesos_Sin_Multidrop(CONFIGURACAO &Actual);
	void InsertarOrdenadaRCL_Liu(CONFIGURACAO &Actual);
	void InsertarOrdenadaRCL_Junqueira(CONFIGURACAO &Actual);
	void Cambiar(ORIG &orig,int a,int b,int c);
	bool Mayor(CONFIGURACAO &C,short int tipo);
//bool CONFIGURACAO::operator<(const CONFIGURACAO &p2)
bool operator<(const CONFIGURACAO &p2) ;
//Funcion para poner el numero de piezas en cada orientacion de la configuracao
	void PonerMedidas(int dx,int dy, int dz);
	bool Mejor_DistA_Que_DistB_FullSupported(ORIG &A, int NumA,ORIG &B, int NumB,int Vol,int dx,int dy, int dz);
	bool Mejor_DistA_Que_DistB_Pesos(ORIG &A, int NumA,ORIG &B, int NumB,int Vol,int dx,int dy, int dz);



};
//*********************
// CLASE BLOQUE
//********************
class BLOQUE{
	int m_Id1; //numero para identificar la pieza 1
	int m_Id2;
	int m_Id;
	int m_x; //Largo tablero
	int m_y; //Ancho tablero
	int m_z;
	int m_num1;
	int m_num2;
	int m_x1;
	int m_y1;
	int m_x2;
	int m_y2;
	short int m_tipo_1;
	short int m_tipo_2;
	int m_q; //Numero de piezas
	//Siempre alguno es posible
	bool m_Rotate_x; //SE puede rotar en dirección vertical en el eje x
	bool m_Rotate_y; //Se puede rotar en dirección en el eje y
	bool m_Rotate_z; //Se puede rotar en dirección en el eje z.
	int m_Volumen; //Volumen de la pieza
	bool m_perdida;
    int m_peso_bloque; //---------------------------------------
	int m_soporte_bloque;//-------------------------------------
	float m_densidad_bloque; //---------------------------------
	int  m_S_x;
	int  m_S_y;
	int  m_S_z;
	int m_cliente_bloque;
	//int m_costo_bloque;
	int m_num; //numero de piezas utilizado
	//Puntero estatico al Contenedor
	static CONTAINER *m_Container;

public:
	//constructor
	BLOQUE(int,int,  int ,  int  , int ,CONTAINER *a,int,int );
	//BLOQUE::BLOQUE(int id1, int id2,int x, int y, int z, int x1,int y1, int x2, int y2, CONTAINER *a, int ,int );
	//BLOQUE::BLOQUE(int id1, int id2,int x, int y, int z, int x1,int y1, int x2, int y2, CONTAINER *a,int ,int ,short int , short int );
	//BLOQUE::BLOQUE(int id1, int id2,int x, int y, int z, int x1,int y1, int x2, int y2, CONTAINER *a,int ,int ,short int , short int, int, int, float, float);

	int Get_Sx(){return m_S_x;};
	int Get_Sy(){return m_S_y;};
	int Get_Sz(){return m_S_z;};
	int Get_m_peso_bloque(){return m_peso_bloque;};//----------------------
	int Get_m_soporte_bloque(){return m_soporte_bloque;};//----------------------
	int Get_m_cliente(){return m_cliente_bloque;};//----------------------
	//int Get_m_costo(){return m_costo_bloque;};//----------------------
	float Get_m_densidad_bloque(){return m_densidad_bloque;};//----------------------
	BLOQUE(){};
	//destructor
	~BLOQUE(){};
	//Funciones set
	void Set_x(int a){m_x=a;};
	void Set_y(int a){m_y=a;};
	void Set_z(int a){m_z=a;};
	void Set_Num(int a){m_num=a;};
	void Set_m_peso_bloque(int a){m_peso_bloque=a;};//------------------
	void Set_m_soporte_bloque(int a){m_soporte_bloque=a;};//------------------
	void Set_m_densidad_bloque(float a){m_densidad_bloque=a;};//--------------
	void Set_m_cliente(float a){m_cliente_bloque=a;};//--------------
	//void Set_m_costo_bloque(float a){m_costo_bloque=a;};//--------------
	//Funciones Get
	int Get_Id1(){return m_Id1;};
	int Get_Id2(){return m_Id2;};
	int Get_Num1(){return m_num1;};
	int Get_Num2(){return m_num2;};
	int Get_Id(){return m_Id;};
	int Get_x(){return m_x;};
	int Get_y(){return m_y;};
	int Get_x_P1(){return m_x1;};
	int Get_y_P1(){return m_y1;};
	int Get_x_P2(){return m_x2;};
	int Get_y_P2(){return m_y2;};
	int Get_z(){return m_z;};
	bool Get_Rx(){return m_Rotate_x;};
	bool Get_Ry(){return m_Rotate_y;};
	bool Get_Rz(){return m_Rotate_z;};
	bool Get_Perdida(){return m_perdida;};
	int Get_Num(){return m_num;};
	int Get_q(){return m_q;};
	int Get_Volumen(){return m_Volumen;};
	void Add_Num(int a){m_num+=a;};
};
//*********************
// CLASE MOV_AUMENTAR
//********************
class MOV_AUMENTAR{
	CONFIGURACAO m_Configuracao;
	std::list<CONFIGURACAO> m_Configuracaos_new;
	//multiset donde mete cuantos hay en cada cuadrante
	std::multiset< double > m_val;
	int m_vol;
	static CONTAINER *m_Container;

public:
	MOV_AUMENTAR(CONTAINER *a){m_Container=a;};
	//constructor
	MOV_AUMENTAR(){};
	//destructor
	~MOV_AUMENTAR(){};
	//Funciones Get

	//Funciones Get
	int Get_Vol(){return m_vol;};
	std::multiset< double > &Get_Val(){return m_val;};
	CONFIGURACAO &Get_Configuracao(){return m_Configuracao;};
	std::list<CONFIGURACAO> &Get_Configuracaos(){return m_Configuracaos_new;};

	//**funciones de 
	//hay que evaluar el movimiento
	//esta funcion me dice cuanto es lo que vale ese movimiento
	//hace las intersecciones con los otros bloque y calcula
	//el valor de esa interseccion
	void EvaluacionMovimiento(CONFIGURACAO &);
	//Cuenta cuanto hay en cada cuadrante del container con la 
	//nueva lista de bloque
	void EvaluarMovimiento();


	void ActualizarBloquesElegido( CONFIGURACAO &E,CONFIGURACAO &C,std::list <CONFIGURACAO> &lista_temp);

	void ActualizarListaDeBloques();

	void MejorCapaDeUnaPiezaEspacio(SPACE &Espacio,PIECE &Pieza);
	void CompararConCapasEspacio(short int tipo,SPACE &Espacio, int x, int y,int z, int Id,int piezas_maximas);
	void ListaDeBloquesNuevos(CONFIGURACAO &C,std::list< CONFIGURACAO> &lista);

	void CambiarConfiguracao(CONFIGURACAO &C,int numpiezas, int ca,int cb, int cc, int a,int b,int c, int id, short int tipo_rota, SPACE &Espacio);

	void CompararMovimientos(MOV_AUMENTAR &otro);


	//	
	//
};
//******************
//CLASE CONTENEDOR
//******************
class CONTAINER{
	int m_X; //Largo tablero
	int m_Y; //Ancho tablero
	int m_Z;
	int m_Numpieces; //Numero de piezas totales
	int m_Q;//Numeror de tipos de piezas
	std::vector< PIECE > m_Pieces;//Lista de piezas
	std::list< SPACE > m_Spaces; //Lista de espacios
	std::list< CONFIGURACAO > m_Configuracaos; //Lista de bloques
	std::list< BLOQUE > m_Bloques;
	std::list< BLOQUE > m_Bloques_Mejora;
	std::list< BLOQUE > m_Bloques_Guardados;
	std::list< CONFIGURACAO > m_ListaRCL; //Lista Restringida de Candidatos
	char m_Nombre[240];//Nombre de la instancia
	int m_total_volumen; //Total del volumen posible de llenar con las piezas
	int m_total_volumen_ocupado; //Total del volumen que se ha llenado del container
	int m_total_costo_ocupado; //Total del costo que se ha llenado del container

	//menor dimension admitida, no tengo en cuenta rotaciones
	int m_dimension_menor;
	//menor volumen para que quepa una pieza
	int m_menor_peso;//--------------------------------------------------
	//menor volumen para que quepa una pieza
	int m_menor_volumen;
	//Funcion que me dice si el objetivo es el vol true o el orig fals
	bool m_objetivo_vol;
	//Funcion que me dices si quiero capas o columnas
	bool m_objetivo_capas;
	//Funcion que me dice si quiero uno mixto para el aleatorizado de col y capas
	bool m_tipo_mixto;
	//variable para indicarme si utilizo constructivo aleatorizado
	bool m_Aleatorizado;
	//Tamaño de la lista restringida de candidatos
	int m_TamRCL;
	double m_Delta;
	//numero de iteraciones Grasp
	int m_max_iteraciones_Grasp;
	//iteracion actual
	int m_iter_actual;
	//maximo del volumen ocupado
	int m_max_total_volumen_ocupado;
	int m_max_total_costo_ocupado;
	int Algoritmo_Opcion;
	bool Algoritmo_Opcion_Ceschia;
	bool Algoritmo_Opcion_Daneses;
    bool Algoritmo_Opcion_Junqueira;
	bool Algoritmo_Opcion_Liu;
	bool Algoritmo_Opcion_Pesos_Sin_Multidrop;
	bool m_mejora_local;
	bool m_mejora_local_cliente;
	//Vector con el numero de piezas utilizado hasta mejora
	std::vector <int> m_Pieces_Mejora;
	//Vector con el numero de piezas utilizado hasta mejora
		std::vector <int> m_Pieces_Mejora_cliente;
		std::vector <int> copia_m_Pieces_Mejora_cliente;
		std::vector <int> m_total_volumen_cliente;

	std::vector <int> vec_minima_longitud;
		
	//Lista de espacios hasta la mejora
	std::list <SPACE > m_Spaces_Mejora;
	std::list <SPACE > m_Spaces_Mejora_cliente;
	std::list <SPACE > copia_m_Spaces;
	//Lista de espacios hasta la mejora
	std::list <CONFIGURACAO > m_Configuracaos_Mejora;
	std::list <CONFIGURACAO > m_Configuracaos_Mejora_cliente;
	std::list <CONFIGURACAO > copia_m_Configuracoes;
		int m_cliente_Mejora;
			int m_cliente_Mejora_cliente;
		bool m_cambiar_cliente_Mejora;
			bool m_cambiar_cliente_Mejora_cliente;
	//volumen ocupado hasta el momento
	int m_total_volumen_ocupado_mejora;
		int m_total_volumen_ocupado_mejora_cliente;
		int m_total_costo_ocupado_mejora_cliente;
	int m_total_volumen_ocupado_cliente;

		std::vector <double> m_volumen_ocupado_cliente;
		std::vector <double> m_superficie_cliente;

	//Lista de espacios hasta la mejora

	//Lista de espacios hasta la mejora

	std::list<CONFIGURACAO> m_Best_Configuracao;
	std::list<PLANOSOPORTE> m_planos;
	//Variables para el método de mejora

	//volumen ocupado hasta el momento

	int m_total_costo_ocupado_mejora;


	//Guardo la peor solucion conseguida hasta ahora
	int m_PeorSolucion_volumen;
	//Para el reactive Grasp
	int m_hastavalor;
	//Para el programa que dibuja
	bool m_paso_a_paso;

	//CClientDC *m_clientedc;
	//factor para dibujar
	double m_factor;

	//es para dibujar o no
	bool m_Dibujar;
	//son los grados con los que hago el dibujo
	double m_grados;
	//Tipo de graveda
	short int m_tipo_gravedad;
	//Tipo de mejora
	//o original
	//1 vol
	//2 es la original + volumen
	short int m_tipo_mejora;
	//Variables para guardar tiempos
	clock_t m_inicio,m_fintime;
	double m_tiempo;
	double m_time_final;
	//Doubles para las medidas de estabilidad
	double m_medida1;
	double m_medida2;
	double m_medida100por100;
	double m_medidaCG;
	double m_medida60por100;
	//m_tipo_origen
	//2 dos esquinas 
	//4 cuatro esquinas
	//8 ocho esquinas
	int m_tipo_origen;
	//0 choose the space by my function
	//1 choose the space by euclidena
	short int m_tipo_eleccion_space;
	short int m_tipo_evaluacion;

	//
	double m_val_quita;
	bool m_val_quita_aleatorio;
	bool m_muchas_iteraciones;
	//
	bool m_full_supported;
	//
	bool m_pesos;
	bool m_cambiar_cliente;
	int m_num_total_clientes;
			bool copia_m_cambiar_cliente;

			int m_evaluacion1;
			int m_evaluacion2;
			int Algoritmo_Opcion_Construccion;
			bool Algoritmo_Opcion_Irrestricto;
			int Algoritmo_Opcion_Mejora;
			int Algoritmo_Arquivo_Imprimir;
			int m_veces_filtrar;

public:
	//constructor
   CONTAINER(instancia , int *, int );
	//CONTAINER(char *nombre,int caso,int opcion);
	//CONTAINER(int mx, int my, int mz);
	//Lee los datos del container y los modifica directamente en la clase container.
	CONTAINER(){};
	//destructor
	~CONTAINER(){};
	//Lectura de datos
	void LeerDatosContainer(instancia, int *, int);
	//Escritura de datos
	void EscribirDatosProgramaDibujo();
	//Funciones Get
	int Get_X(){return m_X;};
	int Get_Y(){return m_Y;};
	int Get_Z(){return m_Z;};
	int Get_Volumen_Utilizado(){return m_total_volumen_ocupado;};
	int Get_Tipo_Mejora(){return m_tipo_mejora;};
	void Set_Val_Quita_Aleatorio(bool a){m_val_quita_aleatorio=a;};
	void SetMuchasIteraciones(bool a){m_muchas_iteraciones=a;};
	int Get_Volumen(){return m_total_volumen;};
	int Get_TamRCL(){return m_TamRCL;};
	std::list <CONFIGURACAO> &Get_ListaConfiguracaos(){return m_Configuracaos;};
	std::list <BLOQUE> &Get_ListaBloques(){return m_Bloques;};
	std::list <CONFIGURACAO> &Get_BestListaConfiguracaos(){return m_Best_Configuracao;};
	std::list < CONFIGURACAO > & Get_ListaRCL(){return m_ListaRCL;};
	std::vector < PIECE > &Get_Piezas(){return m_Pieces;};
	bool Get_Aleatorizado(){return m_Aleatorizado;};
	bool Get_Full_Supported(){return m_full_supported;};
	void Set_Tipo_Origen(int  a){m_tipo_origen=a;};
	int Get_Tipo_Origen(){return m_tipo_origen;};
	bool Get_objetivo_vol(){return m_objetivo_vol;};
	void Set_m_cambiar_cliente(bool a){m_cambiar_cliente=a;};
	bool Get_m_cambiar_cliente(){return m_cambiar_cliente;};
	short int Get_Tipo_Gravedad(){return m_tipo_gravedad;};
	void  Set_Tipo_Gravedad(short int a){m_tipo_gravedad=a;};
	void Set_Eleccion_Space(short int a){m_tipo_eleccion_space=a;};
	void Set_Full_Suported(bool a){m_full_supported=a;};
	void Set_Algoritmo_Opcion(int a){Algoritmo_Opcion=a;};
	void Set_Algoritmo_Opcion_Mejora(int a){Algoritmo_Opcion_Mejora=a;};
	void Set_Algoritmo_Arquivo_Imprimir(int a){Algoritmo_Arquivo_Imprimir=a;};
	void Set_Algoritmo_Opcion_Construccion(int a){Algoritmo_Opcion_Construccion=a;};
	void Set_Algoritmo_Opcion_Irrestricto(bool a){Algoritmo_Opcion_Irrestricto=a;};
	void Set_Algoritmo_Opcion_Ceschia(bool a){Algoritmo_Opcion_Ceschia=a;};
	void Set_Algoritmo_Opcion_Daneses(bool a){Algoritmo_Opcion_Daneses=a;};
	void Set_Algoritmo_Opcion_Junqueira(bool a){Algoritmo_Opcion_Junqueira=a;};
	void Set_Algoritmo_Opcion_Liu(bool a){Algoritmo_Opcion_Liu=a;};
	void Set_Algoritmo_Opcion_Pesos_Sin_Multidrop(bool a){Algoritmo_Opcion_Pesos_Sin_Multidrop=a;};
	int Get_Algoritmo_Opcion(){return Algoritmo_Opcion;};
	int Get_Algoritmo_Opcion_Mejora(){return Algoritmo_Opcion_Mejora;};
	int Get_Algoritmo_Arquivo_Imprimir(){return Algoritmo_Arquivo_Imprimir;};
	int Get_Algoritmo_Opcion_Construccion() {return Algoritmo_Opcion_Construccion;};
	bool Get_Algoritmo_Opcion_Irrestricto(){return Algoritmo_Opcion_Irrestricto;};
	bool Get_Algoritmo_Opcion_Ceschia(){return Algoritmo_Opcion_Ceschia;};
	bool Get_Algoritmo_Opcion_Daneses(){return Algoritmo_Opcion_Daneses;};
	bool Get_Algoritmo_Opcion_Junqueira(){return Algoritmo_Opcion_Junqueira;};
	bool Get_Algoritmo_Opcion_Liu(){return Algoritmo_Opcion_Liu;};
	bool Get_Algoritmo_Opcion_Pesos_Sin_Multidrop(){return Algoritmo_Opcion_Pesos_Sin_Multidrop;};
	void SetValorQuita(double a){m_val_quita=a;};
	//Funciones set
	void SetNombre(char *name){strcpy(m_Nombre,name);};
	void ElegirSpaceConstruccion(SPACE &, bool &posi);
	//**************Para dibujar
	void SetTipoMejora(int a){m_tipo_mejora=a;};
	void Setm_num_total_clientes(int a){m_num_total_clientes=a;};
	int  Getm_num_total_clientes(){return m_num_total_clientes;};
	void Set_m_veces_filtrar(int a){m_veces_filtrar=a;};
	int  Get_m_veces_filtrar(){return m_veces_filtrar;};
	void SetObjetivoCapas(bool a){m_objetivo_capas=a;};
	void SetDibujar(bool a){m_Dibujar=a;};
	void SetPasoaPaso(bool a){m_paso_a_paso=a;};
	void SetFactorDibujar(double factor){m_factor=factor;};
	void SetGrados(double a){m_grados=a;};
	void SetTipoMixto(bool a){m_tipo_mixto=a;};
	//void SetClienteDC(CClientDC *d){m_clientedc=d;};
	void SetMejoraLocal(bool a){m_mejora_local=a;};
	void Set_m_mejora_local_cliente(bool a){m_mejora_local_cliente=a;};
	void Set_m_tipo_evaluacion(int a){m_tipo_evaluacion=a;};
	void DibujarSolucion(std::list< SPACE > &lista_espacios, std::list < CONFIGURACAO > &lista_configuracaos);
	void DibujarBloques(std::list< SPACE > &,std::list < CONFIGURACAO > &);
	void DibujarCaja3D(int x1,int y1, int z1, int x2, int y2, int z2,int,bool );
	void  EscribirSolucion();
	void EliminarSpace(std::list < SPACE > &lista, SPACE &eSpace);
	void EliminarSpaceFlag(std::list < SPACE > &lista, SPACE &eSpace);
	bool IncluidoFlag(SPACE espacio,std::list < SPACE > lista_espacio);

	void ActualizarEspaciosPorCambioCliente(std::list < SPACE > &lista_espacio, std::list < CONFIGURACAO > lista_configuracao);
	void ActualizarEspaciosPorCambioCliente_Ceschia(std::list < SPACE > &lista_espacio, std::list < CONFIGURACAO > lista_configuracao);
	void ActualizarEspaciosPorCambioCliente_Daneses(std::list < SPACE > &lista_espacio, std::list < CONFIGURACAO > lista_configuracao);
	void ActualizarEspaciosPorCambioCliente_Liu(std::list < SPACE > &lista_espacio, std::list < CONFIGURACAO > lista_configuracao);
	void ActualizarEspaciosPorCambioCliente_Junqueira(std::list < SPACE > &lista_espacio, std::list < CONFIGURACAO > lista_configuracao);
	void ActualizarEspaciosPorDelta_Junqueira(std::list < SPACE > &lista_espacio, std::list < CONFIGURACAO > lista_configuracao,int cliente);
	void ActualizarEspaciosPorCambioClienteDELTA_Junqueira(std::list < SPACE > &lista_espacio, std::list < CONFIGURACAO > lista_configuracao,int cliente);
	void ActualizarEspaciosPorAlcanzabilidad_Liu(std::list < SPACE > &lista_espacio);

	void EscribirMejorSolucionOpenGL(std::list<CONFIGURACAO> lista);
	int DibujarOpenGL(std::list<CONFIGURACAO> lista  );
	//Para la nueva cosa de juntar piezas igualitas
	void PuedoJuntar(int pieza1, int pieza2, int ancho1, int largo1, int alto1, int ancho2, int largo2);
	void PuedoJuntarParecidas(int pieza1, int pieza2, int ancho1, int largo1, int alto1, int ancho2, int largo2,short int tipo1,short int tipo2);
//---------------------------------------------------------------------------
	void PuedoJuntarParecidasPesos(int pieza1, int pieza2, int ancho1, int largo1, int alto1, int ancho2, int largo2,short int tipo1,short int tipo2, int peso1, int peso2, int soporte1, int soporte2, float densidad1, float densidad2);
	void JuntarPiezasParecidasPesos();
//---------------------------------------------------------------------------
	void JuntarPiezasParecidas();

	void CambiarConfiguracaoBloque(CONFIGURACAO &C, SPACE &E, BLOQUE &B, CONFIGURACAO &C1, CONFIGURACAO &C2, std::list < SPACE > &lista);


	//******Funcion que me dice si cabe alguna pieza
	bool CabeAlguna(int dx,int dy,int dz);
	//me dice si cabe alguna con esta orientacion
	int EstimacionConEstaOrientacionAlguna(int dx,int dy,int dz, int x, int y,int z, int Id,int piezas_maximas);
	//Esta tiene que sumar por cuantas cajas de media esta soportada cada caja 
	//que no esta en el suelo
	double Medida1Estabilidad(std::list<CONFIGURACAO> &lista);
	
	//Cuanta el numero de cajas que soportan esta caja
	int NumeroDeCajasSoportanEsta(int x1,int y1, int z1, int x2, int y2, int z2,std::list<CONFIGURACAO> &lista);	
	int EstaCajaSoportada(int x1,int y1, int z1, int x2, int y2, int z2,std::list<CONFIGURACAO> &lista,int cantidad);

	///**** Calcula la menor dimension para un espacio

	void MinimaDimension_Ceschia();

	void MinimaDimensionCliente();


	//Esta mide el número medio de cajas que tiene 3 lados pegados a otras cajas
	//Por tanto lo primero recorrer todas las cajas
	double Medida2Estabilidad(std::list<CONFIGURACAO> &lista);
	int EstaCajaTiene3Pegados(int x1,int y1, int z1, int x2, int y2, int z2,std::list<CONFIGURACAO> &lista);


	//Compara dos conjuntos devuelve true si el primero es mejor que el segundo
	bool Mejor_DistA_Que_DistB_Espacios(ORIG &A, int VolA,ORIG &B, int VolB);
	//DEvuelve true si es mejor segun el orden de los vertices
	bool Mejor_DistA_Que_DistB_Origen(ORIG &A, int VolA,ORIG &B, int VolB);
	//Una vez que tiene la mejor solucion la intenta mover hacia un eje
	void CompactarConfiguracaos(std::list<CONFIGURACAO> &lista);
	bool  FuerzaDeGravedad(std::list<CONFIGURACAO> &,short int sentido);
	bool FuerzaDeGravedadIndividual(std::list<CONFIGURACAO> &,short int sentido);

	int PuedoMover(std::list<CONFIGURACAO> &,int,int,int,int,int,int,short int);
	//Para que si empiezo desde uno inicialize las variables
	void InicializarVariables(void);
	//cOSAS DE LOS bloques
	//BLOQUE  &CONTAINER::BuscarPiezasBloque(int );
	void ActualizarListasBloque(SPACE &EspacioT, CONFIGURACAO &Configuracao);
	void EliminarBloquesConEstaPieza(int kblo);
	void ReducirBloquesConEstaPieza(int kblo,int cuantos);


	bool Mejor_DistA_Que_DistB(SPACE & , SPACE &);
	void ElegirConfiguracao(SPACE &,CONFIGURACAO &);
	void ElegirConfiguracao_Pesos_Sin_Multidrop(SPACE &,CONFIGURACAO &);
	void ElegirConfiguracao(SPACE &,CONFIGURACAO &,int cliente);
	void ElegirConfiguracao_Ceschia(SPACE &,CONFIGURACAO &,int cliente);
	void ElegirConfiguracao_Daneses(SPACE &,CONFIGURACAO &,int cliente);
	void ElegirConfiguracao_Liu(SPACE &,CONFIGURACAO &,int cliente);
	void ElegirConfiguracao_Junqueira(SPACE &,CONFIGURACAO &,int cliente);



	SPACE &ElegirSpace();

	void Constructivo_Ceschia(bool);



	bool Grasp_Ceschia();
	


	void Mejora_Cliente_Ceschia(std::vector <bool> &ya_mejorado_cliente,int cliente);

	void MejoraLocalPeso_Ceschia(bool fobjetivo);
	void MejoraLocalFINALPeso_Ceschia(bool fobjetivo);



	void DistintasEvaluaciones(std::list< CONFIGURACAO > m_Configuracaos);
	void ReactiveDeltaPaso1(std::vector <int > &vec_veces, std::vector <double> &vec_valor,std::vector <double> &vec_buenas);
	void Filtrodecajas(std::vector <int > &v1);
	void ReactiveDeltaPaso2(std::vector <int > &vec_veces, std::vector <double> &vec_valor);
	void ActualizarConfiguracaoGravedad(CONFIGURACAO &E, int x1,int y1, int z1, int x2, int y2, int z2,std::list <CONFIGURACAO> &lista_temp, short int sentido);
	void ElegirSpace(SPACE &espacio, bool &posi);
	void ElegirSpace_Ceschia(SPACE &espacio, bool &posi);
	void ElegirSpace_Daneses(SPACE &espacio, bool &posi);
	void ElegirSpace_Pesos_Sin_Multidrop(SPACE &espacio, bool &posi);
	void ElegirSpace_Liu(SPACE &espacio, bool &posi);
	void ElegirSpace_Junqueira(SPACE &espacio, bool &posi);

	void ActualizarListas_Ceschia(SPACE &Espacio, CONFIGURACAO &Configuracao);
	void ActualizarListas_Daneses(SPACE &Espacio, CONFIGURACAO &Configuracao);
	void ActualizarListas_Pesos_Sin_Multidrop(SPACE &Espacio, CONFIGURACAO &Configuracao);
	void ActualizarListas_Liu(SPACE &Espacio, CONFIGURACAO &Configuracao);
	void ActualizarListas_Junqueira(SPACE &Espacio, CONFIGURACAO &Configuracao);

	void ActualizarEspaciosElegido_Ceschia(SPACE &E, CONFIGURACAO &C, std::list< SPACE > &);
	void ActualizarEspaciosElegido_Daneses(SPACE &E, CONFIGURACAO &C, std::list< SPACE > &);
	void ActualizarEspaciosElegido_Pesos_Sin_Multidrop(SPACE &E, CONFIGURACAO &C, std::list< SPACE > &);
	void ActualizarEspaciosElegido_Liu(SPACE &E, CONFIGURACAO &C, std::list< SPACE > &);


	//Para insertalos en la lista ordenada
	void ColocarOrdenadosNuevos(std::list< SPACE > &);
	void InsertarOrden(SPACE &Espacio);
	void InsertarOrden(std::list < SPACE > &,SPACE &Espacio);
	void InsertarOrdenFlag(std::list < SPACE > &,SPACE &Espacio);
	void EliminarInclusiones();
	void EliminarInclusionesNuevos(std::list < SPACE > &Lista);
	//Compara todos los de la lista con lo que ya hay metidos para ver si hay alguna inclusión
	void EliminarInclusionesListaAntigua(std::list < SPACE > &Lista);
	void EliminarListaTempListaOriginal(std::list <SPACE > &ListaOriginal,std::list <SPACE > &Lista);

	//Funcion que verifica varias cosas de la solucion
	void VerificarSolucion(std::list<CONFIGURACAO> &lista);
	//Funcion que comprueba si dos configuracaos intersectan es para verificar
	bool ComprobarConfiguracaosNoIntersectan(std::list<CONFIGURACAO> &lista);
	//Funcion que mira si se cortan las configuracaos y los espacios vacios
	bool ComprobarConfiguracaosNoIntersectanSpaces();
	//Comprobar que todos los bloques son posibles
	bool ComprobarTodosPosible(std::list<CONFIGURACAO> &lista);
	//Estimacion de cuantas caben
	int EstimacionConEstaOrientacion(SPACE &Espacio, int x, int y,int z, int Id,int piezas_maximas);
	int EstimacionConEstaOrientacionPeso(SPACE &Espacio, int x, int y,int z, int Id,int piezas_maximas, int fuerza);
	int EstimacionConEstaOrientacionPeso_Ceschia(SPACE &Espacio, int x, int y,int z, int Id,int piezas_maximas, int fuerza);
	//Estimacion 
	int Estimacion(SPACE &Espacio);
	int EstimacionPesoCliente(SPACE &Espacio,int cliente);
	int EstimacionCliente(SPACE &Espacio, int cliente);
	int EstimacionPesoCliente_Ceschia(SPACE &Espacio,int cliente);
	int EstimacionPeso(SPACE &Espacio);
	//Método de la burbuja para ordenar
	void BubbleSort(std::list<CONFIGURACAO> &lista,short int tipo) ;
	void  PintarSolucion(std::list<CONFIGURACAO> &lista);
	void EscribirSolucionLatex(std::list<CONFIGURACAO> lista);
	//Le doy una configuracao y me reconstruye la solucion para
	//tener los espacios maximales otra vez

	void PonerConfiguracao(SPACE &Espacio,CONFIGURACAO &Configuracao,std::list<CONFIGURACAO> &lista,bool );

	//Para la otra distancia
	bool Mejor_DistA_Que_DistB_Espacios_Euclidea(int A, int VolA,int B, int VolB);

	//Funciones necesarias en el tabu
	//**********************
	void Tabu();
	//
	void InsertarOrden( std::list< CONFIGURACAO >  &Lista, CONFIGURACAO  &nuevo);


	//elegir space construciion
	SPACE & ElegirSpaceConstruccion();
	MOV_AUMENTAR & ElegirMejorBloqueAumentar(SPACE &);
	void EscribirMejorSolucion(std::list<CONFIGURACAO> lista);
	void ConstruirSolucionGuardada();
	void ActualizarBloquesElegido( CONFIGURACAO &Vieja,CONFIGURACAO &Nueva,std::list <CONFIGURACAO> &lista_temp);
	void ActualizarListaDeBloques(bool,CONFIGURACAO &Configuracao,std::list<CONFIGURACAO> &lista);
	void verificarsoporte(std::list<CONFIGURACAO> lista);
	void VerificarPlanosSoporte(std::list<CONFIGURACAO> lista);


};


//*********************
// CLASE PIEZA
//********************
class PIECE{
	int m_Id; //numero para identificar la pieza
	int m_x; //Largo tablero
	bool m_Rotate_x; //SE puede rotar en dirección vertical en el eje x
	int m_y; //Ancho tablero
	bool m_Rotate_y; //Se puede rotar en dirección en el eje y
	int m_z;
	bool m_Rotate_z; //Se puede rotar en dirección en el eje z.
	int m_q; //Numero de piezas
	//Siempre alguno es posible
	//Puntero estatico al Contenedor
	static CONTAINER *m_Container;
	int m_peso_pieza;//----------------------------------------------
	int m_S_x;
	int m_S_y;
	int m_S_z;
	int m_cliente;
	int m_Costo;
	int m_Volumen; //Volumen de la pieza
	float m_densidad_pieza;
	int m_num; //numero de piezas utilizado
public:
	//constructor

	PIECE(int,int, bool, int , bool, int ,bool , int ,CONTAINER *a,int,int, int, int, int,int);


	PIECE(){};
	//destructor
	~PIECE(){};
	//bool PIECE::operator < ( const PIECE& p2);

	//Funciones set
	void Set_Id(int a){m_Id=a;};
	void Set_x(int a){m_x=a;};
	void Set_y(int a){m_y=a;};
	void Set_z(int a){m_z=a;};
	void Set_m_cliente(int a){m_cliente=a;};
	void Set_Num(int a){m_num=a;};
	void Set_m_peso_pieza(int a){m_peso_pieza=a;};//--------------------------------
	void Set_Cliente(int a){m_cliente=a;};//--------------------------------
	void Set_Costo(int a){m_Costo=a;};//--------------------------------
	void Set_m_densidad_pieza(float a){m_densidad_pieza=a;};//--------------------------------
	//Funciones Get
	int Get_Id(){return m_Id;};
	int Get_x(){return m_x;};
	int Get_y(){return m_y;};
	int Get_z(){return m_z;};
	int Get_Cliente(){return m_cliente;};
	bool Get_Rx(){return m_Rotate_x;};
	bool Get_Ry(){return m_Rotate_y;};
	bool Get_Rz(){return m_Rotate_z;};
	int Get_Sx(){return m_S_x;};
	int Get_Sy(){return m_S_y;};
	int Get_Sz(){return m_S_z;};
	int Get_m_peso_pieza(){return m_peso_pieza;};//--------------------------
	int Get_Costo(){return m_Costo;};//--------------------------
	float Get_m_densidad_pieza(){return m_densidad_pieza;};//--------------------------

	int Get_Num(){return m_num;};
	int Get_q(){return m_q;};
	int Get_Volumen(){return m_Volumen;};
	void Add_Num(int a){m_num+=a;};
};
#endif
