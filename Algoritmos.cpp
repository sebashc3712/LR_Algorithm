//#pragma warning (disable: 4786)

//Para incluir el contenedor que tengo
#include "Container.h"
#include <algorithm>    // std::min
#include <time.h>    // std::min
#include <iostream>
//#include <pgobootrun.h>

using namespace std;

void CONTAINER::EliminarSpace(std::list < SPACE > &lista, SPACE &eSpace)
{
	std::list < SPACE > ::iterator it;
	for (it = lista.begin();it!=lista.end();it++)
	{
		if ((*it).Get_x1()==eSpace.Get_x1() && (*it).Get_x2()==eSpace.Get_x2() &&
		(*it).Get_y1()==eSpace.Get_y1() && (*it).Get_y2()==eSpace.Get_y2() &&
		(*it).Get_z1()==eSpace.Get_z1() && (*it).Get_z2()==eSpace.Get_z2() ) 
		{
			lista.erase(it);
			break;
		}
	}
}
void CONTAINER::EliminarSpaceFlag(std::list < SPACE > &lista, SPACE &eSpace)
{
	std::list < SPACE > ::iterator it;
	for (it = lista.begin();it!=lista.end();it++)
	{
		if ((*it).Get_Flag()==false)
		{
			if ((*it).Get_x1()==eSpace.Get_x1() && (*it).Get_x2()==eSpace.Get_x2() &&
				(*it).Get_y1()==eSpace.Get_y1() && (*it).Get_y2()==eSpace.Get_y2() &&
				(*it).Get_z1()==eSpace.Get_z1() && (*it).Get_z2()==eSpace.Get_z2() ) 
			{
				(*it).Set_Flag(true);
				break;
			}
		}
	}
}
bool CONTAINER::IncluidoFlag(SPACE espacio,std::list < SPACE > lista_espacio)
{
	std::list<SPACE> ::reverse_iterator itl2;
	for (itl2=lista_espacio.rbegin();itl2!=lista_espacio.rend();itl2++)
	{
		if ((*itl2).Get_Flag()==false)
		{
			if (espacio.Get_Volumen()>(*itl2).Get_Volumen()) 
				return false;
			if (espacio.Incluido((*itl2))==true)
			{
				//Si esta incluido termina 
				return true;
			}
		}
	}
	return false;
}
void CONTAINER::DistintasEvaluaciones(std::list<CONFIGURACAO> lista)
{
	std::list< CONFIGURACAO > ::iterator itC;	
	m_evaluacion1=0;
	m_evaluacion2=0;
	for (itC=lista.begin();itC!=lista.end();itC++)
	{
		m_evaluacion1+=(*itC).Get_Volumen()*(((*itC).Get_x2()-(*itC).Get_x1())/2)+(*itC).Get_Volumen()*(((*itC).Get_x2()-(*itC).Get_x1())/2);
		m_evaluacion2+=(*itC).Get_x2()*(*itC).Get_Num();
	}
}
void CONTAINER::CompactarConfiguracaos(std::list<CONFIGURACAO> &lista)
{
//	VerificarSolucion();
	
//	PintarSolucion(lista);
	m_total_volumen_ocupado=m_max_total_volumen_ocupado;
	//fijo a que haga almenos una en cada sentido
	bool movido=true;
	short int sentido=0;
	//DibujarSolucion(m_Spaces,lista);

	movido=FuerzaDeGravedad(lista,sentido);
//	PintarSolucion(lista);
	//DibujarSolucion(m_Spaces,lista);
	m_Spaces.clear();
	m_Configuracaos=lista;

	sentido=1;
	movido=true;
	int parar=0;
	while (parar<3)
	{

//		printf("una");
		movido=FuerzaDeGravedad(lista,sentido);

		if (sentido==0)
			sentido=1;
		else
			sentido=0;
			if (sentido==1)
				
				sentido=2;
			else
				sentido=0;
			//DibujarSolucion(m_Spaces,lista);
		if (movido==false)
			parar++;
		else
		{
			parar=0;
			movido=false;
		}
	}
	m_Spaces.clear();
	m_Configuracaos=lista;

	//Una vez hecho esta paso a la individual
	sentido=0;
//	PintarSolucion(lista);
	movido=FuerzaDeGravedadIndividual(lista,sentido);
	m_Spaces.clear();
//	PintarSolucion(lista);

	sentido=1;
	movido=true;
	parar=0;
	while (parar<3)
	{

//	PintarSolucion(lista);
//		printf("una");
		movido=FuerzaDeGravedadIndividual(lista,sentido);

		if (sentido==0)
			sentido=1;
		else
		
			if (sentido==1)
				
				sentido=2;
			else
				sentido=0;
		//DibujarSolucion(m_Spaces,lista);
	m_Spaces.clear();
//	PintarSolucion(lista);
	m_Configuracaos=lista;
//	PintarSolucion(lista);

		if (movido==false)
			parar++;
		else
		{
			parar=0;
			movido=false;
		}


	}

//	printf("\n");
	m_Spaces.clear();
	m_Configuracaos=lista;

	//si quiero hacerla en las tres direcciones



}

//al principio
void CONTAINER::ReactiveDeltaPaso1(std::vector <int > &vec_veces, std::vector <double> &vec_valor,std::vector <double> &vec_buenas)
{
	double total=0;
	int alea=0;
	//A partir de que valor empieza es el inicio 
	//esta más para abajo he puesto de 1 a 9
	if (m_iter_actual%500==0 && m_iter_actual>0)
	{
		total=0;
		m_hastavalor=0;
		//Calculo las medias para tener todos los posibles valores
		for (int ix=1;ix<10 ;ix++)
		{	
			if (vec_veces[ix]!=0 )
			{
				//Tengo la media de lo obtenido para el Delta ix		
				double meanix=((double)vec_valor[ix]/(double)vec_veces[ix]);
				//Tengo la suma de todas las medias 
				total+=pow((double)(meanix-m_PeorSolucion_volumen)/(double)(m_max_total_volumen_ocupado-m_PeorSolucion_volumen),10);
						
			}
		}
		for (int ix=1;ix<10 ;ix++)
		{
			if (vec_veces[ix]!=0 )
			{
				//Tengo un porcentaje de los bueno que es 
				double meanix=((double)vec_valor[ix]/(double)vec_veces[ix]);
				double evalix=pow((double)(meanix-m_PeorSolucion_volumen)/(double)(m_max_total_volumen_ocupado-m_PeorSolucion_volumen),10);
				vec_buenas[ix]=((double)evalix/(double)total)*100;
				//	
				m_hastavalor+=vec_buenas[ix];
			}
		}
		//escribe lo que ha hecho
//		printf("\n");
//		for (ix=1;ix<10;ix++)
//		{
//			double meanix=(((double)vec_valor[ix]/(double)vec_veces[ix])*100)/(double)m_total_volumen;
//
//			printf("%d %d %f %f\n",ix,vec_veces[ix],vec_buenas[ix],meanix);
//		}
	}

	if (m_iter_actual<500)
	{
		alea=get_random(1,9);
		m_Delta=(double)alea/(double)10;

	}
	else
	{
		alea=get_random(1,m_hastavalor);
		//busco cual es el que corresponde
		int suma=0;
		for (int ix=1;ix<10 && suma<alea;ix++)
		{	
			suma+=vec_buenas[ix];
			m_Delta=(double)ix/(double)10;
		}
	}

}
//Actualizar los valores de cada indice
void CONTAINER::ReactiveDeltaPaso2(std::vector <int> &vec_veces,std::vector <double> &vec_valor)
{
	int val=m_Delta*10;
	vec_veces[val]++;
	vec_valor[val]+=m_total_volumen_ocupado;
	//Actualizo la peor solucion obtenida
	if (m_total_volumen_ocupado<m_PeorSolucion_volumen)
		m_PeorSolucion_volumen=m_total_volumen_ocupado;
	
	//if (m_total_costo_ocupado<m_PeorSolucion_volumen)
	//	m_PeorSolucion_volumen=m_total_costo_ocupado;
}
//Le doy una configuracao y me reconstruye la solucion para
//tener los espacios maximales otra vez

