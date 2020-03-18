//Librería para el empaquetamiento en el contenedor


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
#include <time.h>
#include <stdio.h>
//#include <GL/glut.h>

#ifndef Container_H
 #include "Container.h"
#endif

using namespace std;



bool mainCLP(instancia caso, int * ruta, int tamanioRuta, bool banderaDibujado){
	//Declaración de variables
	bool cabe;
	
	
	//Variables de tiempo
	//si es 1 es full_supported
	//cout << "Paso" << endl;
	CONTAINER Container(caso,ruta,tamanioRuta);
	Container.SetValorQuita(0.5);
	bool parametro_opcion_algoritmo=true;	//full support
	int parametro_opcion=3;				//que autor
	int parametro_opcion_construccion=3;//que tipo de construccion
	int parametro_mejoras=1;           //que mejorar realizar   
	int parametro_arquivo_imprimir=1; //archivo para imprimir
	Container.Set_Full_Suported(true);
	Container.Set_Algoritmo_Opcion(parametro_opcion);
	Container.Set_Algoritmo_Opcion_Construccion(parametro_opcion_construccion);
	Container.Set_Algoritmo_Opcion_Mejora(parametro_mejoras);
	Container.Set_Algoritmo_Arquivo_Imprimir(parametro_arquivo_imprimir);

	Container.Set_Val_Quita_Aleatorio(false);
	
	Container.Set_Algoritmo_Opcion_Irrestricto(false);
	if(Container.Get_Algoritmo_Opcion_Irrestricto())
	{
		if (Container.Getm_num_total_clientes()<50)
		{
			Container.Set_m_veces_filtrar(Container.Getm_num_total_clientes()* 5);
		}
		else
		{
			Container.Set_m_veces_filtrar(50 * 5);
		}
	}
	Container.SetTipoMejora(2);
	Container.SetTipoMixto(false);
	Container.SetObjetivoCapas(true);
	switch(parametro_mejoras)
	{
		case 1:
		{
			Container.SetMejoraLocal(false);
			Container.Set_m_mejora_local_cliente(false);
		}
		break;
		case 2:
		{
			Container.SetMejoraLocal(true);
			Container.Set_m_mejora_local_cliente(false);
		}
		break;
		case 3:
		{
			Container.SetMejoraLocal(false);
			Container.Set_m_mejora_local_cliente(true);
		} 
		break;
		case 4:
		{
			Container.SetMejoraLocal(true);
			Container.Set_m_mejora_local_cliente(true);
		} 
		break;
	}	
		
	Container.SetMuchasIteraciones(false);
	Container.Set_Tipo_Origen(8);
	Container.Set_Val_Quita_Aleatorio(false);
//	Container.Set_Cadena_Salida(argv[1]);
	Container.Set_m_tipo_evaluacion(2);
	Container.Set_Algoritmo_Opcion_Daneses(false);
	Container.Set_Algoritmo_Opcion_Ceschia(false);
	if(parametro_opcion_algoritmo)		
		Container.Set_Algoritmo_Opcion_Junqueira(true); //TRUE -> Delta igual a cero 
	else
		Container.Set_Algoritmo_Opcion_Junqueira(false);

//cout << "Paso2" << endl;
 cabe = Container.Grasp_Ceschia();
//	Container.Constructivo_Ceschia(false);
	
	//Bandera de dibujado para mostrar patrón de empaquetamiento en pantalla
	if(banderaDibujado){
		//Container.DibujarOpenGL(Container.Get_BestListaConfiguracaos());
		
		Container.EscribirMejorSolucion(Container.Get_BestListaConfiguracaos());
		
		
	}
	
	
	
	
	
	
	//Retornar el valor de verdad
	return cabe;
	
}