//notar que son casos diferentes si esta compactada o no es mas facil si no esta compactada
//para que funcione correctamente compacta tiene que ser compactada en las tres dimensiones
//si no puede existir una pieza que vaya en medio de los espacios que no haya 
//empezado en ningun sitio si lo hago en X e Y pero en Z

//Si solamente tengo en cuenta las esquinas va a ver ocasiones en que no la pueda volver
//a reproducir aunque este compactada en ese caso tambien incluyo si esta incluida en alguna
//Filtro de cajas por cliente
void CONTAINER::Filtrodecajas(std::vector <int> &v1)
{
	double total=0;
	int alea=0;
	//A partir de que valor empieza es el inicio 
	//esta más para abajo he puesto de 1 a 9
	total=0;
	std::vector< double > vec_probabildades(m_num_total_clientes);
	int maximo=0;
	//Calculo las medias para tener todos los posibles valores
	for (int ix=0;ix<m_num_total_clientes ;ix++)
	{	
		//Tengo la media de lo obtenido para el Delta ix		
		vec_probabildades[ix]=(m_volumen_ocupado_cliente[ix]/m_superficie_cliente[ix])/1000000;
		if (vec_probabildades[ix]>maximo)
			maximo=vec_probabildades[ix];
	}
	maximo++;
	for (int ix=0;ix<m_num_total_clientes ;ix++)
	{	
		//Tengo la media de lo obtenido para el Delta ix		
		vec_probabildades[ix] = maximo-vec_probabildades[ix];
		total+=vec_probabildades[ix];
	}

	
	double volumen_quitado=0;
	volumen_quitado=((double) get_random(30,70)/(double)100)*
		(m_total_volumen-m_max_total_volumen_ocupado);
	while(true)
	{
		int numero_aleatorio=get_random(0,total);
		double total_temp=0;
		int filtrar_cliente=0;
		for (int ix=0;ix<m_num_total_clientes ;ix++)
		{	
				total_temp+=vec_probabildades[ix];
				if (numero_aleatorio<=total_temp)
				{
					//Podría darse el caso que quitar de un solo cliente no sirva para nada
					//habría que quitar más de uno, pero ¿cómo? y ¿cómo actualizarlo?
					filtrar_cliente=ix;
					break;
				}
		}
		std::vector <int>::iterator Iter1;
		int quito=0;
		random_shuffle(v1.begin(),v1.end());
		//printf("Voy a quitar:\n");
		for (Iter1=v1.begin();Iter1!=v1.end() && volumen_quitado>0;Iter1++)
		{
			int pieza=(*Iter1);
			if(m_Pieces[pieza].Get_Cliente()==filtrar_cliente)
			{
	//				m_Pieces[pieza].Add_Num(m_Pieces[pieza].Get_q());
				//quito toda la pieza o el máximo para que este dentro de los valores de la pieza
				quito=min((volumen_quitado/m_Pieces[pieza].Get_Volumen())+1,(double)m_Pieces[pieza].Get_q());
				//m_Pieces[pieza].Set_Num(m_Pieces[pieza].Get_q());
				m_Pieces[pieza].Set_Num(quito);
				//printf("Del Cliente %d El tipo de pieza %d -> %d Ejemplares\n",filtrar_cliente,pieza,quito);
				volumen_quitado-=m_Pieces[pieza].Get_Volumen();
			}
		}
		if(volumen_quitado<=0)
			break;
		else
		{
			total-=vec_probabildades[filtrar_cliente];
			if(total<=0)
				break;
			vec_probabildades[filtrar_cliente]=0;
		}
	}

/*
	for (int ix=0;ix<m_num_total_clientes ;ix++)
	{	
			//Tengo la media de lo obtenido para el Delta ix	
			double meanix=(m_volumen_ocupado_cliente[ix]/m_superficie_cliente[ix]);
			if (meanix>maximo)
				maximo=meanix;
	}
	maximo++;
	for (int ix=0;ix<m_num_total_clientes ;ix++)
	{	

			//Tengo la media de lo obtenido para el Delta ix		
			double meanix=(m_volumen_ocupado_cliente[ix]/m_superficie_cliente[ix]);
			double meanixnor=maximo-meanix;
			total+=meanixnor;
	}
	int numero_aleatorio=get_random(0,total);
	double total_temp=0;
	int filtrar_cliente=0;
	double volumen_quitado=0;
	volumen_quitado=((double) get_random(30,70)/(double)100)*
		(m_total_volumen-m_max_total_volumen_ocupado);
	for (int ix=0;ix<m_num_total_clientes ;ix++)
	{	

			//Tengo la media de lo obtenido para el Delta ix		
			double meanix=(m_volumen_ocupado_cliente[ix]/m_superficie_cliente[ix]);
			double meanixnor=maximo-meanix;
			total_temp+=meanixnor;
			if (numero_aleatorio<=total_temp)
			{
				//Podría darse el caso que quitar de un solo cliente no sirva para nada
				//habría que quitar más de uno, pero ¿cómo? y ¿cómo actualizarlo?
				filtrar_cliente=ix;
				
				break;
			}
	}
	
	std::vector <int>::iterator Iter1;
	
	int quito=0;
	random_shuffle( v1.begin( ), v1.end( ) );
	//printf("Voy a quitar:\n");
	for (Iter1=v1.begin();Iter1!=v1.end() && volumen_quitado>0;Iter1++)
	{
		int pieza=(*Iter1);
		if(m_Pieces[pieza].Get_Cliente()==filtrar_cliente)
		{
//				m_Pieces[pieza].Add_Num(m_Pieces[pieza].Get_q());
			//quito toda la pieza o el máximo para que este dentro de los valores de la pieza
			quito=__min((volumen_quitado/m_Pieces[pieza].Get_Volumen())+1,m_Pieces[pieza].Get_q());
			//m_Pieces[pieza].Set_Num(m_Pieces[pieza].Get_q());
			m_Pieces[pieza].Set_Num(quito);
			//printf("Del Cliente %d El tipo de pieza %d -> %d Ejemplares\n",filtrar_cliente,pieza,quito);
			volumen_quitado-=m_Pieces[pieza].Get_Volumen();
		}
	}
*/
}
///CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA
///CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA
///CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA CESCHIA

bool CONTAINER::Grasp_Ceschia()
{
	m_inicio=clock();
	srand(1);
	int veces_mejora=0;
	for (int ikl=0;ikl<m_Q;ikl++)
	{
		m_Pieces_Mejora.push_back(0);
		m_Pieces_Mejora_cliente.push_back(0);
		copia_m_Pieces_Mejora_cliente.push_back(0);
	}
	m_PeorSolucion_volumen=999999999;
	m_Delta=0.1;
	m_max_iteraciones_Grasp=m_Q*50;
		
	//Valores para el reactive
	//Guardo las veces que ha pasado
	//Variables para el reactive
	std::vector <int> v1;
	int i;
	for ( i = 0 ; i < m_Q ; i++ )
		v1.push_back( i );
	std::vector< int > vec_veces(10);
	//Los valores
	std::vector< double > vec_valor(10);
	//Las buenas
	std::vector< double > vec_buenas(10);
	m_hastavalor=0;
	m_time_final=30;
	int umbral=0;
	int niter=0;
	m_Bloques_Guardados=m_Bloques;
	std::list< CONFIGURACAO> Mejor_Lista_Configuracao;
	int kk=0;
	MinimaDimensionCliente();
	m_fintime=clock();
	m_tiempo=((double)(m_fintime-m_inicio))/CLOCKS_PER_SEC;
	
	for (m_iter_actual=0;((m_iter_actual<=m_max_iteraciones_Grasp)&&(m_max_total_volumen_ocupado<m_total_volumen));m_iter_actual++)
	{
		m_objetivo_vol=true;
		m_Bloques=m_Bloques_Guardados;
		//Dejar todo limpio
		m_Spaces.clear();
		m_Configuracaos.clear();
		m_total_volumen_ocupado=0;
		if(Get_Algoritmo_Opcion_Ceschia())
			m_total_costo_ocupado=0;
		for (int j=0;j<m_Q;j++)
			m_Pieces[j].Set_Num(0);
		MinimaDimension_Ceschia();

		//Ajusta parámetros de reactive
		ReactiveDeltaPaso1(vec_veces,vec_valor,vec_buenas);
		if(Get_Algoritmo_Opcion_Irrestricto()&&(m_iter_actual%500<m_veces_filtrar)&&m_iter_actual>500)
		{
			Filtrodecajas(v1);
		}
		m_objetivo_capas=true;
		//cout << "Paso3" << endl;
		Constructivo_Ceschia(true);
		//cout << "Paso4" << endl;
		Mejor_Lista_Configuracao=m_Configuracaos;
		if(Get_Algoritmo_Opcion_Ceschia())
		{
			if (m_total_costo_ocupado>m_max_total_costo_ocupado)
			{
				m_max_total_costo_ocupado=m_total_costo_ocupado;
				m_Best_Configuracao=m_Configuracaos;
			}
		}
		else
		{
			if (m_total_volumen_ocupado>m_max_total_volumen_ocupado)
			{
				m_max_total_volumen_ocupado=m_total_volumen_ocupado;
				m_Best_Configuracao=m_Configuracaos;
			}
		}
		//Busqueda Local
		ReactiveDeltaPaso2(vec_veces,vec_valor); 
		if (m_mejora_local==true)
		{
				//Si la solución es muy buena lo hago en otro caso NO
			if(Get_Algoritmo_Opcion_Ceschia())
			{	
				int temp=m_total_volumen_ocupado;
				if (m_total_volumen_ocupado<(umbral))
				{
					niter++;
					if (niter==5)
					{
						umbral=umbral-0.2*(double)(1+umbral);
						niter=0;
					}
				}
				else
				{
					//este es unicamente de informacion
					veces_mejora++;
					//Si pasa ponemos este valor
					umbral=m_total_volumen_ocupado;
					niter=0;	
					//Mejora con orig
					if (m_tipo_mejora==0 ||m_tipo_mejora==2)
					{
						MejoraLocalFINALPeso_Ceschia(true);
						if (m_total_costo_ocupado>temp)
						{
							Mejor_Lista_Configuracao=m_Configuracaos;
							temp=m_total_costo_ocupado;
						}
						if (m_total_costo_ocupado>m_max_total_costo_ocupado)
						{
							m_max_total_costo_ocupado=m_total_costo_ocupado;
							m_Best_Configuracao=m_Configuracaos;
						}
					}
					if (m_tipo_mejora==0 ||m_tipo_mejora==2)
					{
						MejoraLocalFINALPeso_Ceschia(false);
						if (m_total_costo_ocupado>temp)
						{
							Mejor_Lista_Configuracao=m_Configuracaos;
							temp=m_total_costo_ocupado;
						}
						if (m_total_costo_ocupado>m_max_total_costo_ocupado)
						{
							m_max_total_costo_ocupado=m_total_costo_ocupado;
							m_Best_Configuracao=m_Configuracaos;
						}
					}
				}
			}
			else
			{
				int temp=m_total_volumen_ocupado;
				if (m_total_volumen_ocupado<(umbral))
				{
					niter++;
					if (niter==5)
					{
						umbral=umbral-0.2*(double)(1+umbral);
						niter=0;
					}
				}
				else
				{
					//este es unicamente de informacion
					veces_mejora++;
					//Si pasa ponemos este valor
					umbral=m_total_volumen_ocupado;
					niter=0;	
					if (m_tipo_mejora==0 ||m_tipo_mejora==2)
					{
						MejoraLocalFINALPeso_Ceschia(true);
						if (m_total_volumen_ocupado>temp)
						{
							Mejor_Lista_Configuracao=m_Configuracaos;
							temp=m_total_volumen_ocupado;
						}
						if (m_total_volumen_ocupado>m_max_total_volumen_ocupado)
						{
							m_max_total_volumen_ocupado=m_total_volumen_ocupado;
							m_Best_Configuracao=m_Configuracaos;
						}
					}
					if (m_tipo_mejora==0 ||m_tipo_mejora==2)
					{
						MejoraLocalFINALPeso_Ceschia(false);
						if (m_total_volumen_ocupado>temp)
						{
							Mejor_Lista_Configuracao=m_Configuracaos;
							temp=m_total_volumen_ocupado;
						}
						if (m_total_volumen_ocupado>m_max_total_volumen_ocupado)
						{
							m_max_total_volumen_ocupado=m_total_volumen_ocupado;
							m_Best_Configuracao=m_Configuracaos;
						}
					}
				}
			}
		}
		//m_fintime=clock();
		//m_tiempo=((double)(m_fintime-m_inicio))/CLOCKS_PER_SEC;
	}
	/*if (m_muchas_iteraciones==false)
	{
	//Medir estabilidad ahora
		m_fintime=clock();
		m_tiempo=((double)(m_fintime-m_inicio))/CLOCKS_PER_SEC;
		if (m_full_supported==false)
		{
			m_medida1=Medida1Estabilidad(m_Configuracaos);
			m_medida2=Medida2Estabilidad(m_Configuracaos);
		}
		else
		{
			m_medida1=Medida1Estabilidad(m_Best_Configuracao);
			m_medida2=Medida2Estabilidad(m_Best_Configuracao);
		}

		FILE *fin3;
			switch(Get_Algoritmo_Arquivo_Imprimir())
			{
				case 2:
					fin3=fopen("Resultados_2.txt","a+");
					break;
				case 3:
					fin3=fopen("Resultados_3.txt","a+");
					break;
				case 4:
					fin3=fopen("Resultados_4.txt","a+");
					break;
				case 5:
					fin3=fopen("Resultados_5.txt","a+");
					break;
				case 6:
					fin3=fopen("Resultados_6.txt","a+");
					break;
				case 7:
					fin3=fopen("Resultados_7.txt","a+");
					break;
				default:
					fin3=fopen("Resultados_1.txt","a+");
			}

			double pt=((double)m_max_total_volumen_ocupado/(double)(m_X*m_Y*m_Z))*100;
		fprintf(fin3,"%s\tV\t%d\tVU\t%d\t%f\tm1\t%f\tm100\t%f\tm55\t%f\tmCG\t%f\tm2\t%f\t%f\n",m_Nombre,m_total_volumen,m_max_total_volumen_ocupado,pt,m_medida1,m_medida100por100,m_medida60por100,m_medidaCG,m_medida2,m_tiempo);
		fclose(fin3);
	}*/
	if(m_total_volumen==m_max_total_volumen_ocupado){
		return true;
	}else{
		return false;
	}
}
void CONTAINER::Constructivo_Ceschia(bool aleatorizado)
{
	if (m_val_quita_aleatorio==true)
	{
		m_val_quita=get_random(25,75);
		m_val_quita=m_val_quita/100;
	}
	bool ya_mejorado=false;
	std::vector <bool> ya_mejorado_cliente;
	for(int j=0;j<m_num_total_clientes;j++)
		ya_mejorado_cliente.push_back(false);


	m_Aleatorizado=aleatorizado;
	//Inicialmente meto un espacio que es todo el contenedor
	SPACE SpaceElegido(0,0,0,m_X,m_Y,m_Z,min((m_X*m_Y*m_Z)+1,m_total_volumen+1),0,m_Y,m_X,this);
	m_Spaces.push_back(SpaceElegido);
	m_cambiar_cliente=false;
	int cliente=0;
	int copia_cliente;
	while (m_Spaces.size()>0)
	{	
		if(m_cambiar_cliente)
		{
				/*
///BUENO AQUI EMPIEZA TODO SOBRE MEJORA LOCAL POR CLIENTES!
///BUENO AQUI EMPIEZA TODO SOBRE MEJORA LOCAL POR CLIENTES!*/
			Mejora_Cliente_Ceschia(ya_mejorado_cliente,cliente);

			cliente++;
			m_total_volumen_ocupado_cliente=0;
			if (cliente>=m_num_total_clientes)
				return;
			else
			{
				m_cambiar_cliente=false;
				std::list <SPACE> ::reverse_iterator itSpace;
				for(itSpace=m_Spaces.rbegin();itSpace!=m_Spaces.rend();itSpace++)
				{
					if((*itSpace).Get_Flag()==true)
					{
						(*itSpace).Set_Flag(false);
					}
				}
				switch(Get_Algoritmo_Opcion())
				{
					case 1:   // CESCHIA
						ActualizarEspaciosPorCambioCliente_Ceschia(m_Spaces,m_Configuracaos);
						break;
					case 2:  // PESOS
						return;
						break;
					case 3:  // DANESES
						ActualizarEspaciosPorCambioCliente_Ceschia(m_Spaces,m_Configuracaos);
						break;
					case 4:  // LIU
					{
						ActualizarEspaciosPorCambioCliente_Ceschia(m_Spaces,m_Configuracaos);
						ActualizarEspaciosPorAlcanzabilidad_Liu(m_Spaces);
					}	break;
					case 5:  // JUNQUEIRA
					{
						if(Get_Algoritmo_Opcion_Junqueira()) //TRUE -> Delta igual a cero 
						{				
							ActualizarEspaciosPorDelta_Junqueira(m_Spaces,m_Configuracaos,cliente);
						}
						else
						{
							ActualizarEspaciosPorCambioCliente_Ceschia(m_Spaces,m_Configuracaos);
							ActualizarEspaciosPorCambioClienteDELTA_Junqueira(m_Spaces,m_Configuracaos,cliente);
						}
					}	break;
				}
			}
		}
		//Elegir espacio//esta función sale si no encuentra ningún espacio
		bool posible=false;	
		ElegirSpace_Ceschia(SpaceElegido,posible);
		if (SpaceElegido.Get_Flag()==true)
			int kk=9;
		if (m_Spaces.size()==0 || (posible==false))
		{
			if(cliente!=(m_num_total_clientes-1))
			{
				m_cambiar_cliente=true;
				cliente++;
				continue;
			}
			else
				return;
		}
		//si no eliminamos perdidas

		ORIG setmin(SpaceElegido.Get_dx(),SpaceElegido.Get_dy(),SpaceElegido.Get_dz());
		//Creo ya inicialmente la configuracao 
		CONFIGURACAO Configuracao(this,setmin,999999);

		//Elegir configuración para ese espacio
		ElegirConfiguracao_Ceschia(SpaceElegido,Configuracao,cliente);

		if (cliente<(m_num_total_clientes-1))
		{
			if (Configuracao.Get_Id()==99999)
			{
				EliminarSpaceFlag(m_Spaces,SpaceElegido);
				continue;
			}
		}
		else
		{
			if (Configuracao.Get_Id()==99999)
			{
				EliminarSpace(m_Spaces,SpaceElegido);
				continue;
			}
		}
		//Actualizar lista de bloques

		if (Configuracao.Get_Id()<1000)
		{
			ActualizarListas_Ceschia(SpaceElegido,Configuracao);
			m_volumen_ocupado_cliente[Configuracao.Get_m_cliente_config()]+=Configuracao.Get_Volumen();
			if(Get_Algoritmo_Opcion_Ceschia())
			{	
				if(Configuracao.Get_z1()==0)
					m_superficie_cliente[Configuracao.Get_m_cliente_config()]+=
					(Configuracao.Get_x2()-Configuracao.Get_x1())*(Configuracao.Get_y2()-Configuracao.Get_y1());
			}
		}

		std::list<SPACE> ::iterator it;
		if (aleatorizado==false) 
		{
			m_max_total_volumen_ocupado=m_total_volumen_ocupado;
			m_Best_Configuracao=m_Configuracaos;
			continue;
		}

		if(m_mejora_local_cliente == true && ya_mejorado_cliente[cliente]==false && m_total_volumen_ocupado_cliente >= m_val_quita*m_total_volumen_cliente[cliente] && m_total_volumen_ocupado_cliente<m_total_volumen_cliente[cliente])
		{
			ya_mejorado_cliente[cliente]=true;
			m_Spaces_Mejora_cliente=m_Spaces;
			m_Configuracaos_Mejora_cliente=m_Configuracaos;
			if(Get_Algoritmo_Opcion_Ceschia())
			{
				m_total_costo_ocupado_mejora_cliente=m_total_costo_ocupado;
			}
			else
			{
				m_total_volumen_ocupado_mejora_cliente=m_total_volumen_ocupado;
			}
			m_cliente_Mejora_cliente=cliente;
			m_cambiar_cliente_Mejora_cliente=m_cambiar_cliente;
			for (register int m=0;m<m_Q;m++)
			{
				m_Pieces_Mejora_cliente[m]= m_Pieces[m].Get_Num();
			}
		}
		if (aleatorizado==true && m_mejora_local==true && ya_mejorado==false && m_total_volumen_ocupado>=m_val_quita*m_max_total_volumen_ocupado)
		{
			ya_mejorado=true;
			m_Spaces_Mejora=m_Spaces;
			m_Configuracaos_Mejora=m_Configuracaos;
			m_Bloques_Mejora=m_Bloques;
			if(Get_Algoritmo_Opcion_Ceschia())
			{
				m_total_costo_ocupado_mejora=m_total_costo_ocupado;
			}
			else
			{
				m_total_volumen_ocupado_mejora=m_total_volumen_ocupado;
			}
			m_cliente_Mejora=cliente;
			m_cambiar_cliente_Mejora=m_cambiar_cliente;
			for (register int m=0;m<m_Q;m++)
			{
				m_Pieces_Mejora[m]=m_Pieces[m].Get_Num();
			}
		}
	}
}
void CONTAINER::MejoraLocalFINALPeso_Ceschia(bool fobjetivo)
{
	m_objetivo_vol=fobjetivo;
	m_Aleatorizado=false;
	if(Get_Algoritmo_Opcion_Ceschia())
	{
		m_total_costo_ocupado=m_total_costo_ocupado_mejora;
	}
	else
	{
		m_total_volumen_ocupado=m_total_volumen_ocupado_mejora;
	}

	m_Configuracaos=m_Configuracaos_Mejora;
	m_Spaces=m_Spaces_Mejora;
	SPACE SpaceElegido(0,0,0,m_X,m_Y,m_Z,	min((m_X*m_Y*m_Z)+1,m_total_volumen+1),0,m_Y,m_X,this);
	for (register int ki=0;ki<m_Q;ki++)
	{
		//Pongo en cada pieza las que tengo
		m_Pieces[ki].Set_Num(m_Pieces_Mejora[ki]);
	}
	m_cambiar_cliente=m_cambiar_cliente_Mejora;
	int cliente=m_cliente_Mejora;
	while (m_Spaces.size()>0)
	{	
		if(m_cambiar_cliente)
		{
			cliente++;
			if (cliente>=m_num_total_clientes)
				return;
			else
			{
				m_cambiar_cliente=false;
				std::list <SPACE> ::reverse_iterator itSpace;
				for(itSpace=m_Spaces.rbegin();itSpace!=m_Spaces.rend();itSpace++)
				{
					if((*itSpace).Get_Flag()==true)
					{
						(*itSpace).Set_Flag(false);
					}
				}
				switch(Get_Algoritmo_Opcion())
				{
					case 1:   // CESCHIA
						ActualizarEspaciosPorCambioCliente_Ceschia(m_Spaces,m_Configuracaos);
						break;
					case 2:  // PESOS
						return;
						break;
					case 3:  // DANESES
						ActualizarEspaciosPorCambioCliente_Ceschia(m_Spaces,m_Configuracaos);
						break;
					case 4:  // LIU
					{
						ActualizarEspaciosPorCambioCliente_Ceschia(m_Spaces,m_Configuracaos);
						ActualizarEspaciosPorAlcanzabilidad_Liu(m_Spaces);
					}	break;
					case 5:  // JUNQUEIRA
					{
						if(Get_Algoritmo_Opcion_Junqueira()) //TRUE -> Delta igual a cero 
						{				
							ActualizarEspaciosPorDelta_Junqueira(m_Spaces,m_Configuracaos,cliente);
						}
						else
						{
							ActualizarEspaciosPorCambioCliente_Ceschia(m_Spaces,m_Configuracaos);
							ActualizarEspaciosPorCambioClienteDELTA_Junqueira(m_Spaces,m_Configuracaos,cliente);
						}
					}	break;
				}
			}
		}
		//Elegir espacio
		//esta función sale si no encuentra ningún espacio
		bool posible=false;	
		ElegirSpace_Ceschia(SpaceElegido,posible);
		if (SpaceElegido.Get_Flag()==true)
			int kk=9;
		if (m_Spaces.size()==0 || (posible==false))
		{
			return;
		}
		ORIG setmin(SpaceElegido.Get_dx(),SpaceElegido.Get_dy(),SpaceElegido.Get_dz());
		CONFIGURACAO Configuracao(this,setmin,999999);

		ElegirConfiguracao_Ceschia(SpaceElegido,Configuracao,cliente);
		if (cliente<(m_num_total_clientes-1))
		{
			if (Configuracao.Get_Id()==99999)
			{
				EliminarSpaceFlag(m_Spaces,SpaceElegido);
				continue;
			}
		}
		else
		{
			if (Configuracao.Get_Id()==99999)
			{
				EliminarSpace(m_Spaces,SpaceElegido);
				continue;
			}
		}
		if (Configuracao.Get_Id()<1000)
			ActualizarListas_Ceschia(SpaceElegido,Configuracao);
	}
	m_objetivo_vol=true;
	m_Aleatorizado=true;
}

void CONTAINER::MejoraLocalPeso_Ceschia(bool fobjetivo)
{
	m_objetivo_vol=fobjetivo;
	m_Aleatorizado=false;
	if(Get_Algoritmo_Opcion_Ceschia())
	{
		m_total_costo_ocupado=m_total_costo_ocupado_mejora_cliente;
	}
	else
	{
		m_total_volumen_ocupado=m_total_volumen_ocupado_mejora_cliente;
	}

	m_Configuracaos=m_Configuracaos_Mejora_cliente;
	m_Spaces=m_Spaces_Mejora_cliente;
	SPACE SpaceElegido(0,0,0,m_X,m_Y,m_Z,min((m_X*m_Y*m_Z)+1,m_total_volumen+1),0,m_Y,m_X,this);
	for (register int ki=0;ki<m_Q;ki++)
	{
		//Pongo en cada pieza las que tengo
		m_Pieces[ki].Set_Num(m_Pieces_Mejora_cliente[ki]);
	}
	m_cambiar_cliente=m_cambiar_cliente_Mejora_cliente;
	int cliente=m_cliente_Mejora_cliente;
	while (m_Spaces.size()>0)
	{	
		if(m_cambiar_cliente)
		{
			m_objetivo_vol=true;
			m_Aleatorizado=true;
			return;
		}
		//Elegir espacio
		//esta función sale si no encuentra ningún espacio
		bool posible=false;	
		ElegirSpace_Ceschia(SpaceElegido,posible);
		if (SpaceElegido.Get_Flag()==true)
			int kk=9;
		if (m_Spaces.size()==0 || (posible==false))
		{
			return;
		}
		ORIG setmin(SpaceElegido.Get_dx(),SpaceElegido.Get_dy(),SpaceElegido.Get_dz());
		CONFIGURACAO Configuracao(this,setmin,999999);

		ElegirConfiguracao_Ceschia(SpaceElegido,Configuracao,cliente);
		if (cliente<(m_num_total_clientes-1))
		{
			if (Configuracao.Get_Id()==99999)
			{
				EliminarSpaceFlag(m_Spaces,SpaceElegido);
				continue;
			}
		}
		else
		{
			if (Configuracao.Get_Id()==99999)
			{
				EliminarSpace(m_Spaces,SpaceElegido);
				continue;
			}
		}
		if (Configuracao.Get_Id()<1000)
			ActualizarListas_Ceschia(SpaceElegido,Configuracao);
	}
	m_objetivo_vol=true;
	m_Aleatorizado=true;
}
void CONTAINER::Mejora_Cliente_Ceschia(std::vector <bool> &ya_mejorado_cliente, int cliente)
{
	int copia_m_total_volumen_ocupado;
	int copia_cliente;
	if (m_mejora_local_cliente==true && ya_mejorado_cliente[cliente])
	{
		//Si la solución es muy buena lo hago en otro caso NO
		int temp=m_total_volumen_ocupado;
		DistintasEvaluaciones(m_Configuracaos);
		int evaluacion1_sinmejora=m_evaluacion1;
		int evaluacion2_sinmejora=m_evaluacion2;
		//copia de la iteracion
		copia_m_Configuracoes = m_Configuracaos;
		copia_m_Spaces        = m_Spaces;
		copia_m_total_volumen_ocupado = m_total_volumen_ocupado;
		copia_m_cambiar_cliente = m_cambiar_cliente;
		copia_cliente         = cliente;
		for (register int ki=0;ki<m_Q;ki++)
		{
			//Pongo en cada pieza las que tengo
			copia_m_Pieces_Mejora_cliente[ki]=m_Pieces[ki].Get_Num();
			//m_Pieces[ki].Set_Num(m_Pieces_Mejora_cliente[ki]);
		}
		//Mejora con orig
		if (m_tipo_mejora==0 ||m_tipo_mejora==2)
		{
			MejoraLocalPeso_Ceschia(true);
			DistintasEvaluaciones(m_Configuracaos);
			switch (m_tipo_evaluacion)
			{
				case 1:
				{
					if(evaluacion1_sinmejora<m_evaluacion1)
					{
						m_Configuracaos = copia_m_Configuracoes;
						m_Spaces        = copia_m_Spaces;
						m_total_volumen_ocupado = copia_m_total_volumen_ocupado;
						m_cambiar_cliente = copia_m_cambiar_cliente;
						cliente         = copia_cliente;
						for (register int ki=0;ki<m_Q;ki++)
						{
							//Pongo en cada pieza las que tengo
							m_Pieces[ki].Set_Num(copia_m_Pieces_Mejora_cliente[ki]);
						}
					}
				}
				break;
				case 2:
				{
					if(evaluacion2_sinmejora<m_evaluacion2)
					{
						m_Configuracaos = copia_m_Configuracoes;
						m_Spaces        = copia_m_Spaces;
						m_total_volumen_ocupado = copia_m_total_volumen_ocupado;
						m_cambiar_cliente = copia_m_cambiar_cliente;
						cliente         = copia_cliente;
						//DibujarOpenGL(m_Configuracaos);
						for (register int ki=0;ki<m_Q;ki++)
						{
							//Pongo en cada pieza las que tengo
							m_Pieces[ki].Set_Num(copia_m_Pieces_Mejora_cliente[ki]);
						}
					}
				}
				break;
			}
		}
		DistintasEvaluaciones(m_Configuracaos);
		evaluacion1_sinmejora=m_evaluacion1;
		evaluacion2_sinmejora=m_evaluacion2;
		//copia de la iteracion
		copia_m_Configuracoes = m_Configuracaos;
		copia_m_Spaces        = m_Spaces;
		copia_m_total_volumen_ocupado = m_total_volumen_ocupado;
		copia_m_cambiar_cliente = m_cambiar_cliente;
		copia_cliente         = cliente;
		for (register int ki=0;ki<m_Q;ki++)
		{
			//Pongo en cada pieza las que tengo
			copia_m_Pieces_Mejora_cliente[ki]=m_Pieces[ki].Get_Num();
		}
		if (m_tipo_mejora==1 ||m_tipo_mejora==2)
		{
			MejoraLocalPeso_Ceschia(false);
			DistintasEvaluaciones(m_Configuracaos);
			switch (m_tipo_evaluacion)
			{
				case 1:
				{
					if(evaluacion1_sinmejora<m_evaluacion1)
					{
						m_Configuracaos = copia_m_Configuracoes;
						m_Spaces        = copia_m_Spaces;
						m_total_volumen_ocupado = copia_m_total_volumen_ocupado;
						m_cambiar_cliente = copia_m_cambiar_cliente;
						cliente         = copia_cliente;
						for (register int ki=0;ki<m_Q;ki++)
						{
							//Pongo en cada pieza las que tengo
							m_Pieces[ki].Set_Num(copia_m_Pieces_Mejora_cliente[ki]);
						}
					}
				}
				break;
				case 2:
				{
					if(evaluacion2_sinmejora<m_evaluacion2)
					{
						m_Configuracaos = copia_m_Configuracoes;
						m_Spaces        = copia_m_Spaces;
						m_total_volumen_ocupado = copia_m_total_volumen_ocupado;
						m_cambiar_cliente = copia_m_cambiar_cliente;
						cliente         = copia_cliente;
						for (register int ki=0;ki<m_Q;ki++)
						{
							//Pongo en cada pieza las que tengo
							m_Pieces[ki].Set_Num(copia_m_Pieces_Mejora_cliente[ki]);
						}
					}
				}
				break;
			}
		}
	}
}
void CONTAINER::ActualizarEspaciosPorCambioCliente_Ceschia(std::list < SPACE > &lista_espacio, std::list < CONFIGURACAO > lista_configuracao)
{
	int flag;
	std::list < SPACE > ::iterator itSpaceAux;
	//std::list <SPACE> ::iterator itSpace;
	std::list< SPACE > lista_temp;
	std::list < CONFIGURACAO > ::iterator itConfig;
	std::list <SPACE> ::reverse_iterator itSpace;
	for(itSpace=lista_espacio.rbegin();itSpace!=lista_espacio.rend();itSpace++)
	{
		if((*itSpace).Get_Flag()==false)
		{
			SPACE Espacio((*itSpace).Get_x1(),(*itSpace).Get_y1(),(*itSpace).Get_z1(),(*itSpace).Get_x2(),(*itSpace).Get_y2(),(*itSpace).Get_z2(), (*itSpace).Get_m_soporta_peso(),(*itSpace).Get_r_y1(),(*itSpace).Get_r_y2(),(*itSpace).Get_r_L(),this);
			SPACE EspacioAux((*itSpace).Get_x1(),(*itSpace).Get_y1(),(*itSpace).Get_z1(),(*itSpace).Get_x2(),(*itSpace).Get_y2(),(*itSpace).Get_z2(), (*itSpace).Get_m_soporta_peso(),(*itSpace).Get_r_y1(),(*itSpace).Get_r_y2(),(*itSpace).Get_r_L(),this);
			flag=0;
			lista_temp.clear();
			for(itConfig = lista_configuracao.begin();itConfig!=lista_configuracao.end();itConfig++)
			{
				if((*itConfig).Get_x1()>=Espacio.Get_x2() && (*itConfig).Get_z2()>Espacio.Get_z1())
				{
					if((*itConfig).Get_y2()>=Espacio.Get_y2() && (*itConfig).Get_y1()<=Espacio.Get_y1())
					{
						flag=1;
						break;
					}
					else
					{
						if((*itConfig).Get_y2()<Espacio.Get_y2() && (*itConfig).Get_y2()>Espacio.Get_y1() &&(*itConfig).Get_y1()<=Espacio.Get_y1())
						{
						//RecortarxDerecha
							Espacio.RecortarSpace(Espacio.Get_x1(),(*itConfig).Get_y2(),Espacio.Get_z1(),Espacio.Get_x2(),Espacio.Get_y2(),Espacio.Get_z2(),max((*itConfig).Get_y2(),Espacio.Get_r_y1()),Espacio.Get_r_y2(),Espacio.Get_r_L());
							if(Espacio.Get_dx()<m_dimension_menor||Espacio.Get_dy()<m_dimension_menor||Espacio.Get_dz()<m_dimension_menor)
							{
								flag=1;
								break;
							}
							else
							{							
								flag=2;
							}
						}
						else
						{
							if((*itConfig).Get_y2()>=Espacio.Get_y2() && (*itConfig).Get_y1()>Espacio.Get_y1() && (*itConfig).Get_y1()<Espacio.Get_y2())
							{
							//RecortarxIzquierda
								Espacio.RecortarSpace(Espacio.Get_x1(),Espacio.Get_y1(),Espacio.Get_z1(),Espacio.Get_x2(),(*itConfig).Get_y1(),Espacio.Get_z2(),Espacio.Get_r_y1(),min((*itConfig).Get_y1(),Espacio.Get_r_y2()),Espacio.Get_r_L());
								if(Espacio.Get_dx()<m_dimension_menor||Espacio.Get_dy()<m_dimension_menor||Espacio.Get_dz()<m_dimension_menor)
									{
										flag=1;
										break;
									}
								else
								{							
									flag=3;
								}
							}
							else
							{
								if((*itConfig).Get_y2()<Espacio.Get_y2() && (*itConfig).Get_y1()>Espacio.Get_y1())
								{
									//RecortarxAlgunLado
									//CrearOtroRecortadoPorLadoFaltante						
									EspacioAux.RecortarSpace(Espacio.Get_x1(),Espacio.Get_y1(),Espacio.Get_z1(),Espacio.Get_x2(),(*itConfig).Get_y1(),Espacio.Get_z2(),max((*itConfig).Get_y2(),Espacio.Get_r_y1()),Espacio.Get_r_y2(),Espacio.Get_r_L());						
									if(EspacioAux.Get_dx()>m_dimension_menor||EspacioAux.Get_dy()>m_dimension_menor||EspacioAux.Get_dz()>m_dimension_menor)
										lista_temp.push_back(EspacioAux);	
									Espacio.RecortarSpace(Espacio.Get_x1(),(*itConfig).Get_y2(),Espacio.Get_z1(),Espacio.Get_x2(),Espacio.Get_y2(),Espacio.Get_z2(),Espacio.Get_r_y1(),min((*itConfig).Get_y1(),Espacio.Get_r_y2()),Espacio.Get_r_L());
									if(Espacio.Get_dx()<m_dimension_menor||Espacio.Get_dy()<m_dimension_menor||Espacio.Get_dz()<m_dimension_menor)
									{
										flag=1;
										break;
									}
									else
									{							
										flag=4;
									}
								}
							}	
						}
					}
				}
			}
			if(flag>1)
			{
				//itSpace=lista_espacio.erase(itSpace);
				(*itSpace).Set_Flag(true);
				if(IncluidoFlag(Espacio,lista_espacio)!=true)
					InsertarOrdenFlag(lista_espacio,Espacio);
				if(lista_temp.size()!=0)
					for(itSpaceAux=lista_temp.begin();itSpaceAux!=lista_temp.end();itSpaceAux++)
						if(IncluidoFlag((*itSpaceAux),lista_espacio)!=true)
							InsertarOrdenFlag(lista_espacio,(*itSpaceAux));
			}
			else
			{
				if(flag==1)
				{
					(*itSpace).Set_Flag(true);
					if(lista_temp.size()!=0)
						for(itSpaceAux=lista_temp.begin();itSpaceAux!=lista_temp.end();itSpaceAux++)
							if(IncluidoFlag((*itSpaceAux),lista_espacio)!=true)
								InsertarOrdenFlag(lista_espacio,(*itSpaceAux));
				}
			}
		}
	}
	for(itSpaceAux=lista_espacio.begin();itSpaceAux!=lista_espacio.end();)
	{
		if((*itSpaceAux).Get_Flag()==true)
		{
			itSpaceAux=lista_espacio.erase(itSpaceAux);
		}
		else
		{
			itSpaceAux++;
		}
	}
}

/// DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES
/// DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES
/// DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES DANESES
///// PESOS SIN MULTIDROP PESOS SIN MULTIDROP PESOS SIN MULTIDROP PESOS SIN MULTIDROP PESOS SIN MULTIDROP PESOS SIN MULTIDROP
///// PESOS SIN MULTIDROP PESOS SIN MULTIDROP PESOS SIN MULTIDROP PESOS SIN MULTIDROP PESOS SIN MULTIDROP PESOS SIN MULTIDROP
///// PESOS SIN MULTIDROP PESOS SIN MULTIDROP PESOS SIN MULTIDROP PESOS SIN MULTIDROP PESOS SIN MULTIDROP PESOS SIN MULTIDROP

/// LIU LIU LIU  LIU LIU LIU LIU  LIU LIU LIU LIU  LIU LIU LIU LIU  LIU LIU LIU LIU  LIU LIU LIU LIU  LIU LIU LIU LIU  LIU
/// LIU LIU LIU  LIU LIU LIU LIU  LIU LIU LIU LIU  LIU LIU LIU LIU  LIU LIU LIU LIU  LIU LIU LIU LIU  LIU LIU LIU LIU  LIU
/// LIU LIU LIU  LIU LIU LIU LIU  LIU LIU LIU LIU  LIU LIU LIU LIU  LIU LIU LIU LIU  LIU LIU LIU LIU  LIU LIU LIU LIU  LIU
void CONTAINER::ActualizarEspaciosPorCambioCliente_Liu(std::list < SPACE > &lista_espacio, std::list < CONFIGURACAO > lista_configuracao)
{
	int flag;
	std::list < SPACE > ::iterator itSpaceAux;
	std::list< SPACE > lista_temp;
	std::list < CONFIGURACAO > ::iterator itConfig;
	std::list <SPACE> ::reverse_iterator itSpace;
	for(itSpace=lista_espacio.rbegin();itSpace!=lista_espacio.rend();itSpace++)
	{
		if((*itSpace).Get_Flag()==false)
		{
			SPACE Espacio((*itSpace).Get_x1(),(*itSpace).Get_y1(),(*itSpace).Get_z1(),(*itSpace).Get_x2(),(*itSpace).Get_y2(),(*itSpace).Get_z2(), (*itSpace).Get_m_soporta_peso(),(*itSpace).Get_r_y1(),(*itSpace).Get_r_y2(),(*itSpace).Get_r_L(),this);
			SPACE EspacioAux((*itSpace).Get_x1(),(*itSpace).Get_y1(),(*itSpace).Get_z1(),(*itSpace).Get_x2(),(*itSpace).Get_y2(),(*itSpace).Get_z2(), (*itSpace).Get_m_soporta_peso(),(*itSpace).Get_r_y1(),(*itSpace).Get_r_y2(),(*itSpace).Get_r_L(),this);
			flag=0;
			lista_temp.clear();
			for(itConfig = lista_configuracao.begin();itConfig!=lista_configuracao.end();itConfig++)
			{
				if((*itConfig).Get_x1()>=Espacio.Get_x2() && (*itConfig).Get_z2()>Espacio.Get_z1())
				{
					if((*itConfig).Get_y2()>=Espacio.Get_y2() && (*itConfig).Get_y1()<=Espacio.Get_y1())
					{
						flag=1;
						break;
					}
					else
					{
						if((*itConfig).Get_y2()<Espacio.Get_y2() && (*itConfig).Get_y2()>Espacio.Get_y1() &&(*itConfig).Get_y1()<=Espacio.Get_y1())
						{
						//RecortarxDerecha
							Espacio.RecortarSpace(Espacio.Get_x1(),(*itConfig).Get_y2(),Espacio.Get_z1(),Espacio.Get_x2(),Espacio.Get_y2(),Espacio.Get_z2(),max((*itConfig).Get_y2(),Espacio.Get_r_y1()),Espacio.Get_r_y2(),Espacio.Get_r_L());
							if(Espacio.Get_dx()<m_dimension_menor||Espacio.Get_dy()<m_dimension_menor||Espacio.Get_dz()<m_dimension_menor)
							{
								flag=1;
								break;
							}
							else
							{							
								flag=2;
							}
						}
						else
						{
							if((*itConfig).Get_y2()>=Espacio.Get_y2() && (*itConfig).Get_y1()>Espacio.Get_y1() && (*itConfig).Get_y1()<Espacio.Get_y2())
							{
							//RecortarxIzquierda
								Espacio.RecortarSpace(Espacio.Get_x1(),Espacio.Get_y1(),Espacio.Get_z1(),Espacio.Get_x2(),(*itConfig).Get_y1(),Espacio.Get_z2(),Espacio.Get_r_y1(),min((*itConfig).Get_y1(),Espacio.Get_r_y2()),Espacio.Get_r_L());
								if(Espacio.Get_dx()<m_dimension_menor||Espacio.Get_dy()<m_dimension_menor||Espacio.Get_dz()<m_dimension_menor)
									{
										flag=1;
										break;
									}
								else
								{							
									flag=3;
								}
							}
							else
							{
								if((*itConfig).Get_y2()<Espacio.Get_y2() && (*itConfig).Get_y1()>Espacio.Get_y1())
								{
									//RecortarxAlgunLado
									//CrearOtroRecortadoPorLadoFaltante						
									EspacioAux.RecortarSpace(Espacio.Get_x1(),Espacio.Get_y1(),Espacio.Get_z1(),Espacio.Get_x2(),(*itConfig).Get_y1(),Espacio.Get_z2(),max((*itConfig).Get_y2(),Espacio.Get_r_y1()),Espacio.Get_r_y2(),Espacio.Get_r_L());						
									if(EspacioAux.Get_dx()>m_dimension_menor||EspacioAux.Get_dy()>m_dimension_menor||EspacioAux.Get_dz()>m_dimension_menor)
										lista_temp.push_back(EspacioAux);	
									Espacio.RecortarSpace(Espacio.Get_x1(),(*itConfig).Get_y2(),Espacio.Get_z1(),Espacio.Get_x2(),Espacio.Get_y2(),Espacio.Get_z2(),Espacio.Get_r_y1(),min((*itConfig).Get_y1(),Espacio.Get_r_y2()),Espacio.Get_r_L());
									if(Espacio.Get_dx()<m_dimension_menor||Espacio.Get_dy()<m_dimension_menor||Espacio.Get_dz()<m_dimension_menor)
									{
										flag=1;
										break;
									}
									else
									{							
										flag=4;
									}
								}
							}	
						}
					}
				}
			}
			if(flag>1)
			{
				//itSpace=lista_espacio.erase(itSpace);
				(*itSpace).Set_Flag(true);
				if(IncluidoFlag(Espacio,lista_espacio)!=true)
					InsertarOrdenFlag(lista_espacio,Espacio);
				if(lista_temp.size()!=0)
					for(itSpaceAux=lista_temp.begin();itSpaceAux!=lista_temp.end();itSpaceAux++)
						if(IncluidoFlag((*itSpaceAux),lista_espacio)!=true)
							InsertarOrdenFlag(lista_espacio,(*itSpaceAux));
			}
			else
			{
				if(flag==1)
				{
					(*itSpace).Set_Flag(true);
					if(lista_temp.size()!=0)
						for(itSpaceAux=lista_temp.begin();itSpaceAux!=lista_temp.end();itSpaceAux++)
							if(IncluidoFlag((*itSpaceAux),lista_espacio)!=true)
								InsertarOrdenFlag(lista_espacio,(*itSpaceAux));
				}
			}
		}
	}
	for(itSpaceAux=lista_espacio.begin();itSpaceAux!=lista_espacio.end();)
	{
		if((*itSpaceAux).Get_Flag()==true)
		{
			itSpaceAux=lista_espacio.erase(itSpaceAux);
		}
		else
		{
			itSpaceAux++;
		}
	}
}
void CONTAINER::ActualizarEspaciosPorAlcanzabilidad_Liu(std::list < SPACE > &lista_espacio)
{
	int flag;
	int Lalcanzable, Halcanzable, Brazo;
	int menor_referencia, y1_referencia, y2_referencia; 
	std::list < SPACE > ::iterator itSpaceAux;
	std::list <SPACE> ::iterator itSpace;
	for(itSpace=lista_espacio.begin();itSpace!=lista_espacio.end();itSpace++)
	{
		if(((*itSpace).Get_Flag()==false)&&((*itSpace).Get_z1()>0))
		{
			flag=0;
			menor_referencia=m_X+1;
			for(itSpaceAux=lista_espacio.begin();itSpaceAux!=lista_espacio.end();itSpaceAux++)
			{
				if(itSpaceAux!=itSpace)
				{
					if((*itSpaceAux).Get_Flag()==false)
					{
						//Solo analice los del suelo, que comienza después del espacio y tienen su fin en la puerta 
						if(((*itSpaceAux).Get_z1()==0)&&((*itSpaceAux).Get_x1()>=(*itSpace).Get_x2())&&((*itSpaceAux).Get_x2()==m_X))
						{
							//Izquierdo
							if(((*itSpaceAux).Get_y1()<=(*itSpace).Get_y1())&&((*itSpaceAux).Get_y2()>(*itSpace).Get_y1())&&((*itSpaceAux).Get_y2()<=(*itSpace).Get_y2()))
							{
								if((*itSpaceAux).Get_x1()<menor_referencia)
								{	
									flag=1;
									menor_referencia =(*itSpaceAux).Get_x1();
									y1_referencia    =(*itSpace).Get_y1();
									y2_referencia    =(*itSpaceAux).Get_y2();
								}
							}
							//Derecho
							if(((*itSpaceAux).Get_y2()>=(*itSpace).Get_y2())&&((*itSpaceAux).Get_y1()<(*itSpace).Get_y2())&&((*itSpaceAux).Get_y1()>=(*itSpace).Get_y1()))
							{
								if((*itSpaceAux).Get_x1()<menor_referencia)
								{
									flag=2;
									menor_referencia =(*itSpaceAux).Get_x1();
									y1_referencia    =(*itSpaceAux).Get_y1();
									y2_referencia    =(*itSpace).Get_y2();
								}
							}
							//Dentro
							if((((*itSpaceAux).Get_y1()>(*itSpace).Get_y1())&&((*itSpaceAux).Get_y1()<(*itSpace).Get_y2()))&&((*itSpaceAux).Get_y2()>(*itSpace).Get_y1())&&((*itSpaceAux).Get_y2()<(*itSpace).Get_y2()))
							{
								if((*itSpaceAux).Get_x1()<menor_referencia)
								{
									flag=3;
									menor_referencia =(*itSpaceAux).Get_x1();
									y1_referencia    =(*itSpaceAux).Get_y1();
									y2_referencia    =(*itSpaceAux).Get_y2();
								}
							}
							//Fuera
							if(((*itSpaceAux).Get_y1()<=(*itSpace).Get_y1())&&((*itSpaceAux).Get_y2()>=(*itSpace).Get_y2()))
							{
								if((*itSpaceAux).Get_x1()<menor_referencia)
								{
									flag=4;
									menor_referencia =(*itSpaceAux).Get_x1();
									y1_referencia    =(*itSpace).Get_y1();
									y2_referencia    =(*itSpace).Get_y2();
								}
							}
						}
					}
				}
				if(menor_referencia==(*itSpace).Get_x2())
					break;
			}
			if(flag>0)
			{
				Lalcanzable=menor_referencia-(*itSpace).Get_x2();
				Halcanzable=(*itSpace).Get_z1();
				Brazo = min(200-Halcanzable,60);
				if((Lalcanzable+Halcanzable<=200)&&(Lalcanzable<=Brazo))
				{
					(*itSpace).Set_r_y1(y1_referencia);
					(*itSpace).Set_r_y2(y2_referencia);
					(*itSpace).Set_r_L(Brazo-Lalcanzable);
				}
				else
				{
					(*itSpace).Set_Flag(true);
				}
			}
			else
			{
				(*itSpace).Set_Flag(true);
			}
		}
	}
	for(itSpaceAux=lista_espacio.begin();itSpaceAux!=lista_espacio.end();)
	{
		if((*itSpaceAux).Get_Flag()==true)
		{
			itSpaceAux=lista_espacio.erase(itSpaceAux);
		}
		else
		{
			itSpaceAux++;
		}
	}
}

/// JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA
/// JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA
/// JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA JUNQUEIRA

void CONTAINER::ActualizarEspaciosPorDelta_Junqueira(std::list < SPACE > &lista_espacio, std::list < CONFIGURACAO > lista_configuracao,int cliente)
{
	int flag;
	int mayor_X=0;
	std::list < SPACE > ::iterator itSpaceAux;
	//std::list <SPACE> ::iterator itSpace;
	std::list< SPACE > lista_temp;
	std::list < CONFIGURACAO > ::iterator itConfig;
	std::list <SPACE> ::reverse_iterator itSpace;
	for(itConfig = lista_configuracao.begin();itConfig!=lista_configuracao.end();itConfig++)
	{
		if((*itConfig).Get_m_cliente_config()==cliente-1)
			if((*itConfig).Get_x2()>mayor_X)
				mayor_X=(*itConfig).Get_x2();
	}
	lista_espacio.clear();
	if(mayor_X<m_X)
	{
		SPACE SpaceElegido(mayor_X,0,0,m_X,m_Y,m_Z,min((m_X*m_Y*m_Z)+1,m_total_volumen+1),0,m_Y,(m_X-mayor_X),this);
		lista_espacio.push_back(SpaceElegido);
	}
}	
void CONTAINER::ActualizarEspaciosPorCambioClienteDELTA_Junqueira(std::list < SPACE > &lista_espacio, std::list < CONFIGURACAO > lista_configuracao,int cliente)
{
	int flag;
	int aux_x1;
	int mayor_X=0;
	std::list < SPACE > ::iterator itSpaceAux;
	//std::list <SPACE> ::iterator itSpace;
	std::list< SPACE > lista_temp;
	std::list < CONFIGURACAO > ::iterator itConfig;
	std::list <SPACE> ::reverse_iterator itSpace;
	for(itConfig = lista_configuracao.begin();itConfig!=lista_configuracao.end();itConfig++)
	{
		if((*itConfig).Get_m_cliente_config()==cliente-1)
			if((*itConfig).Get_x2()>mayor_X)
				mayor_X=(*itConfig).Get_x2();
	}
	for(itSpace=lista_espacio.rbegin();itSpace!=lista_espacio.rend();itSpace++)
	//for(itSpace=lista_espacio.begin();itSpace!=lista_espacio.end();itSpace++)
	{
		if((*itSpace).Get_Flag()==false)
		{
			flag=0;
			SPACE Espacio((*itSpace).Get_x1(),(*itSpace).Get_y1(),(*itSpace).Get_z1(),(*itSpace).Get_x2(),(*itSpace).Get_y2(),(*itSpace).Get_z2(),(*itSpace).Get_m_soporta_peso(),(*itSpace).Get_r_y1(),(*itSpace).Get_r_y2(),(*itSpace).Get_r_L(),this);
			if ((*itSpace).Get_z1()>0)
			{	
				if((*itSpace).Get_x2()==mayor_X)
				{
					aux_x1=max((*itSpace).Get_x1(),mayor_X-vec_minima_longitud[cliente]);
					Espacio.RecortarSpace(aux_x1,(*itSpace).Get_y1(),(*itSpace).Get_z1(),(*itSpace).Get_x2(),(*itSpace).Get_y2(),(*itSpace).Get_z2(),(*itSpace).Get_r_y1(),(*itSpace).Get_r_y2(),((*itSpace).Get_x2()-aux_x1));
					if(Espacio.Get_dx()<m_dimension_menor||Espacio.Get_dy()<m_dimension_menor||Espacio.Get_dz()<m_dimension_menor)
					{
						flag=1;
					}
					else
					{							
						flag=2;
					}
				}
				else
				{
					flag=1;
				}
			}
			else
			{
				aux_x1=max((*itSpace).Get_x1(),mayor_X-vec_minima_longitud[cliente]);
				Espacio.RecortarSpace(aux_x1,(*itSpace).Get_y1(),(*itSpace).Get_z1(),(*itSpace).Get_x2(),(*itSpace).Get_y2(),(*itSpace).Get_z2(),(*itSpace).Get_r_y1(),(*itSpace).Get_r_y2(),((*itSpace).Get_x2()-aux_x1));
				if(Espacio.Get_dx()<m_dimension_menor||Espacio.Get_dy()<m_dimension_menor||Espacio.Get_dz()<m_dimension_menor)
				{
					flag=1;
				}
				else
				{							
					flag=2;
				}
			}
			if(flag>1)
			{
				//itSpace=lista_espacio.erase(itSpace);
				(*itSpace).Set_Flag(true);
				if(IncluidoFlag((*itSpace),lista_espacio)==false)
					InsertarOrdenFlag(lista_espacio,Espacio);
			}
			else
			{
				if(flag==1)
				{
					(*itSpace).Set_Flag(true);
				}
			}
		}
	}
	for(itSpaceAux=lista_espacio.begin();itSpaceAux!=lista_espacio.end();)
	{
		if((*itSpaceAux).Get_Flag()==true)
		{
			itSpaceAux=lista_espacio.erase(itSpaceAux);
		}
		else
		{
			itSpaceAux++;
		}
	}
}
