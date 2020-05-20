#include <list>
#include <vector>
#include <ctime>
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <climits>
#include "lib/VRPH-1.0.0/inc/VRPH.h"
#include "float.h"
#include "math.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <typeinfo>
#include <ilcplex/ilocplex.h>
#define PI 3.14159265

#ifndef cambioTipoDato_H
 #include "cambioTipoDato.h"
#endif

#ifndef mostrarArreglos_H
 #include "mostrarArreglos.h"
#endif

#ifndef cargaArchivos_H
 #include "cargaArchivos.h"
#endif

#ifndef clp_H
 #include "clp.h"
#endif


//#ifndef funciones_H
// #include "funciones.h"
//#endif

//Estructura que contiene la información de una instancia de ACVRP y su solución
struct instanciaACVRP{
	vector < vector < float > > matrizDistancias;
	float capacidadFlota;
	int numeroVehiculos;
	vector < float > demandaClientes;
	vector < vector < float > > solucionACVRP;
};

//Cambio de formato
int * convertirRutaVectorArreglo(vector < int > &v){
	size_t tamanioVector = v.size(), posicion = 0;
	//int * arreglo = new int [tamanioVector-2];
	int * arreglo = new int [tamanioVector];
	for(size_t i=0;i<tamanioVector;++i){
		if(v[i]){
			arreglo[posicion] = v[i];
			++posicion;
		}
	}
	return arreglo;
}

#ifdef _WIN32
#include <windows.h>
#pragma warning(disable: 4996) // #define _CRT_SECURE_NO_WARNINGS
#endif

 #define VALUESHARDCODED
 #define MDVRP
// #define ENTERA
// #define NOLKH

#ifdef ENTERA
typedef float dist_t;
#else
typedef float dist_t;
#endif

#undef MIN_INT_FLOAT
#ifdef MIN_INT_FLOAT
dist_t max(int x, float y) {return ( (x) > (y) ) ? (x) : (y) ; }
#endif

#ifdef _WIN32
extern "C" __declspec(dllimport) int lkh_main(int argc, char *argv[]);
#else
extern "C" int lkh_main(int argc, char *argv[]);
#endif

instancia casoEstudio;

/********* Struct to be used along the algorithm*********************/

struct mdcvfp {
    int nvehicles;
    int ncustomers;
    int ndepots;
    int heightVehicle;
    int widthVehicle;
    int lengthVehicle;
    float volumeVehicle;
    dist_t factor_demand_dist; // Factor to convert capacity in distance
    dist_t penalty_reloc; // cost for relocating a customer
    int numberTypeWaste;
    struct depot {
        int max_duration;
        int max_load;
    };
    vector <depot> depots;
    struct customer {
        int id;
        float x;
        float y;
        int service;
        int demand;
        int frequency;
        int numberBoxes;
        int demandVolume;
    };
    vector <customer> customers;
    struct box{
        int customerId;
        int boxId;
        int heightBox;
        int widthBox;
        int lengthBox;
        bool fragilityBox;
        int wasteType;
    };
    vector <box> boxes;
    struct relocation_point{
        int customerId;
        int x;
        int y;
    };
    vector <relocation_point> relocation_points;
};

/******************************************************************/

class node {
public:
    int id;
    float x;
    float y;
};

class nclients: public node {
public:
    float dem;
};

class ndepots: public node {
public:
    float cap;
    float o_cost;
};

class arco_tabu {
public:
    int o; // Node of origin
    int d; // Node of destination
    int t_del;
    arco_tabu(int _o, int _d, int _t){
        o =_o;
        d = _d;
        t_del = _t;
    }
};

class tabu {
public:
    int it;
    list <arco_tabu> lista;
    tabu() {
        it = 0;
    }
    void add(int _o, int _d){
        int muerte = it + (5 + rand()%6); // Tenure
        if (check(_o,_d)) return;
        lista.push_back(arco_tabu(_o,_d,muerte));
    }
    void refresh(){
        it++;
        for (list <arco_tabu>::iterator _it = lista.begin(); _it != lista.end(); ){
            if (_it->t_del < it)
                _it = lista.erase(_it);
            else
                ++_it;

            if ( _it == lista.end() ) return;
        }
    }
    bool check(int _o, int _d) {
        for (list <arco_tabu>::iterator _it = lista.begin(); _it != lista.end(); ++_it) {
            if (_it->o == _o && _it->d == _d) return true;
            if (_it->o == _d && _it->d == _o) return true;
        }
        return false;
    }
    void mostrar() {
        cout << " Lista Tabu Version " << it << endl;
        for (list <arco_tabu>::iterator _it = lista.begin(); _it != lista.end(); ++_it){
            cout << " Arco " << _it->o << " - " << _it->d << " Tiempo Muerte " << _it->t_del << endl;
        }
    }
};

class vertex {
public:
    int n;
    int a;
    int p;
    int r;
    vertex(int _n, int _a, int _p, int _r) {
        n = _n;
        a = _a;
        p = _p;
        r = _r;
    }
};

class objetivo {
public:
    dist_t a;
    dist_t aa;
    objetivo(dist_t n1) {
        aa = n1;
        a = n1;
    }
    void nuevo(dist_t _u){
        aa = a;
        a = _u;
    }
    dist_t get(){
        return abs(aa - a);
    }
};

// FUNCIONES GENERALES

// Funcion para borrar elementos de un vector
void vector_erase(vector<int> &_v, int _i){
    vector <int>::iterator temp = find(_v.begin(),_v.end(),_i);
    _v.erase(temp);
}

// Funcion para definir que un vector contiene un elemento
bool vectorcontain(vector<int> &_v, int _i, int _j) {
    for (unsigned int i = 0; i <_v.size(); ++i) {
        if ((_v[i] == _i) && (_v[i+1] == _j)) return true;
    }
    return false;
}

// Funcion para retornar distancia entre nodos
dist_t Dist(vector < vector <dist_t> > &_dist,int _ci,int _cj){
    return _dist[_ci - 1][_cj - 1];
}

#define earthRadiusKm 6371.0

// This function converts decimal degrees to radians
double deg2rad(double deg) {
  return (deg * M_PI / 180);
}

//  This function converts radians to decimal degrees
double rad2deg(double rad) {
  return (rad * 180 / M_PI);
}

/**
 * Returns the distance between two points on the Earth.
 * Direct translation from http://en.wikipedia.org/wiki/Haversine_formula
 * @param lat1d Latitude of the first point in degrees
 * @param lon1d Longitude of the first point in degrees
 * @param lat2d Latitude of the second point in degrees
 * @param lon2d Longitude of the second point in degrees
 * @return The distance between the two points in kilometers
 */
 /*
float r_distance(node _ci,node _cj) {
  float lat1r, lon1r, lat2r, lon2r, u, v;
  lat1r = deg2rad(_ci.x);
  lon1r = deg2rad(_ci.y);
  lat2r = deg2rad(_cj.x);
  lon2r = deg2rad(_cj.y);
  u = sin(lat1r)*sin(lat2r);
  v = cos(lat1r)*cos(lat2r)*cos(lon1r-lon2r);
  return earthRadiusKm * acos(u+v);
}*/

float r_distance2(float x1,float x2,float y1,float y2) {
  float lat1r, lon1r, lat2r, lon2r, u, v;
  lat1r = deg2rad(x1);
  lon1r = deg2rad(y1);
  lat2r = deg2rad(x2);
  lon2r = deg2rad(y2);
  u = sin(lat1r)*sin(lat2r);
  v = cos(lat1r)*cos(lat2r)*cos(lon1r-lon2r);
  return earthRadiusKm * acos(u+v);
}

//double lat1d, double lon1d, double lat2d, double lon2d
// Funcion para calcular distancia real
float r_distance(node _ci,node _cj){
    return (sqrt(pow(_ci.x - _cj.x,2) + pow(_ci.y - _cj.y,2)));
}

// Funcion para calcular distancia de las rutas
dist_t disrutas(vector < vector <dist_t> > &_dist,vector < list <int> > &_r){
    dist_t distancia = 0;
    for (size_t i = 0; i < _r.size(); ++i){
        int fin = 1;
        for (list <int>::iterator it_i = _r[i].begin(); fin ;){
            size_t io = *it_i;
            ++it_i;
            if(it_i == _r[i].end()) {
                fin = 0;
                it_i = _r[i].begin();
            }
            size_t id = *it_i;

            distancia += Dist(_dist,io,id);
        }
    }
    return distancia;
}

// Funcion para calcular distancia de una ruta
dist_t oneroute(vector < vector <dist_t> > &_dist,list <int> &_r){
    dist_t distancia = 0;
    int fin = 1;
    for (list <int>::iterator it = _r.begin(); fin ;){
        int io = *it;
        ++it;
        if(it == _r.end()) {
            fin = 0;
            it = _r.begin();
        }
        int id = *it;

        distancia += Dist(_dist,io,id);
    }
    return distancia;
}

// Funcion verificar informacion arcos lista candidatos
bool check(vector < vector<int> > &_t, int _o, int _d) {
    if (_t[_o][_d]) return false;
    if (_t[_d][_o]) return false;
    return true;
}

// Funcion para encontrar el nodo anterior de una lista
int anterior(list <int> &_t, int _n){
    if (_n == _t.front()) return _t.back();
    list<int>::iterator it = find(_t.begin(),_t.end(),_n);
    --it;
    return *it;
}

// Funcion para encontrar el nodo posterior de una lista
int posterior(list <int> &_t, int _n){
    if (_n == _t.back()) return _t.front();
    list<int>::iterator it = find(_t.begin(),_t.end(),_n);
    ++it;
    return *it;
}

// Funcion para calcular demanda de inicio a un nodo
float in_nod(vector <nclients> &_v,list<int> &_t, int _n, int _dep){
    float dem_newr = 0;
    for (list<int>::iterator it = _t.begin(); it != _t.end(); ++it) {
        if (it == _t.begin()) continue;
        if (*it == _n) break;
        dem_newr += _v[(*it) - (_dep + 1)].dem;
    }
    return dem_newr;
}

// Funcion para calcular demanda de un nodo a fin
float nod_fin(vector <nclients> &_v,list<int> &_t, int _n, int _dep){
    float dem_newr = 0;
    list<int>::iterator it = _t.begin();
    do { ++it; } while(*it != _n);
    while(it != _t.end())
    {
        dem_newr += _v[(*it) - (_dep + 1)].dem;
        it++;
    }
    return dem_newr;
}

// Funcion para calcular demanda de inicio a fin
float in_fin(vector <nclients> &_v,list<int> &_t, int _dep){
    float dem_newr = 0;
    for (list<int>::iterator it = _t.begin(); it != _t.end(); ++it) {
        if (it == _t.begin()) continue;
        dem_newr += _v[(*it) - (_dep + 1)].dem;
    }
    return dem_newr;
}

// Funcion para penalizacion por exceso de capacidad de rutas two opt
dist_t over1 (vector <nclients> &_v,vector < list <int> > &_R,int _r1, int _r2,int _io, int _id,
              int _jo, int _jd,int _c,int _dep,vector <float> _dem,int _mov){

    dist_t over = 0;

    if (_mov == 1){
        if (_io <= _dep){
            over += (dist_t) max(0.f,in_nod(_v,_R[_r2],_jd,_dep) + nod_fin(_v,_R[_r1],_id,_dep) - _c);
            over += (dist_t) max(0.f,nod_fin(_v,_R[_r2],_jd,_dep) - _c);
        }
        if (_id <= _dep){
            over += (dist_t) max(0.f,in_fin(_v,_R[_r1],_dep) + nod_fin(_v,_R[_r2],_jd,_dep) - _c);
            over += (dist_t) max(0.f,in_nod(_v,_R[_r2],_jd,_dep) - _c);
        }
        if (_jo <= _dep){
            over += (dist_t) max(0.f,in_nod(_v,_R[_r1],_id,_dep) + nod_fin(_v,_R[_r2],_jd,_dep) - _c);
            over += (dist_t) max(0.f,nod_fin(_v,_R[_r1],_id,_dep) - _c);
        }
        if (_jd <= _dep){
            over += (dist_t) max(0.f,in_fin(_v,_R[_r2],_dep) + nod_fin(_v,_R[_r1],_id,_dep) - _c);
            over += (dist_t) max(0.f,in_nod(_v,_R[_r1],_id,_dep) - _c);
        }
        if ((_io > _dep) && (_id > _dep) && (_jo > _dep) && (_jd > _dep)){
            over += (dist_t) max(0.f,in_nod(_v,_R[_r1],_id,_dep) + nod_fin(_v,_R[_r2],_jd,_dep) - _c);
            over += (dist_t) max(0.f,in_nod(_v,_R[_r2],_jd,_dep) + nod_fin(_v,_R[_r1],_id,_dep) - _c);
        }
    }
    if (_mov == 2 ){
        if (_io <= _dep){
            over += (dist_t) max(0.f,in_fin(_v,_R[_r1],_dep) + nod_fin(_v,_R[_r2],_jd,_dep) - _c);
            over += (dist_t) max(0.f,in_nod(_v,_R[_r2],_jd,_dep) - _c);
        }
        if (_id <= _dep){
            over += (dist_t) max(0.f,in_fin(_v,_R[_r1],_dep) + in_nod(_v,_R[_r2],_jd,_dep) - _c);
            over += (dist_t) max(0.f,nod_fin(_v,_R[_r2],_jd,_dep) - _c);
        }
        if (_jo <= _dep){
            over += (dist_t) max(0.f,in_fin(_v,_R[_r2],_dep) + nod_fin(_v,_R[_r1],_id,_dep) - _c);
            over += (dist_t) max(0.f,in_nod(_v,_R[_r1],_id,_dep) - _c);
        }
        if (_jd <= _dep){
            over += (dist_t) max(0.f,in_fin(_v,_R[_r2],_dep) + in_nod(_v,_R[_r1],_id,_dep) - _c);
            over += (dist_t) max(0.f,nod_fin(_v,_R[_r1],_id,_dep) - _c);
        }
        if ((_io > _dep) && (_id > _dep) && (_jo > _dep) && (_jd > _dep)){
            over += (dist_t) max(0.f,in_nod(_v,_R[_r2],_jd,_dep) + in_nod(_v,_R[_r1],_id,_dep) - _c);
            over += (dist_t) max(0.f,nod_fin(_v,_R[_r1],_id,_dep) + nod_fin(_v,_R[_r2],_jd,_dep) - _c);
        }
    }

    for (int i = 0; i < (int)_dem.size(); ++i){
        if (i == _r1) continue;
        if (i == _r2) continue;
        over += (dist_t) max(0.f,_dem[i] - _c);
    }
    return over;
}

// Funcion para penalizacion por exceso de capacidad rutas two opt
dist_t over2r (vector <nclients> &_v,vector < list <int> > &_R,int _r1, int _r2,size_t _io,size_t _id,
               size_t _jo,size_t _jd,int _c,vector <float> &_dem,vector <float> &_cap,int mov){

    dist_t over = 0;

    if (_io <= _cap.size()){
        if (_jo <= _cap.size()){
            if (mov == 1){
                over = 0;
            }
            if (mov == 2){
                over += (dist_t) max(0.f,(_dem[_r1] + _v[_jd - (_cap.size() + 1)].dem - _v[_id - (_cap.size() + 1)].dem) - _c);
                over += (dist_t) max(0.f,(_dem[_r2] - _v[_jd - (_cap.size() + 1)].dem + _v[_id - (_cap.size() + 1)].dem) - _c);
            }
        }
        if (_jd <= _cap.size()){
            if (mov == 3){
                over = 0;
            }
        }
        if ((_jo > _cap.size()) && (_jd > _cap.size())) {
            if (mov == 2){
                over += (dist_t) max(0.f,(_dem[_r1] + _v[_jd - (_cap.size() + 1)].dem - _v[_id - (_cap.size() + 1)].dem) - _c);
                over += (dist_t) max(0.f,(_dem[_r2] - _v[_jd - (_cap.size() + 1)].dem + _v[_id - (_cap.size() + 1)].dem) - _c);
            }
            if (mov == 3){
                over += (dist_t) max(0.f,in_nod(_v,_R[_r2],_jd,_cap.size()) - _c);
                over += (dist_t) max(0.f,nod_fin(_v,_R[_r1],_id,_cap.size()) + nod_fin(_v,_R[_r2],_jd,_cap.size()) - _c);
            }
        }
    }
    if (_jo <= _cap.size()){
        if (_id <= _cap.size()){
            if (mov == 3){
                over = 0;
            }
        }
        if ((_io > _cap.size()) && (_id > _cap.size())) {
            if (mov == 2){
                over += (dist_t) max(0.f,(_dem[_r1] + _v[_jd - (_cap.size() + 1)].dem - _v[_id - (_cap.size() + 1)].dem) - _c);
                over += (dist_t) max(0.f,(_dem[_r2] - _v[_jd - (_cap.size() + 1)].dem + _v[_id - (_cap.size() + 1)].dem) - _c);
            }
            if (mov == 3){
                over += (dist_t) max(0.f,nod_fin(_v,_R[_r1],_id,_cap.size()) + nod_fin(_v,_R[_r2],_jd,_cap.size()) - _c);
                over += (dist_t) max(0.f,in_nod(_v,_R[_r1],_id,_cap.size()) - _c);
            }
        }
    }
    if (_id <= _cap.size()){
        if (_jd <= _cap.size()){
            if (mov == 1){
                over += (dist_t) max(0.f,(_dem[_r1] + _v[_jo - (_cap.size() + 1)].dem - _v[_io - (_cap.size() + 1)].dem) - _c);
                over += (dist_t) max(0.f,(_dem[_r2] - _v[_jo - (_cap.size() + 1)].dem + _v[_io - (_cap.size() + 1)].dem) - _c);
            }
            if (mov == 2){
                over = 0;
            }
        }
        if ((_jo > _cap.size()) && (_jd > _cap.size())) {
            if (mov == 1){
                over += (dist_t) max(0.f,(_dem[_r1] + _v[_jo - (_cap.size() + 1)].dem - _v[_io - (_cap.size() + 1)].dem) - _c);
                over += (dist_t) max(0.f,(_dem[_r2] - _v[_jo - (_cap.size() + 1)].dem + _v[_io - (_cap.size() + 1)].dem) - _c);
            }
            if (mov == 3){
                over += (dist_t) max(0.f,in_fin(_v,_R[_r1],_cap.size()) + in_nod(_v,_R[_r2],_jd,_cap.size()) - _c);
                over += (dist_t) max(0.f,nod_fin(_v,_R[_r2],_jd,_cap.size()));
            }
        }
    }
    if (_jd <= _cap.size()){
        if ((_io > _cap.size()) && (_id > _cap.size())) {
            if (mov == 1){
                over += (dist_t) max(0.f,(_dem[_r1] + _v[_jo - (_cap.size() + 1)].dem - _v[_io - (_cap.size() + 1)].dem) - _c);
                over += (dist_t) max(0.f,(_dem[_r2] - _v[_jo - (_cap.size() + 1)].dem + _v[_io - (_cap.size() + 1)].dem) - _c);
            }
            if (mov == 3){
                over += (dist_t) max(0.f,in_fin(_v,_R[_r2],_cap.size()) + in_nod(_v,_R[_r1],_id,_cap.size()) - _c);
                over += (dist_t) max(0.f,nod_fin(_v,_R[_r1],_id,_cap.size()));
            }
        }
    }
    if ((_io > _cap.size()) && (_id > _cap.size()) && (_jo > _cap.size()) && (_jd > _cap.size())) {
        if (mov == 1){
            over += (dist_t) max(0.f,(_dem[_r1] + _v[_jo - (_cap.size() + 1)].dem - _v[_io - (_cap.size() + 1)].dem) - _c);
            over += (dist_t) max(0.f,(_dem[_r2] - _v[_jo - (_cap.size() + 1)].dem + _v[_io - (_cap.size() + 1)].dem) - _c);
        }
        if (mov == 2){
            over += (dist_t) max(0.f,(_dem[_r1] + _v[_jd - (_cap.size() + 1)].dem - _v[_id - (_cap.size() + 1)].dem) - _c);
            over += (dist_t) max(0.f,(_dem[_r2] - _v[_jd - (_cap.size() + 1)].dem + _v[_id - (_cap.size() + 1)].dem) - _c);
        }
        if (mov == 4){
            over += (dist_t) max(0.f,nod_fin(_v,_R[_r1],_id,_cap.size()) + nod_fin(_v,_R[_r2],_jd,_cap.size()) - _c);
            over += (dist_t) max(0.f,in_nod(_v,_R[_r2],_jd,_cap.size()) + in_nod(_v,_R[_r1],_id,_cap.size()) - _c);
        }
        if (mov == 5){
            over += (dist_t) max(0.f,nod_fin(_v,_R[_r1],_id,_cap.size()) + nod_fin(_v,_R[_r2],_jd,_cap.size()) - _c);
            over += (dist_t) max(0.f,in_nod(_v,_R[_r2],_jd,_cap.size()) + in_nod(_v,_R[_r1],_id,_cap.size()) - _c);
        }
    }
    for (int i = 0; i < (int)_dem.size(); ++i){
        if (i == _r1) continue;
        if (i == _r2) continue;
        over += (dist_t) max(0.f,_dem[i] - _c);
    }
    return over;
}

// Funcion para penalizacion por exceso de capacidad de depositos two opt
dist_t over2d (vector <nclients> &_v,vector <ndepots> &_w,vector < list <int> > &_R,int _r1, int _r2,int _dep1,
               int _dep2,size_t _io,size_t _id, size_t _jo,size_t _jd,vector <float> &_dem,
               vector <float> &_cap,int mov){

    dist_t over = 0;

    if (_io <= _cap.size()){
        if (_jo <= _cap.size()){
            if (mov == 1){
                over += (dist_t) max(0.f,(_cap[_dep1-1] - _dem[_r1] + _dem[_r2]) - _w[_dep1-1].cap);
                over += (dist_t) max(0.f,(_cap[_dep2-1] - _dem[_r2] + _dem[_r1]) - _w[_dep2-1].cap);
            }
            if (mov == 2){
                over += (dist_t) max(0.f,(_cap[_dep1-1] + _v[_jd - (_cap.size() + 1)].dem
                                          - _v[_id - (_cap.size() + 1)].dem) - _w[_dep1-1].cap);
                over += (dist_t) max(0.f,(_cap[_dep2-1] - _v[_jd - (_cap.size() + 1)].dem
                                          + _v[_id - (_cap.size() + 1)].dem) - _w[_dep2-1].cap);
            }
        }
        if (_jd <= _cap.size()){
            if (mov == 3){
                over += (dist_t) max(0.f,(_cap[_dep1-1] - _dem[_r1] + _dem[_r2]) - _w[_dep1-1].cap);
                over += (dist_t) max(0.f,(_cap[_dep2-1] - _dem[_r2] + _dem[_r1]) - _w[_dep2-1].cap);
            }
        }
        if ((_jo > _cap.size()) && (_jd > _cap.size())) {
            if (mov == 2){
                over += (dist_t) max(0.f,(_cap[_dep1-1] + _v[_jd - (_cap.size() + 1)].dem
                                          - _v[_id - (_cap.size() + 1)].dem) - _w[_dep1-1].cap);
                over += (dist_t) max(0.f,(_cap[_dep2-1] - _v[_jd - (_cap.size() + 1)].dem
                                          + _v[_id - (_cap.size() + 1)].dem) - _w[_dep2-1].cap);
            }
            if (mov == 3){
                over += (dist_t) max(0.f,(_cap[_dep1-1] - nod_fin(_v,_R[_r1],_id,_cap.size())
                                          + in_nod(_v,_R[_r2],_jd,_cap.size())) - _w[_dep1-1].cap);
                over += (dist_t) max(0.f,(_cap[_dep2-1] + nod_fin(_v,_R[_r1],_id,_cap.size())
                                          - in_nod(_v,_R[_r2],_jd,_cap.size())) - _w[_dep2-1].cap);
            }
        }
    }
    if (_jo <= _cap.size()){
        if (_id <= _cap.size()){
            if (mov == 3){
                over += (dist_t) max(0.f,(_cap[_dep1-1] - _dem[_r1] + _dem[_r2]) - _w[_dep1-1].cap);
                over += (dist_t) max(0.f,(_cap[_dep2-1] - _dem[_r2] + _dem[_r1]) - _w[_dep2-1].cap);
            }
        }
        if ((_io > _cap.size()) && (_id > _cap.size())) {
            if (mov == 2){
                over += (dist_t) max(0.f,(_cap[_dep1-1] + _v[_jd - (_cap.size() + 1)].dem
                                          - _v[_id - (_cap.size() + 1)].dem) - _w[_dep1-1].cap);
                over += (dist_t) max(0.f,(_cap[_dep2-1] - _v[_jd - (_cap.size() + 1)].dem
                                          + _v[_id - (_cap.size() + 1)].dem) - _w[_dep2-1].cap);
            }
            if (mov == 3){
                over += (dist_t) max(0.f,(_cap[_dep1-1] - in_nod(_v,_R[_r1],_id,_cap.size())
                                          + nod_fin(_v,_R[_r2],_jd,_cap.size())) - _w[_dep1-1].cap);
                over += (dist_t) max(0.f,(_cap[_dep2-1] + in_nod(_v,_R[_r1],_id,_cap.size())
                                          - nod_fin(_v,_R[_r2],_jd,_cap.size())) - _w[_dep2-1].cap);
            }
        }
    }
    if (_id <= _cap.size()){
        if (_jd <= _cap.size()){
            if (mov == 1){
                over += (dist_t) max(0.f,(_cap[_dep1-1] + _v[_jo - (_cap.size() + 1)].dem
                                          - _v[_io - (_cap.size() + 1)].dem) - _w[_dep1-1].cap);
                over += (dist_t) max(0.f,(_cap[_dep2-1] - _v[_jo - (_cap.size() + 1)].dem
                                          + _v[_io - (_cap.size() + 1)].dem) - _w[_dep2-1].cap);
            }
            if (mov == 2){
                over += (dist_t) max(0.f,(_cap[_dep1-1] - _dem[_r1] + _dem[_r2]) - _w[_dep1-1].cap);
                over += (dist_t) max(0.f,(_cap[_dep2-1] - _dem[_r2] + _dem[_r1]) - _w[_dep2-1].cap);
            }
        }
        if ((_jo > _cap.size()) && (_jd > _cap.size())) {
            if (mov == 1){
                over += (dist_t) max(0.f,(_cap[_dep1-1] + _v[_jo - (_cap.size() + 1)].dem
                                          - _v[_io - (_cap.size() + 1)].dem) - _w[_dep1-1].cap);
                over += (dist_t) max(0.f,(_cap[_dep2-1] - _v[_jo - (_cap.size() + 1)].dem
                                          + _v[_io - (_cap.size() + 1)].dem) - _w[_dep2-1].cap);
            }
            if (mov == 3){
                over += (dist_t) max(0.f,(_cap[_dep1-1] - in_fin(_v,_R[_r1],_cap.size())
                                          + nod_fin(_v,_R[_r2],_jd,_cap.size())) - _w[_dep1-1].cap);
                over += (dist_t) max(0.f,(_cap[_dep2-1] + in_fin(_v,_R[_r1],_cap.size())
                                          - nod_fin(_v,_R[_r2],_jd,_cap.size())) - _w[_dep2-1].cap);
            }
        }
    }
    if (_jd <= _cap.size()){
        if ((_io > _cap.size()) && (_id > _cap.size())) {
            if (mov == 1){
                over += (dist_t) max(0.f,(_cap[_dep1-1] + _v[_jo - (_cap.size() + 1)].dem
                                          - _v[_io - (_cap.size() + 1)].dem) - _w[_dep1-1].cap);
                over += (dist_t) max(0.f,(_cap[_dep2-1] - _v[_jo - (_cap.size() + 1)].dem
                                          + _v[_io - (_cap.size() + 1)].dem) - _w[_dep2-1].cap);
            }
            if (mov == 3){
                over += (dist_t) max(0.f,(_cap[_dep1-1] + in_fin(_v,_R[_r2],_cap.size())
                                          - nod_fin(_v,_R[_r1],_id,_cap.size())) - _w[_dep1-1].cap);
                over += (dist_t) max(0.f,(_cap[_dep2-1] - in_fin(_v,_R[_r2],_cap.size())
                                          + nod_fin(_v,_R[_r1],_id,_cap.size())) - _w[_dep2-1].cap);
            }
        }
    }
    if ((_io > _cap.size()) && (_id > _cap.size()) && (_jo > _cap.size()) && (_jd > _cap.size())) {
        if (mov == 1){
            over += (dist_t) max(0.f,(_cap[_dep1-1] + _v[_jo - (_cap.size() + 1)].dem
                                      - _v[_io - (_cap.size() + 1)].dem) - _w[_dep1-1].cap);
            over += (dist_t) max(0.f,(_cap[_dep2-1] - _v[_jo - (_cap.size() + 1)].dem
                                      + _v[_io - (_cap.size() + 1)].dem) - _w[_dep2-1].cap);
        }
        if (mov == 2){
            over += (dist_t) max(0.f,(_cap[_dep1-1] + _v[_jd - (_cap.size() + 1)].dem
                                      - _v[_id - (_cap.size() + 1)].dem) - _w[_dep1-1].cap);
            over += (dist_t) max(0.f,(_cap[_dep2-1] - _v[_jd - (_cap.size() + 1)].dem
                                      + _v[_id - (_cap.size() + 1)].dem) - _w[_dep2-1].cap);
        }
        if (mov == 4){
            over += (dist_t) max(0.f,(_cap[_dep1-1] - in_nod(_v,_R[_r1],_id,_cap.size())
                                      + nod_fin(_v,_R[_r2],_jd,_cap.size())) - _w[_dep1-1].cap);
            over += (dist_t) max(0.f,(_cap[_dep2-1] + in_nod(_v,_R[_r1],_id,_cap.size())
                                      - nod_fin(_v,_R[_r2],_jd,_cap.size())) - _w[_dep2-1].cap);
        }
        if (mov == 5){
            over += (dist_t) max(0.f,(_cap[_dep1-1] - in_nod(_v,_R[_r1],_id,_cap.size())
                                      + nod_fin(_v,_R[_r2],_jd,_cap.size())) - _w[_dep1-1].cap);
            over += (dist_t) max(0.f,(_cap[_dep2-1] + in_nod(_v,_R[_r1],_id,_cap.size())
                                      - nod_fin(_v,_R[_r2],_jd,_cap.size())) - _w[_dep2-1].cap);
        }
    }
    for (size_t i = 0; i < _cap.size(); ++i){
        if (i == (_dep1-1)) continue;
        if (i == (_dep2-1)) continue;
        over += (dist_t) max(0.f,_cap[i] - _w[i].cap);
    }
    return over;
}

// Funcion para penalizacion por exceso de capacidad de rutas exchange
dist_t o_excr (vector <nclients> &_v,vector <ndepots> &_w,vector < list <int> > &_R,int _r1, int _r2,int _io,int _id,
               int _jo, int _jd,int _c,vector <float> _dem,int mov){

    dist_t over = 0;

    if (_R[_r1].front() == _R[_r2].front()){
        if ((mov == 1) || (mov == 2)){
            over += (dist_t) max(0.f,_dem[_r1] + _v[_jo-(_w.size() + 1)].dem + _v[_jd-(_w.size() + 1)].dem - _c);
            over += (dist_t) max(0.f,_dem[_r2] - _v[_jo-(_w.size() + 1)].dem - _v[_jd-(_w.size() + 1)].dem - _c);
        }
        if ((mov == 3) || (mov == 4)){
            over += (dist_t) max(0.f,_dem[_r1] - _v[_io-(_w.size() + 1)].dem - _v[_id-(_w.size() + 1)].dem - _c);
            over += (dist_t) max(0.f,_dem[_r2] + _v[_io-(_w.size() + 1)].dem + _v[_id-(_w.size() + 1)].dem - _c);
        }
    }
    if (_R[_r1].front() != _R[_r2].front()){
        if ((mov == 1) || (mov == 2)){
            over += (dist_t) max(0.f,_dem[_r1] + _v[_jo-(_w.size() + 1)].dem + _v[_jd-(_w.size() + 1)].dem - _c);
            over += (dist_t) max(0.f,_dem[_r2] - _v[_jo-(_w.size() + 1)].dem - _v[_jd-(_w.size() + 1)].dem - _c);
        }
        if ((mov == 3) || (mov == 4)){
            over += (dist_t) max(0.f,_dem[_r1] - _v[_io-(_w.size() + 1)].dem - _v[_id-(_w.size() + 1)].dem - _c);
            over += (dist_t) max(0.f,_dem[_r2] + _v[_io-(_w.size() + 1)].dem + _v[_id-(_w.size() + 1)].dem - _c);
        }
    }
    for (int i = 0; i < (int)_dem.size(); ++i){
        if (i == _r1) continue;
        if (i == _r2) continue;
        over += (dist_t) max(0.f,_dem[i] - _c);
    }
    return over;
}

// Funcion para penalizacion por exceso de capacidad de depositos exchange
dist_t o_excd (vector <nclients> &_v,vector <ndepots> &_w,vector < list <int> > &_R,int _r1, int _r2,int dep1,int dep2,
               int _io,int _id,int _jo, int _jd,vector <float> _cap,int mov){

    dist_t over = 0;

    if (_R[_r1].front() == _R[_r2].front()){
        if ((mov == 1) || (mov == 2)){
            over = 0;
        }
        if ((mov == 3) || (mov == 4)){
            over = 0;
        }
    }
    if (_R[_r1].front() != _R[_r2].front()){
        if ((mov == 1) || (mov == 2)){
            over += (dist_t) max(0.f,_cap[dep1-1] + _v[_jo-(_w.size() + 1)].dem + _v[_jd-(_w.size() + 1)].dem - _w[dep1-1].cap);
            over += (dist_t) max(0.f,_cap[dep2-1] - _v[_jo-(_w.size() + 1)].dem - _v[_jd-(_w.size() + 1)].dem - _w[dep2-1].cap);
        }
        if ((mov == 3) || (mov == 4)){
            over += (dist_t) max(0.f,_cap[dep1-1] - _v[_io-(_w.size() + 1)].dem - _v[_id-(_w.size() + 1)].dem - _w[dep1-1].cap);
            over += (dist_t) max(0.f,_cap[dep2-1] + _v[_io-(_w.size() + 1)].dem + _v[_id-(_w.size() + 1)].dem - _w[dep2-1].cap);
        }
    }
    for (size_t i = 0; i < _cap.size(); ++i){
        if (i == (dep1-1)) continue;
        if (i != (dep2-1)) continue;
        over += (dist_t) max(0.f,_cap[i] - _w[i].cap);
    }
    return over;
}

// Funcion para penalizacion por exceso de capacidad de rutas cross
dist_t o_crossr (vector <nclients> &_v,vector <ndepots> &_w,int _r1, int _r2,int _io,int _id,int _jo, int _jd,int _c,
                 vector <float> _dem){

    dist_t over = 0;

    over += (dist_t) max(0.f,_dem[_r1] + _v[_jo-(_w.size() + 1)].dem + _v[_jd-(_w.size() + 1)].dem
                         - _v[_io-(_w.size() + 1)].dem - _v[_id-(_w.size() + 1)].dem - _c);
    over += (dist_t) max(0.f,_dem[_r2] + _v[_io-(_w.size() + 1)].dem + _v[_id-(_w.size() + 1)].dem
                         - _v[_jo-(_w.size() + 1)].dem - _v[_jd-(_w.size() + 1)].dem - _c);

    for (int i = 0; i < (int)_dem.size(); ++i){
        if (i == _r1) continue;
        if (i == _r2) continue;
        over += (dist_t) max(0.f,_dem[i] - _c);
    }
    return over;
}

// Funcion para penalizacion por exceso de capacidad de depositos cross
dist_t o_crossd (vector <nclients> &_v,vector <ndepots> &_w,int dep1,int dep2,int _io,int _id,int _jo,int _jd,
                 vector <float> _cap){

    dist_t over = 0;

    over += (dist_t) max(0.f,_cap[dep1-1] + _v[_jo-(_w.size() + 1)].dem + _v[_jd-(_w.size() + 1)].dem
                         - _v[_io-(_w.size() + 1)].dem - _v[_id-(_w.size() + 1)].dem - _w[dep1-1].cap);
    over += (dist_t) max(0.f,_cap[dep2-1] + _v[_io-(_w.size() + 1)].dem + _v[_id-(_w.size() + 1)].dem
                         - _v[_jo-(_w.size() + 1)].dem - _v[_jd-(_w.size() + 1)].dem - _w[dep2-1].cap);

    for (size_t i = 0; i < _cap.size(); ++i){
        if (i == (dep1-1)) continue;
        if (i != (dep2-1)) continue;
        over += (dist_t) max(0.f,_cap[i] - _w[i].cap);
    }
    return over;
}

// Funcion para calcular delta insertion
dist_t delta_insert(vector < vector <dist_t> > &_dist,int _n,int _o,int _d,int _a,int _p){

    dist_t dist_t1 = Dist(_dist,_o,_n);
    dist_t dist_t2 = Dist(_dist,_d,_n);
    dist_t dist_t3 = Dist(_dist,_a,_p);
    dist_t dist_t4 = Dist(_dist,_n,_a);
    dist_t dist_t5 = Dist(_dist,_n,_p);
    dist_t dist_t6 = Dist(_dist,_o,_d);

    return (dist_t1 + dist_t2 + dist_t3 - dist_t4 - dist_t5 - dist_t6);
}

// Funcion para calcular delta swap
dist_t delta_swap(vector < vector <dist_t> > &_dist,int _n1,int _n2,int _an1,int _pn1,int _an2,int _pn2){

    dist_t dist_t1 = Dist(_dist,_an2,_n1);
    dist_t dist_t2 = Dist(_dist,_n1,_pn2);
    dist_t dist_t3 = Dist(_dist,_an1,_n2);
    dist_t dist_t4 = Dist(_dist,_n2,_pn1);
    dist_t dist_t5 = Dist(_dist,_an1,_n1);
    dist_t dist_t6 = Dist(_dist,_n1,_pn1);
    dist_t dist_t7 = Dist(_dist,_an2,_n2);
    dist_t dist_t8 = Dist(_dist,_n2,_pn2);

    return (dist_t1 + dist_t2 + dist_t3 + dist_t4 - dist_t5 - dist_t6 - dist_t7 - dist_t8);
}

// Funcion para calcular delta two opt (same depot[Alternativa 1])
dist_t delta_topts1(vector < vector <dist_t> > &_dist,int _o1,int _d1,int _o2,int _d2){

    dist_t dist_t1 = Dist(_dist,_o1,_d2);
    dist_t dist_t2 = Dist(_dist,_o2,_d1);
    dist_t dist_t3 = Dist(_dist,_o1,_d1);
    dist_t dist_t4 = Dist(_dist,_o2,_d2);

    return (dist_t1 + dist_t2 - dist_t3 - dist_t4);
}

// Funcion para calcular delta two opt (same route - same depot[Alternativa 2])
dist_t delta_topts2(vector < vector <dist_t> > &_dist,int _o1,int _d1,int _o2,int _d2){

    dist_t dist_t1 = Dist(_dist,_o1,_o2);
    dist_t dist_t2 = Dist(_dist,_d1,_d2);
    dist_t dist_t3 = Dist(_dist,_o1,_d1);
    dist_t dist_t4 = Dist(_dist,_o2,_d2);

    return (dist_t1 + dist_t2 - dist_t3 - dist_t4);
}

// Funcion para calcular delta two opt (different depot [alternativa 1])
dist_t dif_1(vector < vector <dist_t> > &_dist,list<int> &_r1, list<int> &_r2,int _o1,int _d1,int _o2,int _d2){

    int _ao1 = anterior(_r1,_o1);
    int _ao2 = anterior(_r2,_o2);

    dist_t dist_t1 = Dist(_dist,_o1,_d2);
    dist_t dist_t2 = Dist(_dist,_o2,_d1);
    dist_t dist_t3 = Dist(_dist,_o2,_ao1);
    dist_t dist_t4 = Dist(_dist,_o1,_ao2);
    dist_t dist_t5 = Dist(_dist,_o1,_d1);
    dist_t dist_t6 = Dist(_dist,_o2,_d2);
    dist_t dist_t7 = Dist(_dist,_ao1,_o1);
    dist_t dist_t8 = Dist(_dist,_ao2,_o2);

    return (dist_t1 + dist_t2 + dist_t3 + dist_t4 - dist_t5 - dist_t6 - dist_t7 - dist_t8);
}

// Funcion para calcular delta two opt (different depot [alternativa 2])
dist_t dif_2(vector < vector <dist_t> > &_dist,list<int> &_r1, list<int> &_r2,int _o1,int _d1,int _o2,int _d2){

    int _pd1 = posterior(_r1,_d1);
    int _pd2 = posterior(_r2,_d2);

    dist_t dist_t1 = Dist(_dist,_o1,_d2);
    dist_t dist_t2 = Dist(_dist,_o2,_d1);
    dist_t dist_t3 = Dist(_dist,_d2,_pd1);
    dist_t dist_t4 = Dist(_dist,_d1,_pd2);
    dist_t dist_t5 = Dist(_dist,_o1,_d1);
    dist_t dist_t6 = Dist(_dist,_o2,_d2);
    dist_t dist_t7 = Dist(_dist,_d1,_pd1);
    dist_t dist_t8 = Dist(_dist,_d2,_pd2);

    return (dist_t1 + dist_t2 + dist_t3 + dist_t4 - dist_t5 - dist_t6 - dist_t7 - dist_t8);
}

// Funcion para calcular delta two opt (different depot [alternativa 3])
dist_t dif_3(vector < vector <dist_t> > &_dist,list<int> &_r1, list<int> &_r2,int _o1,int _d1,int _o2,int _d2,
             int mov,int _dep){

    int _w1 = _r1.front();
    int _w2 = _r2.front();

    dist_t dist_t1 = Dist(_dist,_o1,_o2);
    dist_t dist_t2 = Dist(_dist,_d1,_d2);
    dist_t dist_t5 = Dist(_dist,_o1,_d1);
    dist_t dist_t6 = Dist(_dist,_o2,_d2);

    dist_t dist_t3 = 0;
    dist_t dist_t4 = 0;
    dist_t dist_t7 = 0;
    dist_t dist_t8 = 0;
    int _adp1 = 0;
    int _adp2 = 0;
    int _pd1 = 0;
    int _pd2 = 0;

    if((_o1 <= _dep) || (_d2 <= _dep)){
        if (mov == 3){
            _adp1 = anterior(_r1,_w1);
            _pd2 = posterior(_r2,_w2);
            dist_t3 = Dist(_dist,_w1,_pd2);
            dist_t4 = Dist(_dist,_w2,_adp1);
            dist_t7 = Dist(_dist,_adp1,_w1);
            dist_t8 = Dist(_dist,_w2,_pd2);
        }
    }
    if((_d1 <= _dep) || (_o2 <= _dep)){
        if (mov == 3){
            _adp2 = anterior(_r2,_w2);
            _pd1 = posterior(_r1,_w1);
            dist_t3 = Dist(_dist,_w1,_adp2);
            dist_t4 = Dist(_dist,_w2,_pd1);
            dist_t7 = Dist(_dist,_w1,_pd1);
            dist_t8 = Dist(_dist,_adp2,_w2);
        }
    }
    if((_o1 > _dep) && (_d1 > _dep) && (_o2 > _dep) && (_d2 > _dep)){
        if (mov == 4){
            _adp2 = anterior(_r2,_w2);
            _pd1 = posterior(_r1,_w1);
            dist_t3 = Dist(_dist,_w1,_adp2);
            dist_t4 = Dist(_dist,_w2,_pd1);
            dist_t7 = Dist(_dist,_w1,_pd1);
            dist_t8 = Dist(_dist,_adp2,_w2);
        }
        if (mov == 5){
            _adp1 = anterior(_r1,_w1);
            _pd2 = posterior(_r2,_w2);
            dist_t3 = Dist(_dist,_w1,_pd2);
            dist_t4 = Dist(_dist,_w2,_adp1);
            dist_t7 = Dist(_dist,_adp1,_w1);
            dist_t8 = Dist(_dist,_w2,_pd2);
        }
    }
    return (dist_t1 + dist_t2 + dist_t3 + dist_t4 - dist_t5 - dist_t6 - dist_t7 - dist_t8);
}

// Funcion para calcular delta exchange
dist_t dif_ex(vector < vector <dist_t> > &_dist,list<int> &_r1, list<int> &_r2,int _o1,int _d1,int _o2,int _d2,int mov){

    dist_t dist_t1 = 0;
    dist_t dist_t2 = 0;
    dist_t dist_t3 = 0;
    dist_t dist_t4 = 0;
    dist_t dist_t5 = 0;
    dist_t dist_t6 = 0;

    int _ao1 = 0;
    int _pd1 = 0;
    int _ao2 = 0;
    int _pd2 = 0;

    if ((mov == 1) || (mov == 2)){
        _ao2 = anterior(_r2,_o2);
        _pd2 = posterior(_r2,_d2);
        if (mov == 1){
            dist_t1 = Dist(_dist,_o1,_o2);
            dist_t2 = Dist(_dist,_d2,_d1);
        }
        if (mov == 2){
            dist_t1 = Dist(_dist,_o1,_d2);
            dist_t2 = Dist(_dist,_o2,_d1);
        }
        dist_t3 = Dist(_dist,_ao2,_pd2);
        dist_t4 = Dist(_dist,_ao2,_o2);
        dist_t5 = Dist(_dist,_d2,_pd2);
        dist_t6 = Dist(_dist,_o1,_d1);
    }
    if ((mov == 3) || (mov == 4)){
        _ao1 = anterior(_r1,_o1);
        _pd1 = posterior(_r1,_d1);
        if (mov == 3){
            dist_t1 = Dist(_dist,_o2,_o1);
            dist_t2 = Dist(_dist,_d1,_d2);
        }
        if (mov == 4){
            dist_t1 = Dist(_dist,_o2,_d1);
            dist_t2 = Dist(_dist,_o1,_d2);
        }

        dist_t3 = Dist(_dist,_ao1,_pd1);
        dist_t4 = Dist(_dist,_ao1,_o1);
        dist_t5 = Dist(_dist,_d1,_pd1);
        dist_t6 = Dist(_dist,_o2,_d2);
    }
    return (dist_t1 + dist_t2 + dist_t3 - dist_t4 - dist_t5 - dist_t6);
}

// Funcion para calcular delta cross
dist_t dif_cross(vector < vector <dist_t> > &_dist,list<int> &_r1, list<int> &_r2,int _o1,int _d1,int _o2,int _d2){

    int _ao1 = anterior(_r1,_o1);
    int _pd1 = posterior(_r1,_d1);
    int _ao2 = anterior(_r2,_o2);
    int _pd2 = posterior(_r2,_d2);

    dist_t dist_t1 = Dist(_dist,_o1,_ao2);
    dist_t dist_t2 = Dist(_dist,_d1,_pd2);
    dist_t dist_t3 = Dist(_dist,_o2,_ao1);
    dist_t dist_t4 = Dist(_dist,_d2,_pd1);
    dist_t dist_t5 = Dist(_dist,_o1,_ao1);
    dist_t dist_t6 = Dist(_dist,_d1,_pd1);
    dist_t dist_t7 = Dist(_dist,_o2,_ao2);
    dist_t dist_t8 = Dist(_dist,_d2,_pd2);

    return (dist_t1 + dist_t2 + dist_t3 + dist_t4 - dist_t5 - dist_t6 - dist_t7 - dist_t8);
}


// Funcion para crear lista candidatos
void build_cand(bool _round,vector < list <int> > &_r,vector < vector<int> > &_c,vector < vector<float> > &_d,
                float _B, dist_t _C,int _dep, int _client){

    for(int i = 0; i < (_dep + _client); ++i){
        for (int j = 0; j < (_dep + _client); ++j){
            _c[i][j] = 0;
        }
    }

    size_t tamano_ruta = _r.size();
    for (size_t i = 0; i < tamano_ruta; ++i) {
        int fin = 1;
        for (list <int>::iterator it = _r[i].begin(); fin ; ){
            int origen = *it;
            ++it;
            if(it == _r[i].end()) {
                fin = 0;
                it = _r[i].begin();
            }
            int destino = *it;

            _c[origen - 1][destino - 1] = 1;
        }
    }

    for (int i = 0; i < _dep; ++i) {
        for (int j = 0; j < _client; ++j) {
            if(!check(_c,i,j + _dep)) continue;
            _c[i][j + _dep] = 1;
        }
    }

    float t_value;

    if(_round == 1)
        t_value = ((_B) * ((_C/100) / (_client + _r.size())));
    else
        t_value = ((_B) * ((_C) / (_client + _r.size())));

    for (int i = 0; i < _client; ++i) {
        for (int j = i+1; j < _client; ++j) {
            if(!check(_c,i + _dep,j + _dep)) continue;
            if(_d[i][j] < t_value ) {
                _c[i + _dep][j + _dep] = 1;
            }
        }
    }
}

// Funcion para crear Nodos Candidatos
void build_nodos(vector < list <int> > &_v,vector <vertex> &_n,int _dep){
    size_t tamano = _v.size();
    for (size_t i = 0; i < tamano; ++i) {
        int fin = 1;
        for (list <int>::iterator it = _v[i].begin(); fin ;){
            int Temp_Node = *it;
            ++it;
            if(it == _v[i].end()) {
                fin = 0;
                it = _v[i].begin();
            }
            if (Temp_Node <= _dep) continue;

            _n.insert(_n.begin(),vertex(Temp_Node,anterior(_v[i],Temp_Node),posterior(_v[i],Temp_Node),i));
        }
    }
}

// Funcion aprobar tabu
bool ap_tabu(tabu &_t,list<int> &_r1, list<int> &_r2,int _n1, int _n2, int _o1, int _d1,int _o2, int _d2,
             int _an1,int _pn1,int _an2,int _pn2,int _counter) {

    if (_counter == 1){
        if(_t.check(_o1,_d1)) return false;
        if(_t.check(_an1,_n1)) return false;
        if(_t.check(_n1,_pn1)) return false;
    }
    if (_counter == 2){
        if(_t.check(_an1,_n1)) return false;
        if(_t.check(_n1,_pn1)) return false;
        if(_t.check(_an2,_n2)) return false;
        if(_t.check(_n2,_pn2)) return false;
    }
    if (_counter == 3){
        if(_t.check(_o1,_d1)) return false;
        if(_t.check(_o2,_d2)) return false;
    }
    if (_counter == 4){
        if(_t.check(_o1,_d1)) return false;
        if(_t.check(_o2,_d2)) return false;
    }
    if (_counter == 5){
        int aio = anterior(_r1,_o1);
        int ajo = anterior(_r2,_o2);
        if(_t.check(_o1,_d1)) return false;
        if(_t.check(_o2,_d2)) return false;
        if(_t.check(aio,_o1)) return false;
        if(_t.check(ajo,_o2)) return false;
    }
    if (_counter == 6){
        int pid = posterior(_r1,_d1);
        int pjd = posterior(_r2,_d2);
        if(_t.check(_o1,_d1)) return false;
        if(_t.check(_o2,_d2)) return false;
        if(_t.check(_d1,pid)) return false;
        if(_t.check(_d2,pjd)) return false;
    }
    if (_counter == 7){
        int p_dp2 = posterior(_r2,_r2.front());
        int a_dp1 = anterior(_r1,_r1.front());
        if(_t.check(_o1,_d1)) return false;
        if(_t.check(_o2,_d2)) return false;
        if(_t.check(_r2.front(),p_dp2)) return false;
        if(_t.check(a_dp1,_r1.front())) return false;
    }
    if (_counter == 8){
        int p_dp1 = posterior(_r1,_r1.front());
        int a_dp2 = anterior(_r2,_r2.front());
        if(_t.check(_o1,_d1)) return false;
        if(_t.check(_o2,_d2)) return false;
        if(_t.check(_r1.front(),p_dp1)) return false;
        if(_t.check(a_dp2,_r2.front())) return false;
    }
    if (_counter == 9){
        int ajo = anterior(_r2,_o2);
        int pjd = posterior(_r2,_d2);
        if(_t.check(_o1,_d1)) return false;
        if(_t.check(ajo,_o2)) return false;
        if(_t.check(_d2,pjd)) return false;
    }
    if (_counter == 10){
        int aio = anterior(_r1,_o1);
        int pid = posterior(_r1,_d1);
        if(_t.check(_o2,_d2)) return false;
        if(_t.check(aio,_o1)) return false;
        if(_t.check(_d1,pid)) return false;
    }
    if (_counter == 11){
        int aio = anterior(_r1,_o1);
        int pid = posterior(_r1,_d1);
        int ajo = anterior(_r2,_o2);
        int pjd = posterior(_r2,_d2);
        if(_t.check(aio,_o1)) return false;
        if(_t.check(_d1,pid)) return false;
        if(_t.check(ajo,_o2)) return false;
        if(_t.check(_d2,pjd)) return false;
    }
    return true;
}

// Funcion aprobar lista candidatos
bool ap_cand(vector < vector<int> > &_c,list<int> &_r1, list<int> &_r2,int _n1, int _n2, int _io,int _id,int _jo,
             int _jd,int _a1,int _p1,int _a2,int _p2,int _counter) {

    if (_counter == 1){
        if(check(_c,_io-1,_n1-1)) return false;
        if(check(_c,_n1-1,_id-1)) return false;
        if(_r1.size() != 2)
            if(check(_c,_a1-1,_p1-1)) return false;
    }
    if (_counter == 2){
        if (check(_c,_a1-1,_n2-1)) return false;
        if (check(_c,_n2-1,_p1-1)) return false;
        if (check(_c,_a2-1,_n1-1)) return false;
        if (check(_c,_n1-1,_p2-1)) return false;
    }
    if (_counter == 3){
        if (check(_c,_io-1,_jo-1)) return false;
        if (check(_c,_id-1,_jd-1)) return false;
    }
    if (_counter == 4){
        if (check(_c,_io-1,_jd-1)) return false;
        if (check(_c,_jo-1,_id-1)) return false;
    }
    if (_counter == 5){
        int _aio = anterior(_r1,_io);
        int _ajo = anterior(_r2,_jo);
        if (check(_c,_io-1,_jd-1)) return false;
        if (check(_c,_jo-1,_id-1)) return false;
        if (check(_c,_ajo-1,_io-1)) return false;
        if (check(_c,_aio-1,_jo-1)) return false;
    }
    if (_counter == 6){
        int _pid = posterior(_r1,_id);
        int _pjd = posterior(_r2,_jd);
        if (check(_c,_io-1,_jd-1)) return false;
        if (check(_c,_id-1,_jo-1)) return false;
        if (check(_c,_id-1,_pjd-1)) return false;
        if (check(_c,_jd-1,_pid-1)) return false;
    }
    if (_counter == 7){
        int p_dp2 = posterior(_r2,_r2.front());
        int a_dp1 = anterior(_r1,_r1.front());
        if (check(_c,_io-1,_jo-1)) return false;
        if (check(_c,_id-1,_jd-1)) return false;
        if (check(_c,_r2.front()-1,a_dp1-1)) return false;
        if (check(_c,_r1.front()-1,p_dp2-1)) return false;
    }
    if (_counter == 8){
        int p_dp1 = posterior(_r1,_r1.front());
        int a_dp2 = anterior(_r2,_r2.front());
        if (check(_c,_io-1,_jo-1)) return false;
        if (check(_c,_id-1,_jd-1)) return false;
        if (check(_c,_r2.front()-1,p_dp1-1)) return false;
        if (check(_c,_r1.front()-1,a_dp2-1)) return false;
    }
    if (_counter == 9){
        int _ajo = anterior(_r2,_jo);
        int _pjd = posterior(_r2,_jd);
        if (check(_c,_io-1,_jo-1)) return false;
        if (check(_c,_jd-1,_id-1)) return false;
        if(_r2.size() != 3)
            if (check(_c,_ajo-1,_pjd-1)) return false;
    }
    if (_counter == 10){
        int _ajo = anterior(_r2,_jo);
        int _pjd = posterior(_r2,_jd);
        if (check(_c,_io-1,_jd-1)) return false;
        if (check(_c,_jo-1,_id-1)) return false;
        if(_r2.size() != 3)
            if (check(_c,_ajo-1,_pjd-1)) return false;
    }
    if (_counter == 11){
        int _aio = anterior(_r1,_io);
        int _pid = posterior(_r1,_id);
        if (check(_c,_jo-1,_io-1)) return false;
        if (check(_c,_id-1,_jd-1)) return false;
        if(_r1.size() != 3)
            if (check(_c,_aio-1,_pid-1)) return false;
    }
    if (_counter == 12){
        int _aio = anterior(_r1,_io);
        int _pid = posterior(_r1,_id);
        if (check(_c,_jo-1,_id-1)) return false;
        if (check(_c,_io-1,_jd-1)) return false;
        if(_r1.size() != 3)
            if (check(_c,_aio-1,_pid-1)) return false;
    }
    if (_counter == 13){
        int _aio = anterior(_r1,_io);
        int _pid = posterior(_r1,_id);
        int _ajo = anterior(_r2,_jo);
        int _pjd = posterior(_r2,_jd);
        if (check(_c,_ajo-1,_io-1)) return false;
        if (check(_c,_pjd-1,_id-1)) return false;
        if (check(_c,_aio-1,_jo-1)) return false;
        if (check(_c,_pid-1,_jd-1)) return false;
    }
    return true;
}

// Funcion para verificar factibilidad de la ruta
bool valido_route(vector <float> &_dem,int _vc){
    int contador = 0;
    for (size_t i = 0; i < _dem.size(); ++i) {
        if((_dem[i] - (float)_vc) > 0){
            contador += 1;
        }
    }
    if (contador == 0) return true;
    else return false;
}

// Funcion para verificar factibilidad de depots
bool valido_depot(vector <ndepots> &_d,vector <float> &_cd){
    int contador = 0;
    for (size_t i = 0; i < _cd.size(); ++i) {
        if(_cd[i] > _d[i].cap){
            contador += 1;
        }
    }
    if (contador == 0) return true;
    else return false;
}

// Funcion para same depot movimiento 1
void s_depot1 (tabu &_t,vector < vector <int> > &_cand,list<int> &_r1,list<int> &_r2,list<int> &_r3,list<int> &_r4,
               int io,int id,int jo,int jd,int dep,int route1,int route2,vector <float> &_dem,vector <nclients> &_c){

    _t.add(io,jd);
    _t.add(id,jo);
    _cand[io-1][jd-1] = 1;
    _cand[id-1][jo-1] = 1;

    if(io <= dep){
        list<int>::iterator i1 = find(_r2.begin(),_r2.end(),jd); // Iterador a jd
        list<int>::iterator i2 = find(_r1.begin(),_r1.end(),id); // Iterador a id
        _r3.splice(_r3.begin(),_r1,i2,_r1.end());
        _r4.splice(_r4.begin(),_r2,i1,_r2.end());
        _r2.splice(_r2.end(),_r3);
        _r1.splice(_r1.end(),_r4);
    }
    if(jo <= dep){
        list<int>::iterator i1 = find(_r1.begin(),_r1.end(),id); // Iterador a id
        list<int>::iterator i2 = find(_r2.begin(),_r2.end(),jd); // Iterador a jd
        _r3.splice(_r3.begin(),_r2,i2,_r2.end());
        _r4.splice(_r4.begin(),_r1,i1,_r1.end());
        _r1.splice(_r1.end(),_r3);
        _r2.splice(_r2.end(),_r4);
    }
    if(id <= dep){
        list<int>::iterator i1 = find(_r2.begin(),_r2.end(),jd); // Iterador a jd
        _r1.splice(_r1.end(),_r2,i1,_r2.end());
    }
    if(jd <= dep){
        list<int>::iterator i1 = find(_r1.begin(),_r1.end(),id); // Iterador a id
        _r2.splice(_r2.end(),_r1,i1,_r1.end());
    }
    if((io > dep) && (jo > dep) && (id > dep) && (jd > dep)) {
        list<int>::iterator i1 = find(_r1.begin(),_r1.end(),id); // Iterador a id
        list<int>::iterator i2 = find(_r2.begin(),_r2.end(),jd); // Iterador a jd
        _r3.splice(_r3.begin(),_r2,i2,_r2.end());
        _r4.splice(_r4.begin(),_r1,i1,_r1.end());
        _r1.splice(_r1.end(),_r3);
        _r2.splice(_r2.end(),_r4);
    }
    _dem[route1] = in_fin(_c,_r1,dep);
    _dem[route2] = in_fin(_c,_r2,dep);
}

// Funcion para same depot movimiento 2
void s_depot2 (tabu &_t,vector < vector <int> > &_cand,list<int> &_r1,list<int> &_r2,list<int> &_r3,list<int> &_r4,
               int io,int id,int jo,int jd,int dep,int pdr1,int pdr2,int route1,int route2,vector <float> &_dem,
               vector <nclients> &_c){

    _t.add(io,jo);
    _t.add(id,jd);
    _cand[io-1][jo-1] = 1;
    _cand[id-1][jd-1] = 1;

    if(io <= dep){
        list<int>::iterator i1 = find(_r1.begin(),_r1.end(),id); // Iterador a id
        list<int>::iterator i2 = find(_r2.begin(),_r2.end(),jd); // Iterador a jd
        _r3.splice(_r3.begin(),_r2,i2,_r2.end());
        _r3.reverse();
        _r1.splice(i1,_r3);
    }
    if(jo <= dep){
        list<int>::iterator i1 = find(_r1.begin(),_r1.end(),id); // Iterador a id
        list<int>::iterator i2 = find(_r2.begin(),_r2.end(),jd); // Iterador a jd
        _r3.splice(_r3.begin(),_r1,i1,_r1.end());
        _r3.reverse();
        _r2.splice(i2,_r3);
    }
    if(id <= dep){
        list<int>::iterator i1 = find(_r2.begin(),_r2.end(),pdr2); // Iterador al nodo posterior ruta 2
        list<int>::iterator i2 = find(_r2.begin(),_r2.end(),jd); // Iterador a jd
        _r3.splice(_r3.begin(),_r2,i1,i2);
        _r3.reverse();
        _r1.splice(_r1.end(),_r3);
    }
    if(jd <= dep){
        list<int>::iterator i1 = find(_r1.begin(),_r1.end(),pdr1); // Iterador al nodo posterior ruta 1
        list<int>::iterator i2 = find(_r1.begin(),_r1.end(),id); // Iterador a id
        _r3.splice(_r3.begin(),_r1,i1,i2);
        _r3.reverse();
        _r2.splice(_r2.end(),_r3);
    }
    if((io > dep) && (jo > dep) && (id > dep) && (jd > dep)) {
        list<int>::iterator i1 = find(_r1.begin(),_r1.end(),id); // Iterador a id
        list<int>::iterator i2 = find(_r2.begin(),_r2.end(),jd); // Iterador a jd
        list<int>::iterator i3 = find(_r1.begin(),_r1.end(),pdr1); // Iterador al nodo posterior ruta 1
        _r3.splice(_r3.begin(),_r1,i1,_r1.end());
        _r3.reverse();
        _r3.splice(_r3.end(),_r2,i2,_r2.end());
        _r4.splice(_r4.begin(),_r1,i3,_r1.end());
        _r4.reverse();
        _r2.splice(_r2.end(),_r4);
        _r1.splice(_r1.end(),_r3);
    }
    _dem[route1] = in_fin(_c,_r1,dep);
    _dem[route2] = in_fin(_c,_r2,dep);
}

// Funcion para diferent depot 1
void d_depot1 (tabu &_t,vector < vector <int> > &_cand,list<int> &_r1,list<int> &_r2,size_t io,size_t id,
               size_t jo,size_t jd,size_t aio,size_t ajo,int w1,int w2,int route1,int route2,
               vector <float> &_dem,vector <float> &_cap,vector <nclients> &_c){

    _t.add(io,jd);
    _t.add(jo,id);
    _t.add(aio,jo);
    _t.add(ajo,io);

    _cand[io-1][jd-1] = 1;
    _cand[jo-1][id-1] = 1;
    _cand[aio-1][jo-1] = 1;
    _cand[ajo-1][io-1] = 1;

    list<int>::iterator o1 = find(_r1.begin(),_r1.end(),io); // Iterador a io
    list<int>::iterator o2 = find(_r2.begin(),_r2.end(),jo); // Iterador a jo
    swap(*o1,*o2);

    if((io <= _cap.size()) && (jo <= _cap.size())) {
        _cap[w1-1] += _dem[route2];
        _cap[w1-1] -= _dem[route1];
        _cap[w2-1] += _dem[route1];
        _cap[w2-1] -= _dem[route2];
    } else {
        _dem[route1] += _c[jo - (_cap.size()+1)].dem;
        _dem[route1] -= _c[io - (_cap.size()+1)].dem;
        _dem[route2] += _c[io - (_cap.size()+1)].dem;
        _dem[route2] -= _c[jo - (_cap.size()+1)].dem;
        _cap[w1-1] += _c[jo - (_cap.size()+1)].dem;
        _cap[w1-1] -= _c[io - (_cap.size()+1)].dem;
        _cap[w2-1] += _c[io - (_cap.size()+1)].dem;
        _cap[w2-1] -= _c[jo - (_cap.size()+1)].dem;
    }
}

// Funcion para diferent depot 2
void d_depot2 (tabu &_t,vector < vector <int> > &_cand,list<int> &_r1,list<int> &_r2,size_t io,size_t id,
               size_t jo,size_t jd,size_t pid,size_t pjd,int w1,int w2,int route1,int route2,
               vector <float> &_dem,vector <float> &_cap,vector <nclients> &_c){

    _t.add(io,jd);
    _t.add(jo,id);
    _t.add(id,pjd);
    _t.add(jd,pid);

    _cand[io-1][jd-1] = 1;
    _cand[jo-1][id-1] = 1;
    _cand[id-1][pjd-1] = 1;
    _cand[jd-1][pid-1] = 1;

    list<int>::iterator d1 = find(_r1.begin(),_r1.end(),id); // Iterador a id
    list<int>::iterator d2 = find(_r2.begin(),_r2.end(),jd); // Iterador a jd
    swap(*d1,*d2);

    if((id <= _cap.size()) && (jd <= _cap.size())) {
        _cap[w1-1] += _dem[route2];
        _cap[w1-1] -= _dem[route1];
        _cap[w2-1] += _dem[route1];
        _cap[w2-1] -= _dem[route2];
    } else {
        _dem[route1] += _c[jd - (_cap.size()+1)].dem;
        _dem[route1] -= _c[id - (_cap.size()+1)].dem;
        _dem[route2] += _c[id - (_cap.size()+1)].dem;
        _dem[route2] -= _c[jd - (_cap.size()+1)].dem;
        _cap[w1-1] += _c[jd - (_cap.size()+1)].dem;
        _cap[w1-1] -= _c[id - (_cap.size()+1)].dem;
        _cap[w2-1] += _c[id - (_cap.size()+1)].dem;
        _cap[w2-1] -= _c[jd - (_cap.size()+1)].dem;
    }
}

// Funcion para diferent depot 3
void d_depot3 (tabu &_t,vector < vector <int> > &_cand,list<int> &_r1,list<int> &_r2,list<int> &_r3,size_t io,
               size_t id,size_t jo,size_t jd,size_t aw1,size_t aw2,size_t pw1,
               size_t pw2,size_t w1,size_t w2,int route1,int route2,vector <float> &_dem,
               vector <float> &_cap,vector <nclients> &_c){

    _t.add(io,jo);
    _t.add(id,jd);

    _cand[io-1][jo-1] = 1;
    _cand[id-1][jd-1] = 1;

    if((io <= _cap.size()) && (jd <= _cap.size())) {
        _t.add(w1,pw2);
        _t.add(w2,aw1);
        _cand[w1-1][pw2-1] = 1;
        _cand[w2-1][aw1-1] = 1;

        list<int>::iterator s1 = find(_r1.begin(),_r1.end(),io);
        list<int>::iterator s2 = find(_r2.begin(),_r2.end(),jd);
        swap(*s1,*s2);
        _cap[w1-1] += _dem[route2];
        _cap[w1-1] -= _dem[route1];
        _cap[w2-1] += _dem[route1];
        _cap[w2-1] -= _dem[route2];
    }
    if((jo <= _cap.size()) && (id <= _cap.size())) {
        _t.add(w1,aw2);
        _t.add(w2,pw1);
        _cand[w1-1][aw2-1] = 1;
        _cand[w2-1][pw1-1] = 1;

        list<int>::iterator s1 = find(_r2.begin(),_r2.end(),jo);
        list<int>::iterator s2 = find(_r1.begin(),_r1.end(),id);
        swap(*s1,*s2);
        _cap[w1-1] += _dem[route2];
        _cap[w1-1] -= _dem[route1];
        _cap[w2-1] += _dem[route1];
        _cap[w2-1] -= _dem[route2];
    }
    if((io <= _cap.size()) && (jo > _cap.size()) && (jd > _cap.size())) {
        _t.add(w1,pw2);
        _t.add(w2,aw1);
        _cand[w1-1][pw2-1] = 1;
        _cand[w2-1][aw1-1] = 1;

        _cap[w1-1] -= _dem[route1];
        _cap[w2-1] -= _dem[route2];
        _dem[route1] = 0;
        _dem[route2] = 0;

        list<int>::iterator i1 = find(_r1.begin(),_r1.end(),id);
        list<int>::iterator i2 = find(_r2.begin(),_r2.end(),jd);
        list<int>::iterator i3 = find(_r1.begin(),_r1.end(),w1);
        list<int>::iterator i4 = find(_r2.begin(),_r2.end(),w2);
        swap(*i3, *i4);

        _r3.splice(_r3.begin(),_r2,i2,_r2.end());
        _r3.reverse();
        _r3.splice(_r3.end(),_r1,i1,_r1.end());
        _r1.splice(_r1.end(),_r3);
        _r1.swap(_r2);

        _dem[route1] = in_fin(_c,_r1,_cap.size());
        _dem[route2] = in_fin(_c,_r2,_cap.size());
        _cap[w1-1] += _dem[route1];
        _cap[w2-1] += _dem[route2];
    }
    if((jo <= _cap.size()) && (io > _cap.size()) && (id > _cap.size())) {
        _t.add(w1,aw2);
        _t.add(w2,pw1);
        _cand[w1-1][aw2-1] = 1;
        _cand[w2-1][pw1-1] = 1;

        _cap[w1-1] -= _dem[route1];
        _cap[w2-1] -= _dem[route2];
        _dem[route1] = 0;
        _dem[route2] = 0;

        list<int>::iterator i1 = find(_r1.begin(),_r1.end(),id);
        list<int>::iterator i2 = find(_r2.begin(),_r2.end(),jd);
        list<int>::iterator i3 = find(_r1.begin(),_r1.end(),w1);
        list<int>::iterator i4 = find(_r2.begin(),_r2.end(),w2);
        swap(*i3, *i4);

        _r3.splice(_r3.begin(),_r1,i1,_r1.end());
        _r3.reverse();
        _r3.splice(_r3.end(),_r2,i2,_r2.end());
        _r2.splice(_r2.end(),_r3);
        _r1.swap(_r2);

        _dem[route1] = in_fin(_c,_r1,_cap.size());
        _dem[route2] = in_fin(_c,_r2,_cap.size());
        _cap[w1-1] += _dem[route1];
        _cap[w2-1] += _dem[route2];
    }
    if((id <= _cap.size()) && (jo > _cap.size()) && (jd > _cap.size())) {
        _t.add(w1,aw2);
        _t.add(w2,pw1);
        _cand[w1-1][aw2-1] = 1;
        _cand[w2-1][pw1-1] = 1;

        _cap[w1-1] -= _dem[route1];
        _cap[w2-1] -= _dem[route2];
        _dem[route1] = 0;
        _dem[route2] = 0;

        list<int>::iterator i1 = find(_r2.begin(),_r2.end(),posterior(_r2,w2));
        list<int>::iterator i2 = find(_r2.begin(),_r2.end(),jd);
        list<int>::iterator i3 = find(_r1.begin(),_r1.end(),w1);
        list<int>::iterator i4 = find(_r2.begin(),_r2.end(),w2);
        swap(*i3, *i4);

        _r3.splice(_r3.begin(),_r2,i1,i2);
        _r3.reverse();
        _r1.splice(_r1.end(),_r3);
        _r1.swap(_r2);

        _dem[route1] = in_fin(_c,_r1,_cap.size());
        _dem[route2] = in_fin(_c,_r2,_cap.size());
        _cap[w1-1] += _dem[route1];
        _cap[w2-1] += _dem[route2];
    }
    if((jd <= _cap.size()) && (io > _cap.size()) && (id > _cap.size())) {
        _t.add(w1,pw2);
        _t.add(w2,aw1);
        _cand[w1-1][pw2-1] = 1;
        _cand[w2-1][aw1-1] = 1;

        _cap[w1-1] -= _dem[route1];
        _cap[w2-1] -= _dem[route2];
        _dem[route1] = 0;
        _dem[route2] = 0;

        list<int>::iterator i1 = find(_r1.begin(),_r1.end(),posterior(_r1,w1));
        list<int>::iterator i2 = find(_r1.begin(),_r1.end(),id);
        list<int>::iterator i3 = find(_r1.begin(),_r1.end(),w1);
        list<int>::iterator i4 = find(_r2.begin(),_r2.end(),w2);
        swap(*i3, *i4);

        _r3.splice(_r3.begin(),_r1,i1,i2);
        _r3.reverse();
        _r2.splice(_r2.end(),_r3);
        _r1.swap(_r2);

        _dem[route1] = in_fin(_c,_r1,_cap.size());
        _dem[route2] = in_fin(_c,_r2,_cap.size());
        _cap[w1-1] += _dem[route1];
        _cap[w2-1] += _dem[route2];
    }
}

// Funcion para diferent depot 4
void d_depot4 (tabu &_t,vector < vector <int> > &_cand,list<int> &_r1,list<int> &_r2,list<int> &_r3,list<int> &_r4,
               size_t io,size_t id,size_t jo,size_t jd,int aw2,int pw1,int pw2,int w1,int w2,
               int route1,int route2,vector <float> &_dem,vector <float> &_cap,vector <nclients> &_c){

    _t.add(io,jo);
    _t.add(id,jd);
    _t.add(w1,aw2);
    _t.add(w2,pw1);

    _cand[io-1][jo-1] = 1;
    _cand[id-1][jd-1] = 1;
    _cand[w1-1][aw2-1] = 1;
    _cand[w2-1][pw1-1] = 1;

    _cap[w1-1] -= _dem[route1];
    _cap[w2-1] -= _dem[route2];
    _dem[route1] = 0;
    _dem[route2] = 0;

    list<int>::iterator i1 = find(_r2.begin(),_r2.end(),pw2);
    list<int>::iterator i2 = find(_r2.begin(),_r2.end(),jd);
    list<int>::iterator i3 = find(_r1.begin(),_r1.end(),id);
    list<int>::iterator i4 = find(_r1.begin(),_r1.end(),pw1);

    _r3.splice(_r3.begin(),_r2,i1,i2);
    _r3.splice(_r3.end(),_r1,i3,_r1.end());
    _r4.splice(_r4.end(),_r2,i2,_r2.end());
    _r4.splice(_r4.begin(),_r1,i4,_r1.end());
    _r3.reverse();
    _r4.reverse();
    _r1.splice(_r1.end(),_r4);
    _r2.splice(_r2.end(),_r3);

    _dem[route1] = in_fin(_c,_r1,_cap.size());
    _dem[route2] = in_fin(_c,_r2,_cap.size());
    _cap[w1-1] += _dem[route1];
    _cap[w2-1] += _dem[route2];
}

// Funcion para diferent depot 5
void d_depot5 (tabu &_t,vector < vector <int> > &_cand,list<int> &_r1,list<int> &_r2,list<int> &_r3,list<int> &_r4,
               size_t io,size_t id,size_t jo,size_t jd,int aw1,int pw1,int pw2,int w1,int w2,
               int route1,int route2,vector <float> &_dem,vector <float> &_cap,vector <nclients> &_c){

    _t.add(io,jo);
    _t.add(id,jd);
    _t.add(w1,pw2);
    _t.add(w2,aw1);

    _cand[io-1][jo-1] = 1;
    _cand[id-1][jd-1] = 1;
    _cand[w1-1][pw2-1] = 1;
    _cand[w2-1][aw1-1] = 1;

    _cap[w1-1] -= _dem[route1];
    _cap[w2-1] -= _dem[route2];
    _dem[route1] = 0;
    _dem[route2] = 0;

    list<int>::iterator i1 = find(_r2.begin(),_r2.end(),pw2);
    list<int>::iterator i2 = find(_r2.begin(),_r2.end(),jd);
    list<int>::iterator i3 = find(_r1.begin(),_r1.end(),id);
    list<int>::iterator i4 = find(_r1.begin(),_r1.end(),pw1);

    _r3.splice(_r3.begin(),_r2,i1,i2);
    _r3.splice(_r3.end(),_r1,i3,_r1.end());
    _r4.splice(_r4.end(),_r2,i2,_r2.end());
    _r4.splice(_r4.begin(),_r1,i4,_r1.end());
    _r1.splice(_r1.end(),_r3);
    _r2.splice(_r2.end(),_r4);

    _dem[route1] = in_fin(_c,_r1,_cap.size());
    _dem[route2] = in_fin(_c,_r2,_cap.size());
    _cap[w1-1] += _dem[route1];
    _cap[w2-1] += _dem[route2];
}

// Funcion para ver ruta
void ver_ruta(vector < list <int> > &_t){
    for (size_t i = 0; i < _t.size(); ++i) {
        cout << " Ruta " << i+1 << " :";
        list<int>::iterator it;
        for (it = _t[i].begin(); it != _t[i].end(); ++it) {
            cout << *it << " ";
        }
        cout << endl;
    }
    cin.get();
}

// Funcion para VRPH library
void vrphl(char *input=NULL,char *output=NULL,char *initial=NULL) {
    bool fruity_loop = true;
    bool enable_sa = true;
    bool enable_rtr = true;
    bool enable_ej = false;

    if (!input) return;

    double tini = clock();

    int n = VRPGetDimension(input);
    VRP V(n);
    V.read_TSPLIB_file(input);

    int num_ejected = n-2 < 20 ? n-2 : 20;
    int num_trials = 1000;

    double best_obj = -1;
    int *best_sol = new int[n+2];
    if (initial) {
        V.read_solution_file(initial);
        best_obj = V.get_total_route_length();
        V.export_solution_buff(best_sol);
        cout << "VRPH File " << V.get_total_route_length()
             << " time " << (clock()-tini)/CLOCKS_PER_SEC << endl;
    } else {
        ClarkeWright CW(n);
        CW.Construct(&V,
                     1.0, // lambda
                     false); // verbose
        best_obj = V.get_total_route_length();
        V.export_solution_buff(best_sol);
        cout << "VRPH ClarkeWright " << V.get_total_route_length()
             << " time " << (clock()-tini)/CLOCKS_PER_SEC << endl;
    }

    if (fruity_loop) cout << "VRPH fruity_loop mode" << endl;
    do {
        double current_sol = best_obj;

        do {
            if ( ! enable_sa ) break;
            V.SA_solve(ONE_POINT_MOVE+TWO_POINT_MOVE+TWO_OPT, // heuristics
                       2, // starting_temperature
                       .99, // cooling_ratio
                       2, // iters_per_loop
                       200, // num_loops
                       10, // nlist_size
                       false); // verbose
            if ( best_obj > V.get_total_route_length() ) {
                best_obj = V.get_total_route_length();
                cout << "VRPH SA " << V.get_total_route_length()
                     << " time " << (clock()-tini)/CLOCKS_PER_SEC << endl;
                V.export_solution_buff(best_sol);
            } else {
                V.import_solution_buff(best_sol);
                break;
            }
        } while (fruity_loop);

        do {
            if ( ! enable_rtr ) break;
            V.RTR_solve(ONE_POINT_MOVE+TWO_POINT_MOVE+TWO_OPT+VRPH_USE_NEIGHBOR_LIST, // heuristics
                        30, // intensity
                        5, // max_tries
                        2, // num_perturbs
                        .01, // dev
                        40, // nlist_size
                        VRPH_LI_PERTURB, // perturb_type
                        VRPH_FIRST_ACCEPT, // accept_type
                        false); // verbose
            if ( best_obj > V.get_total_route_length() ) {
                best_obj = V.get_total_route_length();
                cout << "VRPH RTR " << V.get_total_route_length()
                     << " time " << (clock()-tini)/CLOCKS_PER_SEC << endl;
                V.export_solution_buff(best_sol);
            } else {
                V.import_solution_buff(best_sol);
                break;
            }
        } while (fruity_loop);

        do {
            if ( ! enable_ej ) break;
            double current_ej = best_obj;
            int *ejected_buff = new int[n+2];

            for(int i=0;i<num_trials;++i) {
                int r = VRPH_DEPOT;
                while(r == VRPH_DEPOT) r = (int)(lcgrand(11)*(n-1));

                V.eject_neighborhood(r, // j
                                     num_ejected, // num
                                     ejected_buff); // nodelist
                V.inject_set(num_ejected, // num
                             ejected_buff, // nodelist
                             VRPH_REGRET_SEARCH, // rules
                             50); // attempts

                if ( best_obj > V.get_total_route_length() ) {
                    best_obj = V.get_total_route_length();
                    cout << "VRPH EJ " << i << " Regret " << V.get_total_route_length()
                         << " time " << (clock()-tini)/CLOCKS_PER_SEC << endl;
                    V.export_solution_buff(best_sol);
                } else {
                    V.import_solution_buff(best_sol);
                }
            }
            delete[] ejected_buff;

            if (current_ej <= best_obj+1e-4) break;
        } while (fruity_loop);

        if (current_sol <= best_obj+1e-4) break;
    } while (fruity_loop);

    delete [] best_sol;
    cout << "* VRPH"
         << " route/best "
         << V.get_total_route_length() << "/" << V.get_best_known()
         << " time " << (clock()-tini)/CLOCKS_PER_SEC << endl;
    if (output) {
        V.write_solution_file(output);
    }
}

/********Function to calculate the volume of a type of waste per customer************/
//returns volume as integer

int volumeWasteType(vector<mdcvfp::box> boxes, int wasteType, int customerId, mdcvfp mydata){

    if(customerId>=mydata.ncustomers){
        if(customerId==15 || customerId==16 || customerId==17){
            customerId=0;
        }else{
            customerId=ceil((customerId-mydata.ncustomers)/3.0);
        }
    }

    int lengthStructBoxes=boxes.size();
    int totalVolume{0};
    for(int i{0};i<lengthStructBoxes;i++){
        if(boxes[i].customerId==customerId && boxes[i].wasteType==wasteType){
            totalVolume+=boxes[i].heightBox*boxes[i].widthBox*boxes[i].lengthBox;
        }
    }
    return totalVolume;
}
/************************************************************************************/

/*****Function to find the combination that maximize the available space in vehicle*****/
//Returns two vectors of integers. One for the combination and another with the available space
//after of loading the vehicle with optimal combination

void MakeCombiUtil(vector<vector<int> >& ans,
    vector<int>& tmp, int n, int left, int k)
{
    // Pushing this vector to a vector of vector
    if (k == 0) {

        ans.push_back(tmp);
        return;

    }

    // i iterates from left to n. First time
    // left will be 1
    for (int i = left; i <= n; ++i)
    {

        tmp.push_back(i);
        MakeCombiUtil(ans, tmp, n, i + 1, k - 1);
        // Popping out last inserted element
        // from the vector
        tmp.pop_back();

    }
}

// Prints all combinations of size k of numbers
// from 1 to n.
vector<vector<int> > MakeCombi(int n, int k)
{
    vector<vector<int> > ans;
    vector<int> tmp;
    MakeCombiUtil(ans, tmp, n, 1, k);
    return ans;
}

// this function returns the optimal combination of types of waste that minimize the unused space in th vehcile when
// it can't load the entire demand

vector <vector<int>> CombinationToLoad(int available_capacity, vector<mdcvfp::box> boxes, int number_type_wastes, int customer_id,
                                       mdcvfp mydata){

    int gap{45000};
    int optimal_combination_demand{0};
    vector <vector <int>> result;
    int passing_test{0};
    for(int i{1};i<number_type_wastes;i++){

        vector <vector <int>> partial_combinations = MakeCombi(number_type_wastes, i);

        for(int j{0};j<partial_combinations.size();j++){

            int partial_demand{0};

            for(int k{0};k<partial_combinations[j].size();k++){

                partial_demand+=volumeWasteType(boxes,partial_combinations[j][k],customer_id,mydata);

            }

            if((float(available_capacity)-float(partial_demand))>=0.000000001 &&
               partial_demand > 0){

                if((available_capacity-partial_demand)<gap){

                    if(result.size()==0){

                        vector <int> temp(number_type_wastes,0);

                        for(int l{0};l<partial_combinations[j].size();l++){

                            temp[partial_combinations[j][l]-1]=1;

                        }

                        result.push_back(temp);
                        gap=available_capacity-partial_demand;
                        optimal_combination_demand=partial_demand;
                        passing_test+=1;

                    }
                    else{

                        result.pop_back();
                        vector <int> temp(number_type_wastes,0);

                        for(int l{0};l<partial_combinations[j].size();l++){

                            temp[partial_combinations[j][l]-1]=1;

                        }

                        result.push_back(temp);
                        gap=available_capacity-partial_demand;
                        optimal_combination_demand=partial_demand;
                        passing_test+=1;

                    }
                }
            }
        }
    }

    if(passing_test==0 || optimal_combination_demand==0){

        return vector<vector<int>>();

    }else{

        vector<int> optimal_combination_demand_vector;
        optimal_combination_demand_vector.push_back(optimal_combination_demand);
        result.push_back(optimal_combination_demand_vector);
        return result;

    }
}
/**********************************************************************************/

/*****Function that changes 0 for 1 and 1 for 0 in a vector of recollection types*******/
// Returns a vector of integer

vector <int> OneForZeros(vector<int> types_vector){

    vector <int> result;
    for(int i{0};i<types_vector.size();i++){

        if(types_vector[i]==0){

            result.push_back(1);

        }else{

            result.push_back(0);

        }
    }

    return result;

}
/****************************************************************************************/

/*****************Function that generates n points around a costumer*********************/

vector< vector<int> > GenerateNPoints(int number_points, int radio, int x, int y){

    vector< vector<int> > result;
    for(int i{0};i<number_points;i++){

        vector<int> point;
        int x_1, y_1;
        int angle = rand() % 361;

        if(angle>=0 && angle<=90){

            x_1=radio*cos(angle*PI/180.0);
            point.push_back(int(x+x_1));
            y_1=radio*sin(angle*PI/180.0);
            point.push_back(int(y+y_1));

        }else if(angle>90 && angle<180){

            angle=180-angle;
            x_1=radio*cos(angle*PI/180.0);
            point.push_back(int(x-x_1));
            y_1=radio*sin(angle*PI/180.0);
            point.push_back(int(y+y_1));

        }else if(angle>=180 && angle<=70){

            angle=270-angle;
            x_1=radio*sin(angle*PI/180.0);
            point.push_back(int(x-x_1));
            y_1=radio*cos(angle*PI/180.0);
            point.push_back(int(y-y_1));

        }else{

            angle=360-angle;
            x_1=radio*cos(angle*PI / 180.0);
            point.push_back(int(x+x_1));
            y_1=radio*sin(angle*PI / 180.0);
            point.push_back(int(y-y_1));

        }

        result.push_back(point);

    }

    return result;

}
/*********************************************************************************************/

/***************Function that returns the relocation points for each customer*****************/

vector <int> RelocationPoints(int customer_id, vector <mdcvfp::relocation_point> relocation_points){

    vector <int> result;
    for(int reloc_point{0};reloc_point<relocation_points.size();reloc_point++){

        if(relocation_points[reloc_point].customerId==customer_id){

            result.push_back(reloc_point);

        }
    }

    return result;

}

/********************Function to extract the vertices vectors of a route**********************/

vector<vector<int>> Vertices(vector <int> route,vector <nclients> data_customers,vector <ndepots> data_depots){

    vector <vector <int>> result;
    vector <int> x_vertices;
    vector <int> y_vertices;
    x_vertices.push_back(data_depots[0].x);
    y_vertices.push_back(data_depots[0].y);
    for(int customer{0};customer<route.size();customer++){

            x_vertices.push_back(data_customers[route[customer]-1].x);
            y_vertices.push_back(data_customers[route[customer]-1].y);

    }

    result.push_back(x_vertices);
    result.push_back(y_vertices);
    return result;

}
/*********************************************************************************************/

/*********************Function to know if a point is inside a route********************/

int InsideARoute(int nvert, int *vertx, int *verty, int testx, int testy){

  int i, j, c = 0;
  for (i = 0, j = nvert-1; i < nvert; j = i++) {

    if ( ((verty[i]>testy) != (verty[j]>testy)) &&
     (testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]) )

       c = !c;

  }

  return c;

}
/***************************************************************************************/

/***********************Distance from a point to a line**********************************************/

float DistanceToALine(int x, int y, int last_customer, bool is_last_depot, int next_customer, bool is_next_depot,
                       vector <nclients> data_customers, vector <ndepots> data_depots ){

    float distance{0};
    int x_last_customer{0};
    int y_last_customer{0};
    int x_next_customer{0};
    int y_next_customer{0};
    if (is_last_depot==false){

        x_last_customer = data_customers[last_customer-1].x;
        y_last_customer = data_customers[last_customer-1].y;

    }else{

        x_last_customer = data_depots[last_customer].x;
        y_last_customer = data_depots[last_customer].y;

    }

    if (is_next_depot==false){

        x_next_customer = data_customers[next_customer-1].x;
        y_next_customer = data_customers[next_customer-1].y;

    }else{

        x_next_customer = data_depots[next_customer].x;
        y_next_customer = data_depots[next_customer].y;

    }

    if(y_last_customer==y_next_customer){

        distance=abs(y-y_last_customer);

    }else if(x_last_customer==x_next_customer){

        distance=abs(x-x_last_customer);

    }else{

        float slope = (float(y_next_customer-y_last_customer)/float(x_next_customer-x_last_customer));
        float k = (-slope*x_last_customer) + y_last_customer;
        distance = abs(k+(slope*float(x))-float(y))/sqrt(1+pow(slope,2));

    }

    return distance;

}

/*****************************Function to convert a vector to an array int[]****************************/

int* VectorToArray(vector <int> vector_to_convert){

    int result_array[vector_to_convert.size()];
    for(int vector_item{0};vector_item<vector_to_convert.size();vector_item++){

        result_array[vector_item]=vector_to_convert[vector_item];
    }

    return result_array;

}
/*******************************************************************************************************/

/****************************Function to print matrices*************************************************/

int PrintMatrix(vector <vector <int>> matrix, string symbol_in, string symbol_out){

    cout<<symbol_in;
    for(int row{0};row<matrix.size();row++){

        cout<<symbol_in;

        for(int col{0};col<matrix[row].size();col++){

            if(col!=matrix[row].size()-1){

                cout<<matrix[row][col]<<", ";

            }
            else{

                cout<<matrix[row][col];

            }
        }

        cout<<symbol_out<<endl;

    }

    cout<<symbol_out;
    cout<<endl;
    return 0;

}
/*******************************************************************************************************/

/****************************Function to print distance matrix*************************************************/

int PrintDistanceMatrix(vector <vector <dist_t>> matrix, string symbol_in, string symbol_out){

    cout<<symbol_in;
    for(int row{0};row<matrix.size();row++){

        cout<<symbol_in;

        for(int col{0};col<matrix[row].size();col++){

            if(col!=matrix[row].size()-1){

                cout<<matrix[row][col]<<", ";

            }
            else{

                cout<<matrix[row][col];

            }
        }

        cout<<symbol_out<<","<<endl;

    }

    cout<<symbol_out;
    cout<<endl;
    return 0;

}
/*******************************************************************************************************/



/****************************Function to know the cluster of a customer****************************************/

vector<int> ClusterOfCustomer(vector<vector<int>> solution, int customer){
    vector<int> result;
    for(int cluster{0};cluster<solution.size();cluster++){
        for(int cust{0};cust<solution[cluster].size();cust++){
            if(solution[cluster][cust]==customer){
                result.push_back(cluster);
                result.push_back(cust);
                break;
            }
        }
    }
    return result;
}
/**************************************************************************************************************/

/****************************Wrapper estimator*****************************************************************/

dist_t WrapperEst(vector<vector<int>> solution, int customer_node, vector<ndepots> D_Data, vector<nclients> C_Data,
                  mdcvfp mydata, int Ncostumers){

        vector<int> info_cluster_customer=ClusterOfCustomer(solution,customer_node); // Cluster and position of customer
        int cluster=info_cluster_customer[0];
        int customer=info_cluster_customer[1];

        vector <vector<int>> vector_result;
        vector_result = Vertices(solution[cluster],C_Data,D_Data); // x and y coordinates are extracted
        vector <int> x_vertices = vector_result[0]; // x coordinate
        vector <int> y_vertices = vector_result[1]; // y coordinate
        int x_vertices_2[x_vertices.size()];
        copy(x_vertices.begin(),x_vertices.end(),x_vertices_2); // x coordinate to Array
        int y_vertices_2[y_vertices.size()];
        copy(y_vertices.begin(),y_vertices.end(),y_vertices_2); // y coordinate to Array

        vector<int> relocation_points_customer = RelocationPoints(customer_node,mydata.relocation_points);

        int points_inside_route{0};
        vector <vector<int>> reloc_points_inside_route;

        for(int relocation_point{0};relocation_point<relocation_points_customer.size();relocation_point++){

            vector<int> temp;

            // verify if the point is inside the route
            int inside_the_route=InsideARoute(solution[cluster].size()+1,x_vertices_2,y_vertices_2,
                                              mydata.relocation_points[relocation_points_customer[relocation_point]].x,
                                              mydata.relocation_points[relocation_points_customer[relocation_point]].y);

            // if point is inside the route, then it is added to a vector
            if(inside_the_route==1){

                points_inside_route+=inside_the_route;
                temp.push_back(relocation_points_customer[relocation_point]);
                temp.push_back(relocation_point);
                reloc_points_inside_route.push_back(temp);
            }
        }

        if(points_inside_route==0){return 999999.0;}

        else{


        }
}

/**************************************************************************************************************/

/****************************Wrapper operator*****************************************************************/

void WrapperOpt(vector<vector<int>> solution, int customer_node, vector<ndepots> D_Data, vector<nclients> C_Data,
                vector<vector<int>> relocation_decision_matrix, mdcvfp mydata, int Ncostumers){

    vector<int> info_cluster_customer=ClusterOfCustomer(solution,customer_node); // Cluster and position of customer
    int cluster=info_cluster_customer[0];
    int customer=info_cluster_customer[1];

    vector <vector<int>> vector_result;
    vector_result = Vertices(solution[cluster],C_Data,D_Data); // x and y coordinates are extracted
    vector <int> x_vertices = vector_result[0]; // x coordinate
    vector <int> y_vertices = vector_result[1]; // y coordinate
    int x_vertices_2[x_vertices.size()];
    copy(x_vertices.begin(),x_vertices.end(),x_vertices_2); // x coordinate to Array
    int y_vertices_2[y_vertices.size()];
    copy(y_vertices.begin(),y_vertices.end(),y_vertices_2); // y coordinate to Array

    // Relocations points of the current customer
    vector <int> relocation_points_customer = RelocationPoints(solution[cluster][customer]-1,
                                                               mydata.relocation_points);

    int points_inside_route{0};
    vector <vector<int>> reloc_points_inside_route;

    for(int relocation_point{0};relocation_point<relocation_points_customer.size();relocation_point++){

            vector<int> temp;

            // verify if the point is inside the route
            int inside_the_route=InsideARoute(solution[cluster].size()+1,x_vertices_2,y_vertices_2,
                                              mydata.relocation_points[relocation_points_customer[relocation_point]].x,
                                              mydata.relocation_points[relocation_points_customer[relocation_point]].y);

            // if point is inside the route, then it is added to a vector
            if(inside_the_route==1){

                points_inside_route+=inside_the_route;
                temp.push_back(relocation_points_customer[relocation_point]);
                temp.push_back(relocation_point);
                reloc_points_inside_route.push_back(temp);

            }
    }


    // if there is only one point, modify the route
    if(points_inside_route==1){
        /*
        C_Data[size_C_Data].id=size_C_Data+1;
        C_Data[size_C_Data].x=mydata.relocation_points[reloc_points_inside_route[0][0]].x;
        C_Data[size_C_Data].y=mydata.relocation_points[reloc_points_inside_route[0][0]].y;
        C_Data[size_C_Data].dem=C_Data[post_lkh_clusters[cluster][customer]-1].dem;
        */
        relocation_decision_matrix[solution[cluster][customer]-1][reloc_points_inside_route[0][1]]=1;
        solution[cluster][customer]=(reloc_points_inside_route[0][0]+1+Ncostumers);
        //size_C_Data+=1;
        vector_result = Vertices(solution[cluster],C_Data,D_Data);
        x_vertices=vector_result[0];
        y_vertices=vector_result[1];
        copy(x_vertices.begin(),x_vertices.end(),x_vertices_2);
        copy(y_vertices.begin(),y_vertices.end(),y_vertices_2);

    // if there are more points, verify what point is closer and modify the route
    }else if(points_inside_route>1){

        float min_distance{999999}; // variable to save the minimum distance
        int best_point{0}; // variable to save the best relocation point

        for(int point{0};point<reloc_points_inside_route.size();point++){

            // if the current customer is the first customer of the cluster
            if(customer==0){

                float temp_distance=DistanceToALine(mydata.relocation_points[reloc_points_inside_route[point][0]].x,
                                                  mydata.relocation_points[reloc_points_inside_route[point][0]].y,
                                                  0,true,solution[cluster][customer+1],false,C_Data,D_Data);

                if(temp_distance<min_distance){
                    min_distance=temp_distance;
                    best_point=point;

                }
            }

            // if the current customer is the last customer of the cluster
            else if(customer==solution[cluster].size()-1){

                float temp_distance=DistanceToALine(mydata.relocation_points[reloc_points_inside_route[point][0]].x,
                                                  mydata.relocation_points[reloc_points_inside_route[point][0]].y,
                                                  solution[cluster][customer-1],false,0,true,C_Data,D_Data);

                if(temp_distance<min_distance){

                    min_distance=temp_distance;
                    best_point=point;

                }
            }

            else{

                float temp_distance=DistanceToALine(mydata.relocation_points[reloc_points_inside_route[point][0]].x,
                                                  mydata.relocation_points[reloc_points_inside_route[point][0]].y,
                                                  solution[cluster][customer-1],false,
                                                  solution[cluster][customer+1],false,C_Data,D_Data);

                if(temp_distance<min_distance){

                    min_distance=temp_distance;
                    best_point=point;

                }
            }
            /*
            C_Data[size_C_Data].id=size_C_Data+1;
            C_Data[size_C_Data].x=mydata.relocation_points[reloc_points_inside_route[best_point][0]].x;
            C_Data[size_C_Data].y=mydata.relocation_points[reloc_points_inside_route[best_point][0]].y;
            C_Data[size_C_Data].dem=C_Data[post_lkh_clusters[cluster][customer]-1].dem;
            relocation_decision_matrix[post_lkh_clusters[cluster][customer]-1][reloc_points_inside_route[best_point][1]]=1;
            post_lkh_clusters[cluster][customer]=size_C_Data+1;
            size_C_Data+=1;
            vector_result = Vertices(post_lkh_clusters[cluster],C_Data,D_Data);
            cout<<"TEST"<<endl;
            x_vertices=vector_result[0];
            y_vertices=vector_result[1];
            copy(x_vertices.begin(),x_vertices.end(),x_vertices_2);
            copy(y_vertices.begin(),y_vertices.end(),y_vertices_2);*/

        }
        /*
        C_Data[size_C_Data].id=size_C_Data+1;
        C_Data[size_C_Data].x=mydata.relocation_points[reloc_points_inside_route[best_point][0]].x;
        C_Data[size_C_Data].y=mydata.relocation_points[reloc_points_inside_route[best_point][0]].y;
        C_Data[size_C_Data].dem=C_Data[post_lkh_clusters[cluster][customer]-1].dem;
        */
        relocation_decision_matrix[solution[cluster][customer]-1][reloc_points_inside_route[best_point][1]]=1;
        solution[cluster][customer]=(reloc_points_inside_route[best_point][0]+1+Ncostumers);
        //size_C_Data+=1;
        vector_result = Vertices(solution[cluster],C_Data,D_Data);
        x_vertices=vector_result[0];
        y_vertices=vector_result[1];
        copy(x_vertices.begin(),x_vertices.end(),x_vertices_2);
        copy(y_vertices.begin(),y_vertices.end(),y_vertices_2);

    }

    // vector of points is deleted
    reloc_points_inside_route.clear();

}
/*************************************************************************************************************/

/************************************Insertion operator estimator***********************************************/

dist_t InsertionEstimator(vector<vector<dist_t>> Distancias, int last_customer, int current_customer, int next_customer,
                       mdcvfp mydata){

        vector<int> reloc_points;
        int initial_customer{0};
        int initial_reloc_point{0};

        if(current_customer>mydata.ncustomers){

            initial_customer = ceil((current_customer-mydata.ncustomers)/3.0);
            initial_reloc_point=mydata.ncustomers+1+3*(initial_customer-1);
            int temp=initial_reloc_point;

            for(int point1{0};point1<3;point1++){

                if(temp==current_customer){

                    reloc_points.push_back(initial_customer);

                }else{

                    reloc_points.push_back(temp);

                }

                temp++;

            }

        }else{

            initial_reloc_point=mydata.ncustomers+1+3*(current_customer-1);

            for(int point{0};point<3;point++){

                reloc_points.push_back(initial_reloc_point);
                initial_reloc_point++;

            }
        }

        dist_t result{9999.0};
        int best_point{current_customer};

        for(int point2{0};point2<reloc_points.size();point2++){

            dist_t temp=Distancias[last_customer][reloc_points[point2]]+Distancias[reloc_points[point2]][next_customer]-\
                        Distancias[last_customer][current_customer]-Distancias[current_customer][next_customer];

            if(temp<result){

                result=temp;
                best_point=reloc_points[point2];

            }

        }

        dist_t reloc_cost{0.0};

        if(current_customer>mydata.ncustomers && best_point<=mydata.ncustomers){

            reloc_cost=-1*mydata.penalty_reloc;

        }else if(current_customer>mydata.ncustomers && best_point>mydata.ncustomers){

            reloc_cost=0.0;

        }else if(current_customer<=mydata.ncustomers && best_point>mydata.ncustomers){

            reloc_cost=mydata.penalty_reloc;

        }else{

            reloc_cost=0.0;

        }

        return result+reloc_cost;
}

/****************************************************************************************************************************/

/******************************************Insertion operator****************************************************************/

struct ResultInsertion{
    vector<vector<int>> solution;
    dist_t result;
    dist_t reloc_cost;
};

ResultInsertion Insertion(vector<vector<int>> solution, vector<vector<dist_t>> Distancias, int last_customer, int current_customer,
               int next_customer, mdcvfp mydata){

    ResultInsertion data_to_return;

    data_to_return.solution=vector<vector<int>>();
    data_to_return.result=0.0;
    data_to_return.reloc_cost=0.0;

    vector<int> info_cluster=ClusterOfCustomer(solution,current_customer);
    vector<int> reloc_points;
    int initial_customer{0};
    int initial_reloc_point{0};

    if(current_customer>mydata.ncustomers){

        initial_customer = ceil((current_customer-mydata.ncustomers)/3.0);
        initial_reloc_point=mydata.ncustomers+1+3*(initial_customer-1);
        int temp=initial_reloc_point;

        for(int point1{0};point1<3;point1++){

            if(temp==current_customer){

                reloc_points.push_back(initial_customer);

            }else{

                reloc_points.push_back(temp);

            }

            temp++;

        }

    }else{

        initial_reloc_point=mydata.ncustomers+1+3*(current_customer-1);

        for(int point{0};point<3;point++){

            reloc_points.push_back(initial_reloc_point);
            initial_reloc_point++;

        }
    }

    dist_t result{9999.0};
    int best_point{current_customer};

    for(int point2{0};point2<reloc_points.size();point2++){

        dist_t temp=Distancias[last_customer][reloc_points[point2]]+Distancias[reloc_points[point2]][next_customer]-\
                    Distancias[last_customer][current_customer]-Distancias[current_customer][next_customer];

        if(temp<result){

            result=temp;
            solution[info_cluster[0]][info_cluster[1]]=reloc_points[point2];
            best_point=reloc_points[point2];

        }

    }

    dist_t reloc_cost{0.0};

    if(current_customer>mydata.ncustomers && best_point<=mydata.ncustomers){

        reloc_cost=-1*mydata.penalty_reloc;

    }else if(current_customer>mydata.ncustomers && best_point>mydata.ncustomers){

        reloc_cost=0.0;

    }else if(current_customer<=mydata.ncustomers && best_point>mydata.ncustomers){

        reloc_cost=mydata.penalty_reloc;

    }else{

        reloc_cost=0.0;

    }

    data_to_return.solution=solution;
    data_to_return.result=result+reloc_cost;
    data_to_return.reloc_cost=reloc_cost;

    return data_to_return;
}

/*****************************************************************************************************************************/

/**********************************Function to know if solution exceeds capacity**********************************************/

dist_t ExceedsCapacity(vector<vector<int>> solution, list<vector<vector<int>>> demand, mdcvfp mydata){

    //cout<<"-"<<solution.size()<<"-"<<demand.size()<<endl;

    dist_t result{0.0};

    list<vector<vector<int>>>::iterator itRecTypes = demand.begin();

    for(int clust{0};clust<solution.size();clust++){

        vector<vector<int>> temp_rec_vector = (*itRecTypes);

        int recollected{0};
        //cout<<"** "<<solution[clust].size()<<" ** "<<temp_rec_vector.size()<<endl;

        for(int custo{0};custo<solution[clust].size();custo++){

            for(int waste_type{1};waste_type<=mydata.numberTypeWaste;waste_type++){

                if(temp_rec_vector[custo][waste_type-1]==1){

                    //cout<<"Check ok in ExceedsCapacity"<<endl;
                    recollected+=volumeWasteType(mydata.boxes,waste_type,solution[clust][custo]-1,mydata);
                    //cout<<"The result of function is: "<<volumeWasteType(mydata.boxes,waste_type,solution[clust][custo]-1,mydata)<<endl;
                }
            }
        }
        //cout<<recollected<<" --------------- "<<mydata.volumeVehicle<<endl;
        //cout<<">>>>>>>>>>>>>>>>>>>>>>>"<<endl;
        if(float(recollected)>mydata.volumeVehicle){
            //cout<<"ENTROOOOOOOOOOO"<<endl;
            dist_t temp=(float(recollected)-mydata.volumeVehicle)/mydata.factor_demand_dist;
            result+=temp;
        }

        ++itRecTypes;

    }

    return result;
}

/***************************************************************************************************************************/

/***************************Function to calculate the objetive function*********************************/

// This function returns a vector with two floats. The first float corresponds to the value of the f.o
// The second float corresponds to the feasibility of the solution: 1.0 feasible, 0.0 infeasible.

vector<dist_t> FunctionObjective (vector<vector<int>> solution, vector < vector <dist_t> > distances,
                                  list<vector<vector<int>>> demand, mdcvfp mydata){

    vector<dist_t> result;
    dist_t total_distance{0.0};

    for(int cluster{0};cluster<solution.size();cluster++){

        total_distance+=distances[0][solution[cluster][0]];
        total_distance+=distances[solution[cluster][solution[cluster].size()-1]][0];

        for(int customer{1};customer<solution[cluster].size();customer++){

            total_distance+=distances[solution[cluster][customer-1]][solution[cluster][customer]];
            //cout<<"Cluster "<<cluster<<" customer "<<solution[cluster][customer]<<". The distance is = "<<total_distance<<endl;

        }
    }
    dist_t ex=ExceedsCapacity(solution,demand,mydata);

    cout<<"The excess is.... "<<ex<<endl;

    total_distance+=ex;

    result.push_back(total_distance);
    if(ex>0){
        result.push_back(0.0);
    }else{
        result.push_back(1.0);
    }

    return result;

}

/******************************************************************************************************************************/

/*************************************Swap inter-route Estimator************************************************************/

dist_t SwapInterRouteEstimator(int customer_1, int customer_2, vector<vector<int>> solution, vector<vector<dist_t>> Distancias,
                               list<vector<vector<int>>> demand, mdcvfp mydata, dist_t Exceed1, vector<dist_t> excess_clusters,
                               vector<nclients> C_Data){

    //dist_t Exceed1=ExceedsCapacity(solution,demand,mydata);


    vector<int> info_cluster1 = ClusterOfCustomer(solution,customer_1);
    vector<int> info_cluster2 = ClusterOfCustomer(solution,customer_2);

    dist_t result{0.0};

    if(info_cluster1[1]==0 && info_cluster2[1]!=0 && info_cluster2[1]!=(solution[info_cluster2[0]].size()-1)){

        result = Distancias[0][customer_2]+Distancias[customer_2][solution[info_cluster1[0]][info_cluster1[1]+1]]+\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_1]+\
                Distancias[customer_1][solution[info_cluster2[0]][info_cluster2[1]+1]]-\
                Distancias[0][customer_1]-Distancias[customer_1][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_2]-\
                Distancias[customer_2][solution[info_cluster2[0]][info_cluster2[1]+1]];

    }
    if(info_cluster1[1]!=0 && info_cluster2[1]==0 && info_cluster1[1]!=(solution[info_cluster1[0]].size()-1)){

        result = Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_2]+\
                Distancias[customer_2][solution[info_cluster1[0]][info_cluster1[1]+1]]+Distancias[0][customer_1]+\
                Distancias[customer_1][solution[info_cluster2[0]][info_cluster2[1]+1]]-\
                Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_1]-\
                Distancias[customer_1][solution[info_cluster1[0]][info_cluster1[1]+1]]-Distancias[0][customer_2]-\
                Distancias[customer_2][solution[info_cluster2[0]][info_cluster2[1]+1]];


    }
    if(info_cluster1[1]==0 && info_cluster2[1]==0){

        result = Distancias[customer_2][solution[info_cluster1[0]][info_cluster1[1]+1]]+\
                Distancias[customer_1][solution[info_cluster2[0]][info_cluster2[1]+1]]-\
                Distancias[customer_1][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                Distancias[customer_2][solution[info_cluster2[0]][info_cluster2[1]+1]];

    }
    if(info_cluster1[1]==(solution[info_cluster1[0]].size()-1) && info_cluster2[1]!=0 &&
             info_cluster2[1]!=(solution[info_cluster2[0]].size()-1)){

        result = Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_2]+Distancias[customer_2][0]+\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_1]+\
                Distancias[customer_1][solution[info_cluster2[0]][info_cluster2[1]+1]]-\
                Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_1]-Distancias[customer_1][0]-\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_2]-\
                Distancias[customer_2][solution[info_cluster2[0]][info_cluster2[1]+1]];

    }
    if(info_cluster1[1]==(solution[info_cluster1[0]].size()-1) && info_cluster2[1]==0){

        result = Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_2]+\
                Distancias[customer_1][solution[info_cluster2[0]][info_cluster2[1]+1]]-\
                Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_1]-\
                Distancias[customer_2][solution[info_cluster2[0]][info_cluster2[1]+1]];

    }
    if(info_cluster1[1]!=0 && info_cluster2[1]==(solution[info_cluster2[0]].size()-1) &&
             info_cluster1[1]!=(solution[info_cluster1[0]].size()-1)){

        result = Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_2]+\
                Distancias[customer_2][solution[info_cluster1[0]][info_cluster1[1]+1]]+\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_1]+Distancias[customer_1][0]-\
                Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_1]-\
                Distancias[customer_1][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_2]-Distancias[customer_2][0];

    }
    if(info_cluster1[1]==0 && info_cluster2[1]==(solution[info_cluster2[0]].size()-1)){

        result = Distancias[customer_2][solution[info_cluster1[0]][info_cluster1[1]+1]]+\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_1]-\
                Distancias[customer_1][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_2];

    }if(info_cluster1[1]==(solution[info_cluster1[0]].size()-1) && info_cluster2[1]==(solution[info_cluster2[0]].size()-1)){

        result =Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_2]+\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_1]-\
                Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_1]+\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_2];

    }
    if(info_cluster1[1]!=0 && info_cluster1[1]!=(solution[info_cluster1[0]].size()-1) &&
        info_cluster2[1]!=0 && info_cluster2[1]!=(solution[info_cluster2[0]].size()-1)){

        result = Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_2]+\
                Distancias[customer_2][solution[info_cluster1[0]][info_cluster1[1]+1]]+\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_1]+\
                Distancias[customer_1][solution[info_cluster2[0]][info_cluster2[1]+1]]-\
                Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_1]-\
                Distancias[customer_1][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_2]-\
                Distancias[customer_2][solution[info_cluster2[0]][info_cluster2[1]+1]];

    }

    int temp_customer = solution[info_cluster1[0]][info_cluster1[1]];
    solution[info_cluster1[0]][info_cluster1[1]]=customer_2;
    solution[info_cluster2[0]][info_cluster2[1]]=temp_customer;

    //dist_t Exceed2=ExceedsCapacity(solution,demand,mydata);

    dist_t Exceed2{0.0};

    dist_t temp_exc1 = excess_clusters[info_cluster1[0]]-C_Data[customer_1-1].dem+\
                        +C_Data[customer_2-1].dem;
    dist_t temp_exc2 = excess_clusters[info_cluster2[0]]-C_Data[customer_2-1].dem+\
                        +C_Data[customer_1-1].dem;

    excess_clusters[info_cluster1[0]]=temp_exc1;
    excess_clusters[info_cluster2[0]]=temp_exc2;

    for(int i{0};i<excess_clusters.size();i++){

        if(excess_clusters[i]>0.0){

            Exceed2+=excess_clusters[i];
        }
    }

    result=result-Exceed1+(Exceed2*mydata.factor_demand_dist);
    //result=result+Exceed2;

    //cout<<ExceedsCapacity(solution,demand,mydata)<<" THIS IS THE EXCEED....."<<endl;

    return result;
}

/*****************************************************************************************************************************/

/*************************************Swap inter-route Operator************************************************************/

struct ResultSwap{
    vector<vector<int>> solution;
    dist_t result;
    list<vector<vector<int>>> demand;
    dist_t excess;
    vector<dist_t> excess_clusters;
};

ResultSwap SwapInterRoute(int customer_1, int customer_2, vector<vector<int>> solution, vector<vector<dist_t>> Distancias,
                          list<vector<vector<int>>> demand, mdcvfp mydata, dist_t Exceed1, vector<dist_t> excess_clusters,
                          vector<nclients> C_Data){

    //dist_t Exceed1=ExceedsCapacity(solution,demand,mydata);


    ResultSwap data_to_return;

    data_to_return.solution=vector<vector<int>>();
    data_to_return.result=0.0;
    data_to_return.demand=list<vector<vector<int>>>();
    data_to_return.excess=0.0;
    data_to_return.excess_clusters=vector<dist_t>();

    vector<int> info_cluster1 = ClusterOfCustomer(solution,customer_1);
    vector<int> info_cluster2 = ClusterOfCustomer(solution,customer_2);

    //cout<<"INFO CLUSTERS CALCULATED"<<endl;

    dist_t result{0.0};

    if(info_cluster1[1]==0 && info_cluster2[1]!=0 && info_cluster2[1]!=(solution[info_cluster2[0]].size()-1)){

        result = Distancias[0][customer_2]+Distancias[customer_2][solution[info_cluster1[0]][info_cluster1[1]+1]]+\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_1]+\
                Distancias[customer_1][solution[info_cluster2[0]][info_cluster2[1]+1]]-\
                Distancias[0][customer_1]-Distancias[customer_1][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_2]-\
                Distancias[customer_2][solution[info_cluster2[0]][info_cluster2[1]+1]];

    }
    if(info_cluster1[1]!=0 && info_cluster2[1]==0 && info_cluster1[1]!=(solution[info_cluster1[0]].size()-1)){

        result = Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_2]+\
                Distancias[customer_2][solution[info_cluster1[0]][info_cluster1[1]+1]]+Distancias[0][customer_1]+\
                Distancias[customer_1][solution[info_cluster2[0]][info_cluster2[1]+1]]-\
                Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_1]-\
                Distancias[customer_1][solution[info_cluster1[0]][info_cluster1[1]+1]]-Distancias[0][customer_2]-\
                Distancias[customer_2][solution[info_cluster2[0]][info_cluster2[1]+1]];


    }
    if(info_cluster1[1]==0 && info_cluster2[1]==0){

        result = Distancias[customer_2][solution[info_cluster1[0]][info_cluster1[1]+1]]+\
                Distancias[customer_1][solution[info_cluster2[0]][info_cluster2[1]+1]]-\
                Distancias[customer_1][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                Distancias[customer_2][solution[info_cluster2[0]][info_cluster2[1]+1]];

    }
    if(info_cluster1[1]==(solution[info_cluster1[0]].size()-1) && info_cluster2[1]!=0 &&
             info_cluster2[1]!=(solution[info_cluster2[0]].size()-1)){

        result = Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_2]+Distancias[customer_2][0]+\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_1]+\
                Distancias[customer_1][solution[info_cluster2[0]][info_cluster2[1]+1]]-\
                Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_1]-Distancias[customer_1][0]-\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_2]-\
                Distancias[customer_2][solution[info_cluster2[0]][info_cluster2[1]+1]];

    }
    if(info_cluster1[1]==(solution[info_cluster1[0]].size()-1) && info_cluster2[1]==0){

        result = Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_2]+\
                Distancias[customer_1][solution[info_cluster2[0]][info_cluster2[1]+1]]-\
                Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_1]-\
                Distancias[customer_2][solution[info_cluster2[0]][info_cluster2[1]+1]];

    }
    if(info_cluster1[1]!=0 && info_cluster2[1]==(solution[info_cluster2[0]].size()-1) &&
             info_cluster1[1]!=(solution[info_cluster1[0]].size()-1)){

        result = Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_2]+\
                Distancias[customer_2][solution[info_cluster1[0]][info_cluster1[1]+1]]+\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_1]+Distancias[customer_1][0]-\
                Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_1]-\
                Distancias[customer_1][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_2]-Distancias[customer_2][0];

    }
    if(info_cluster1[1]==0 && info_cluster2[1]==(solution[info_cluster2[0]].size()-1)){

        result = Distancias[customer_2][solution[info_cluster1[0]][info_cluster1[1]+1]]+\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_1]-\
                Distancias[customer_1][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_2];

    }if(info_cluster1[1]==(solution[info_cluster1[0]].size()-1) && info_cluster2[1]==(solution[info_cluster2[0]].size()-1)){

        result =Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_2]+\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_1]-\
                Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_1]+\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_2];

    }
    if(info_cluster1[1]!=0 && info_cluster1[1]!=(solution[info_cluster1[0]].size()-1) &&
        info_cluster2[1]!=0 && info_cluster2[1]!=(solution[info_cluster2[0]].size()-1)){

        result = Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_2]+\
                Distancias[customer_2][solution[info_cluster1[0]][info_cluster1[1]+1]]+\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_1]+\
                Distancias[customer_1][solution[info_cluster2[0]][info_cluster2[1]+1]]-\
                Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_1]-\
                Distancias[customer_1][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_2]-\
                Distancias[customer_2][solution[info_cluster2[0]][info_cluster2[1]+1]];

    }

    //cout<<"RESULT CALCULATED!"<<endl;
    int temp_customer = solution[info_cluster1[0]][info_cluster1[1]];
    solution[info_cluster1[0]][info_cluster1[1]]=customer_2;
    solution[info_cluster2[0]][info_cluster2[1]]=temp_customer;

    data_to_return.solution=solution;
    //dist_t Exceed2=ExceedsCapacity(solution,demand,mydata);

    dist_t Exceed2{0.0};

    dist_t temp_exc1 = excess_clusters[info_cluster1[0]]-C_Data[customer_1-1].dem+\
                        +C_Data[customer_2-1].dem;
    dist_t temp_exc2 = excess_clusters[info_cluster2[0]]-C_Data[customer_2-1].dem+\
                        +C_Data[customer_1-1].dem;

    excess_clusters[info_cluster1[0]]=temp_exc1;
    excess_clusters[info_cluster2[0]]=temp_exc2;

    for(int i{0};i<excess_clusters.size();i++){

        if(excess_clusters[i]>0.0){

            Exceed2+=excess_clusters[i];
        }
    }

    data_to_return.result=result-Exceed1+(Exceed2*mydata.factor_demand_dist);
    data_to_return.excess=(Exceed2*mydata.factor_demand_dist);
    //data_to_return.result=result+Exceed2;
    data_to_return.demand=demand;
    data_to_return.excess_clusters=excess_clusters;
    //data_to_return.result=result;

    //cout<<ExceedsCapacity(data_to_return.solution,demand,mydata)<<" THIS IS THE EXCEED....."<<endl;

    return data_to_return;

}

/*****************************************************************************************************************************/

/*********************************Insertion-to-route Estimator****************************************************************/

dist_t InsertionRouteEstimator(int customer_o, int customer_d, vector<vector<int>> solution, vector<vector<dist_t>> Distancias,
                               list<vector<vector<int>>> demand, mdcvfp mydata, dist_t Exceed1, vector<dist_t> excess_clusters,
                               vector<nclients> C_Data){

        //dist_t Exceed1=ExceedsCapacity(solution,demand,mydata);


        vector<int> info_cluster1 = ClusterOfCustomer(solution,customer_o);
        vector<int> info_cluster2 = ClusterOfCustomer(solution,customer_d);

        dist_t result{0.0};
        int counter_if{0};

        if(info_cluster1[1]==0 && info_cluster2[1]!=0 && info_cluster2[1]!=(solution[info_cluster2[0]].size()-1)){
            //cout<<"Case 1"<<endl; //ok
            counter_if++;

            result = Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_o]+\
                    Distancias[customer_o][customer_d]-Distancias[0][customer_o]-\
                    Distancias[customer_o][solution[info_cluster1[0]][info_cluster1[1]+1]]+\
                    Distancias[0][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                    Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_d];

        }
        if(info_cluster1[1]!=0 && info_cluster2[1]==0 && info_cluster1[1]!=(solution[info_cluster1[0]].size()-1)){
            //cout<<"Case 2"<<endl;//ok
            counter_if++;

            result = Distancias[0][customer_o]+Distancias[customer_o][customer_d]-\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_o]-\
                    Distancias[customer_o][solution[info_cluster1[0]][info_cluster1[1]+1]]+\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                    Distancias[0][customer_d];

        }
        if(info_cluster1[1]==0 && info_cluster2[1]==0){
            //cout<<"Case 3"<<endl; //ok
            counter_if++;

            result = Distancias[customer_o][customer_d]-Distancias[0][customer_d]+\
                    Distancias[0][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                    Distancias[customer_o][solution[info_cluster1[0]][info_cluster1[1]+1]];

        }
        if(info_cluster1[1]==(solution[info_cluster1[0]].size()-1) && info_cluster2[1]!=0 &&
                 info_cluster2[1]!=(solution[info_cluster2[0]].size()-1)){
            //cout<<"Case 4"<<endl; // ok
            counter_if++;

            result = Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_o]+\
                    Distancias[customer_o][customer_d]-Distancias[customer_o][0]-\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_o]+\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][0]-\
                    Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_d];

        }
        if(info_cluster1[1]==(solution[info_cluster1[0]].size()-1) && info_cluster2[1]==0){
            //cout<<"Case 5"<<endl; //ok
            counter_if++;

            result = Distancias[customer_o][customer_d]-\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_o]-\
                    Distancias[0][customer_d]+\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][0];

        }
        if(info_cluster1[1]!=0 && info_cluster2[1]==(solution[info_cluster2[0]].size()-1) &&
                 info_cluster1[1]!=(solution[info_cluster1[0]].size()-1)){
            //cout<<"Case 6"<<endl; // ok
            counter_if++;

            result = Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_o]+\
                    Distancias[customer_o][customer_d]-Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_d]-\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_o]-\
                    Distancias[customer_o][solution[info_cluster1[0]][info_cluster1[1]+1]]+\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][solution[info_cluster1[0]][info_cluster1[1]+1]];

        }
        if(info_cluster1[1]==0 && info_cluster2[1]==(solution[info_cluster2[0]].size()-1)){
            //cout<<"Case 7"<<endl; // ok
            counter_if++;

            result = Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_o]+\
                    Distancias[customer_o][customer_d]+Distancias[0][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                    Distancias[0][customer_o]-Distancias[customer_o][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                    Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_d];

        }
        if(info_cluster1[1]==(solution[info_cluster1[0]].size()-1) &&
                 info_cluster2[1]==(solution[info_cluster2[0]].size()-1)){
            //cout<<"Case 8"<<endl; //ok
            counter_if++;

            result = Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_o]+\
                    Distancias[customer_o][customer_d]+Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][0]-\
                    Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_d]-\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_o]-\
                    Distancias[customer_o][0];
        }
        if(info_cluster1[1]!=0 && info_cluster1[1]!=(solution[info_cluster1[0]].size()-1) &&
            info_cluster2[1]!=0 && info_cluster2[1]!=(solution[info_cluster2[0]].size()-1)){
            //cout<<"Case 9"<<endl; // ok
            counter_if++;

            result = Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_o]+\
                    Distancias[customer_o][customer_d]+\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_o]-\
                    Distancias[customer_o][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                    Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_d];

        }

        if(counter_if==0){
            cout<<"DIDN'T ENTER TO IF!!!<<<<<<<<<"<<endl;
            cout<<"Size of cluster o "<<solution[info_cluster1[0]].size()-1<<endl;
            cout<<"Size of cluster d "<<solution[info_cluster2[0]].size()-1<<endl;
            cout<<info_cluster1[1]<<" and "<<info_cluster2[1]<<endl;
        }

        //PrintMatrix(solution,"[","]");
        int temp{0};
        temp = solution[info_cluster1[0]][info_cluster1[1]];
        //cout<<"temp is: "<<temp<<endl;
        if(info_cluster1[1]!=0){
            solution[info_cluster1[0]].erase(solution[info_cluster1[0]].begin()+\
                                         info_cluster1[1]);
        }else{
            solution[info_cluster1[0]].erase(solution[info_cluster1[0]].begin());
        }

        //PrintMatrix(solution,"[","]");
        if(info_cluster2[1]==0){
            //cout<<"temp is: "<<temp<<endl;
            solution[info_cluster2[0]].insert(solution[info_cluster2[0]].begin(),temp);
        }else{
            //cout<<"temp is: "<<temp<<endl;
            solution[info_cluster2[0]].insert(solution[info_cluster2[0]].begin()+\
                                          info_cluster2[1],temp);
        }
        //PrintMatrix(solution,"[","]");


        //cout<<"Customers changed!!"<<endl;

        list<vector<vector<int>>>::iterator it1 = demand.begin();
        if(info_cluster1[0]!=0){
            advance(it1,info_cluster1[0]);
        }
        vector<vector<int>> vector_it1 = (*it1);
        //PrintMatrix(vector_it1,"[","]");
        vector<int> temp_it1 = vector_it1[info_cluster1[1]];
        if(info_cluster1[1]!=0){
            vector_it1.erase(vector_it1.begin()+info_cluster1[1]);
        }else{
            vector_it1.erase(vector_it1.begin());
        }
        //PrintMatrix(vector_it1,"[","]");
        (*it1)=vector_it1;

        //cout<<"Before packing completed!!"<<endl;

        list<vector<vector<int>>>::iterator it2 = demand.begin();
        if(info_cluster2[0]!=0){
          advance(it2,info_cluster2[0]);
        }
        vector<vector<int>> vector_it2 = (*it2);
        //PrintMatrix(vector_it2,"[","]");
        if(info_cluster2[1]!=0){
            vector_it2.insert(vector_it2.begin()+info_cluster2[1],temp_it1);
        }else{
            vector_it2.insert(vector_it2.begin(),temp_it1);
        }
        //PrintMatrix(vector_it2,"[","]");
        (*it2)=vector_it2;

        //dist_t Exceed2=ExceedsCapacity(solution,demand,mydata);

        dist_t Exceed2{0.0};

        dist_t temp_exc1 = excess_clusters[info_cluster1[0]]-C_Data[customer_o-1].dem;
        dist_t temp_exc2 = excess_clusters[info_cluster2[0]]+C_Data[customer_o-1].dem;

        excess_clusters[info_cluster1[0]]=temp_exc1;
        excess_clusters[info_cluster2[0]]=temp_exc2;

        for(int i{0};i<excess_clusters.size();i++){

            if(excess_clusters[i]>0.0){

                Exceed2+=excess_clusters[i];
            }
        }

        result=result-Exceed1+(Exceed2*mydata.factor_demand_dist);
        //result=result+Exceed2;

        //cout<<ExceedsCapacity(solution,demand,mydata)<<" THIS IS THE EXCEED....."<<endl;

        return result;


}

/******************************************************************************************************************************/

/*********************************Insertion-to-route Operator******************************************************************/

struct ResultInsertionRoute{
    vector<vector<int>> solution;
    list<vector<vector<int>>> demand;
    dist_t result;
    dist_t excess;
    vector<dist_t> excess_clusters;
};

ResultInsertionRoute InsertionRouteOpt(int customer_o, int customer_d, vector<vector<int>> solution, vector<vector<dist_t>> Distancias,
                               list<vector<vector<int>>> demand, mdcvfp mydata, dist_t Exceed1, vector<dist_t> excess_clusters,
                               vector<nclients> C_Data){

        //dist_t Exceed1=ExceedsCapacity(solution,demand,mydata);

        ResultInsertionRoute data_to_return;

        data_to_return.solution=vector<vector<int>>();
        data_to_return.result=0.0;
        data_to_return.demand=list<vector<vector<int>>>();
        data_to_return.excess=0.0;
        data_to_return.excess_clusters=vector<dist_t>();

        vector<int> info_cluster1 = ClusterOfCustomer(solution,customer_o);
        vector<int> info_cluster2 = ClusterOfCustomer(solution,customer_d);

        dist_t result{0.0};

        if(info_cluster1[1]==0 && info_cluster2[1]!=0 && info_cluster2[1]!=(solution[info_cluster2[0]].size()-1)){
            //cout<<"Case 1"<<endl; //ok

            result = Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_o]+\
                    Distancias[customer_o][customer_d]-Distancias[0][customer_o]-\
                    Distancias[customer_o][solution[info_cluster1[0]][info_cluster1[1]+1]]+\
                    Distancias[0][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                    Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_d];

        }
        if(info_cluster1[1]!=0 && info_cluster2[1]==0 && info_cluster1[1]!=(solution[info_cluster1[0]].size()-1)){
            //cout<<"Case 2"<<endl;//ok

            result = Distancias[0][customer_o]+Distancias[customer_o][customer_d]-\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_o]-\
                    Distancias[customer_o][solution[info_cluster1[0]][info_cluster1[1]+1]]+\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                    Distancias[0][customer_d];

        }
        if(info_cluster1[1]==0 && info_cluster2[1]==0){
            //cout<<"Case 3"<<endl; //ok

            result = Distancias[customer_o][customer_d]-Distancias[0][customer_d]+\
                    Distancias[0][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                    Distancias[customer_o][solution[info_cluster1[0]][info_cluster1[1]+1]];

        }
        if(info_cluster1[1]==(solution[info_cluster1[0]].size()-1) && info_cluster2[1]!=0 &&
                 info_cluster2[1]!=(solution[info_cluster2[0]].size()-1)){
            //cout<<"Case 4"<<endl; // ok

            result = Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_o]+\
                    Distancias[customer_o][customer_d]-Distancias[customer_o][0]-\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_o]+\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][0]-\
                    Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_d];

        }
        if(info_cluster1[1]==(solution[info_cluster1[0]].size()-1) && info_cluster2[1]==0){
            //cout<<"Case 5"<<endl; //ok

            result = Distancias[customer_o][customer_d]-\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_o]-\
                    Distancias[0][customer_d]+\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][0];

        }
        if(info_cluster1[1]!=0 && info_cluster2[1]==(solution[info_cluster2[0]].size()-1) &&
                 info_cluster1[1]!=(solution[info_cluster1[0]].size()-1)){
            //cout<<"Case 6"<<endl; // ok

            result = Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_o]+\
                    Distancias[customer_o][customer_d]-Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_d]-\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_o]-\
                    Distancias[customer_o][solution[info_cluster1[0]][info_cluster1[1]+1]]+\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][solution[info_cluster1[0]][info_cluster1[1]+1]];

        }
        if(info_cluster1[1]==0 && info_cluster2[1]==(solution[info_cluster2[0]].size()-1)){
            //cout<<"Case 7"<<endl; // ok

            result = Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_o]+\
                    Distancias[customer_o][customer_d]+Distancias[0][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                    Distancias[0][customer_o]-Distancias[customer_o][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                    Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_d];

        }
        if(info_cluster1[1]==(solution[info_cluster1[0]]).size()-1 &&
                 info_cluster2[2]==(solution[info_cluster2[0]]).size()-1){
            //cout<<"Case 8"<<endl; //ok

            result = Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_o]+\
                    Distancias[customer_o][customer_d]+Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][0]-\
                    Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_d]-\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_o]-\
                    Distancias[customer_o][0];
        }
        if(info_cluster1[1]!=0 && info_cluster1[1]!=(solution[info_cluster1[0]].size()-1) &&
            info_cluster2[1]!=0 && info_cluster2[1]!=(solution[info_cluster2[0]].size()-1)){
            //cout<<"Case 9"<<endl; // ok

            result = Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_o]+\
                    Distancias[customer_o][customer_d]+\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                    Distancias[solution[info_cluster1[0]][info_cluster1[1]-1]][customer_o]-\
                    Distancias[customer_o][solution[info_cluster1[0]][info_cluster1[1]+1]]-\
                    Distancias[solution[info_cluster2[0]][info_cluster2[1]-1]][customer_d];

        }

        //cout<<"RESULT CALCULATED!!!"<<endl;

        //PrintMatrix(solution,"[","]");
        int temp{0};
        temp = solution[info_cluster1[0]][info_cluster1[1]];
        //cout<<"temp is: "<<temp<<endl;
        if(info_cluster1[1]!=0){
            solution[info_cluster1[0]].erase(solution[info_cluster1[0]].begin()+\
                                         info_cluster1[1]);
        }else{
            solution[info_cluster1[0]].erase(solution[info_cluster1[0]].begin());
        }
        //PrintMatrix(solution,"[","]");
        if(info_cluster2[1]==0){
            //cout<<"temp is: "<<temp<<endl;
            solution[info_cluster2[0]].insert(solution[info_cluster2[0]].begin(),temp);
        }else{
            //cout<<"temp is: "<<temp<<endl;
            solution[info_cluster2[0]].insert(solution[info_cluster2[0]].begin()+\
                                          info_cluster2[1],temp);
        }
        //PrintMatrix(solution,"[","]");


        //cout<<"Customers changed!!"<<endl;

        list<vector<vector<int>>>::iterator it1 = demand.begin();
        if(info_cluster1[0]!=0){
            advance(it1,info_cluster1[0]);
        }
        vector<vector<int>> vector_it1 = (*it1);
        //PrintMatrix(vector_it1,"[","]");
        vector<int> temp_it1 = vector_it1[info_cluster1[1]];
        if(info_cluster1[1]!=0){
            vector_it1.erase(vector_it1.begin()+info_cluster1[1]);
        }else{
            vector_it1.erase(vector_it1.begin());
        }
        //PrintMatrix(vector_it1,"[","]");
        (*it1)=vector_it1;

        //cout<<"Before packing completed!!"<<endl;

        list<vector<vector<int>>>::iterator it2 = demand.begin();
        if(info_cluster2[0]!=0){
          advance(it2,info_cluster2[0]);
        }
        vector<vector<int>> vector_it2 = (*it2);
        //PrintMatrix(vector_it2,"[","]");
        if(info_cluster2[1]!=0){
            vector_it2.insert(vector_it2.begin()+info_cluster2[1],temp_it1);
        }else{
            vector_it2.insert(vector_it2.begin(),temp_it1);
        }
        //PrintMatrix(vector_it2,"[","]");
        (*it2)=vector_it2;


        //cout<<"Packing changed!!"<<endl;

        //dist_t Exceed2=ExceedsCapacity(solution,demand,mydata);

        dist_t Exceed2{0.0};

        dist_t temp_exc1 = excess_clusters[info_cluster1[0]]-C_Data[customer_o-1].dem;
        dist_t temp_exc2 = excess_clusters[info_cluster2[0]]+C_Data[customer_o-1].dem;

        excess_clusters[info_cluster1[0]]=temp_exc1;
        excess_clusters[info_cluster2[0]]=temp_exc2;

        for(int i{0};i<excess_clusters.size();i++){

            if(excess_clusters[i]>0.0){

                Exceed2+=excess_clusters[i];
            }
        }

        result=result-Exceed1+(Exceed2*mydata.factor_demand_dist);
        //result=result+Exceed2;
        //cout<<"Result calculated!!"<<endl;
        data_to_return.result=result;
        data_to_return.solution=solution;
        data_to_return.demand=demand;
        data_to_return.excess=(Exceed2*mydata.factor_demand_dist);
        data_to_return.excess_clusters=excess_clusters;

        //cout<<"End Insertion to route"<<endl;

        //cout<<ExceedsCapacity(solution,demand,mydata)<<" THIS IS THE EXCEED....."<<endl;

        return data_to_return;

}



/******************************************************************************************************************************/

/*************************************Two-Opt Estimator************************************************************/

dist_t TwoOptEstimator(int customer_1, int customer_2, vector<vector<int>> solution, vector<vector<dist_t>> Distancias,
                       mdcvfp mydata){

    vector<int> info_pos1 = ClusterOfCustomer(solution,customer_1);
    vector<int> info_pos2 = ClusterOfCustomer(solution,customer_2);
    int cluster = info_pos1[0];

    //cout<<"INFO CLUSTERS CALCULATED"<<endl;

    dist_t result{0.0};

   if(info_pos1[1]==0 && info_pos2[1]==solution[cluster].size()-1){
        //cout<<"Case 1"<<endl;

        result = Distancias[customer_2][solution[cluster][info_pos1[1]+1]]+\
                Distancias[solution[cluster][info_pos2[1]-1]][customer_1]-\
                Distancias[customer_1][solution[cluster][info_pos1[1]+1]]-\
                Distancias[solution[cluster][info_pos2[1]-1]][customer_2];

    }

    if(info_pos1[1]==solution[cluster].size()-1 && info_pos2[1]==0){
        //cout<<"Case 2"<<endl;

        result = Distancias[solution[cluster][info_pos1[1]-1]][customer_2]+\
                Distancias[customer_1][solution[cluster][info_pos2[1]+1]]-\
                Distancias[solution[cluster][info_pos1[1]-1]][customer_1]-\
                Distancias[customer_2][solution[cluster][info_pos2[1]+1]];


    }

    if(info_pos1[1]==0 && info_pos2[1]!=solution[cluster].size()-1){
        //cout<<"Case 3"<<endl;

        if(info_pos2[1]==info_pos1[1]+1){

            result = Distancias[0][customer_2]+Distancias[customer_1][solution[cluster][info_pos2[1]+1]]-\
                Distancias[0][customer_1]-Distancias[customer_2][solution[cluster][info_pos2[1]+1]];

        }else{

            result = Distancias[0][customer_2]+\
                Distancias[customer_1][solution[cluster][info_pos2[1]+1]]-Distancias[0][customer_1]-\
                Distancias[customer_2][solution[cluster][info_pos2[1]+1]]+\
                Distancias[solution[cluster][info_pos2[1]-1]][customer_1]+\
                Distancias[customer_2][solution[cluster][info_pos1[1]+1]]-\
                Distancias[customer_1][solution[cluster][info_pos1[1]+1]]-\
                Distancias[solution[cluster][info_pos2[1]-1]][customer_2];

        }

    }

    if(info_pos1[1]!=solution[cluster].size()-1 && info_pos2[1]==0){
        //cout<<"Case 4"<<endl;

        if(info_pos1[1]==info_pos2[1]+1){

            result = Distancias[0][customer_1]+Distancias[customer_2][solution[cluster][info_pos1[1]+1]]-\
                Distancias[0][customer_2]-Distancias[customer_1][solution[cluster][info_pos1[1]+1]];
        }else{

            result = Distancias[customer_2][solution[cluster][info_pos1[1]+1]]+Distancias[0][customer_1]-\
                Distancias[customer_1][solution[cluster][info_pos1[1]+1]]-Distancias[0][customer_2]+\
                Distancias[solution[cluster][info_pos1[1]-1]][customer_2]+\
                Distancias[solution[cluster][info_pos2[1]+1]][customer_1]-\
                Distancias[customer_2][solution[cluster][info_pos2[1]+1]]-\
                Distancias[solution[cluster][info_pos1[1]-1]][customer_1];
        }

    }

    if(info_pos1[1]==solution[cluster].size()-1 && info_pos2[1]!=solution[cluster].size()-1){

        if(info_pos1[1]==info_pos2[1]+1){

            result = Distancias[solution[cluster][info_pos2[1]-1]][customer_1]+Distancias[0][customer_2]-\
                Distancias[0][customer_1]-Distancias[solution[cluster][info_pos2[1]-1]][customer_2];
        }else{

            result = Distancias[solution[cluster][info_pos2[1]-1]][customer_1]+\
                Distancias[solution[cluster][info_pos2[1]+1]][customer_1]+\
                Distancias[customer_2][solution[cluster][info_pos1[1]-1]]+Distancias[0][customer_2]-\
                Distancias[0][customer_1]-Distancias[solution[cluster][info_pos1[1]-1]][customer_1]-\
                Distancias[solution[cluster][info_pos2[1]-1]][customer_2]-\
                Distancias[customer_2][solution[cluster][info_pos2[1]+1]];
        }
    }

    if(info_pos2[1]==solution[cluster].size()-1 && info_pos1[1]!=solution[cluster].size()-1){

        if(info_pos2[1]==info_pos1[1]+1){

            result = Distancias[solution[cluster][info_pos1[1]-1]][customer_2]+Distancias[0][customer_1]-\
                Distancias[solution[cluster][info_pos1[1]-1]][customer_1]-Distancias[0][customer_2];
        }else{

            result = Distancias[solution[cluster][info_pos1[1]-1]][customer_2]+\
                Distancias[solution[cluster][info_pos1[1]+1]][customer_2]+\
                Distancias[customer_1][solution[cluster][info_pos2[1]-1]]+Distancias[0][customer_1]-\
                Distancias[solution[cluster][info_pos1[1]-1]][customer_1]-\
                Distancias[customer_1][solution[cluster][info_pos1[1]+1]]-Distancias[0][customer_2]-\
                Distancias[solution[cluster][info_pos2[1]-1]][customer_2];
        }
    }

    if(info_pos1[1]!=0 && info_pos1[1]!=solution[cluster].size()-1 && info_pos2[1]!=0 && info_pos2[1]!=solution[cluster].size()-1){
        //cout<<"Case 5"<<endl;

        if(info_pos1[1]==info_pos2[1]+1){

            result = Distancias[customer_2][solution[cluster][info_pos1[1]+1]]+\
                    Distancias[solution[cluster][info_pos2[1]-1]][customer_1]-\
                    Distancias[solution[cluster][info_pos2[1]-1]][customer_2]-\
                    Distancias[customer_1][solution[cluster][info_pos1[1]+1]];
        }
        if(info_pos2[1]==info_pos1[1]+1){

            result = Distancias[customer_1][solution[cluster][info_pos2[1]+1]]+\
                    Distancias[solution[cluster][info_pos1[1]-1]][customer_2]-\
                    Distancias[solution[cluster][info_pos1[1]-1]][customer_1]-\
                    Distancias[customer_2][solution[cluster][info_pos2[1]+1]];
        }
        if(info_pos1[1]!=info_pos2[1]+1 && info_pos2[1]!=info_pos1[1]+1){

            if(info_pos2[1]>info_pos1[1]){

                result = Distancias[solution[cluster][info_pos1[1]-1]][customer_2]+\
                    Distancias[customer_1][solution[cluster][info_pos2[1]+1]]-\
                    Distancias[solution[cluster][info_pos1[1]-1]][customer_1]-\
                    Distancias[customer_2][solution[cluster][info_pos2[1]+1]]+\
                    Distancias[customer_1][solution[cluster][info_pos2[1]-1]]+\
                    Distancias[customer_2][solution[cluster][info_pos1[1]+1]]-\
                    Distancias[customer_1][solution[cluster][info_pos1[1]+1]]-\
                    Distancias[solution[cluster][info_pos2[1]-1]][customer_2];

            }else{

                result = Distancias[solution[cluster][info_pos1[1]-1]][customer_2]+\
                    Distancias[customer_2][solution[cluster][info_pos1[1]+1]]+\
                    Distancias[customer_1][solution[cluster][info_pos2[1]-1]]+\
                    Distancias[solution[cluster][info_pos2[1]+1]][customer_1]-\
                    Distancias[solution[cluster][info_pos2[1]-1]][customer_2]-\
                    Distancias[customer_2][solution[cluster][info_pos2[1]+1]]-\
                    Distancias[solution[cluster][info_pos1[1]-1]][customer_1]-\
                    Distancias[customer_1][solution[cluster][info_pos1[1]+1]];

            }

        }
    }


    //cout<<"RESULT CALCULATED!"<<endl;


    return result;

}

/*****************************************************************************************************************************/

/*************************************Two-Opt Operator************************************************************/

struct ResultTwoOpt{
    vector<vector<int>> solution;
    dist_t result;
};

ResultTwoOpt TwoOptOperator(int customer_1, int customer_2, vector<vector<int>> solution, vector<vector<dist_t>> Distancias,
                       mdcvfp mydata){

    ResultTwoOpt data_to_return;

    data_to_return.result=0.0;
    data_to_return.solution=vector<vector<int>>();

    vector<int> info_pos1 = ClusterOfCustomer(solution,customer_1);
    vector<int> info_pos2 = ClusterOfCustomer(solution,customer_2);
    int cluster = info_pos1[0];

    //cout<<"INFO CLUSTERS CALCULATED"<<endl;

    dist_t result{0.0};

    if(info_pos1[1]==0 && info_pos2[1]==solution[cluster].size()-1){
        //cout<<"Case 1"<<endl;

        result = Distancias[customer_2][solution[cluster][info_pos1[1]+1]]+\
                Distancias[solution[cluster][info_pos2[1]-1]][customer_1]-\
                Distancias[customer_1][solution[cluster][info_pos1[1]+1]]-\
                Distancias[solution[cluster][info_pos2[1]-1]][customer_2];

    }
    if(info_pos1[1]==solution[cluster].size()-1 && info_pos2[1]==0){
        //cout<<"Case 2"<<endl;

        result = Distancias[solution[cluster][info_pos1[1]-1]][customer_2]+\
                Distancias[customer_1][solution[cluster][info_pos2[1]+1]]-\
                Distancias[solution[cluster][info_pos1[1]-1]][customer_1]-\
                Distancias[customer_2][solution[cluster][info_pos2[1]+1]];


    }
    if(info_pos1[1]==0 && info_pos2[1]!=solution[cluster].size()-1){
        //cout<<"Case 3"<<endl;

        if(info_pos2[1]==info_pos1[1]+1){

            result = Distancias[0][customer_2]+Distancias[customer_1][solution[cluster][info_pos2[1]+1]]-\
                Distancias[0][customer_1]-Distancias[customer_2][solution[cluster][info_pos2[1]+1]];

        }else{

            result = Distancias[0][customer_2]+\
                Distancias[customer_1][solution[cluster][info_pos2[1]+1]]-Distancias[0][customer_1]-\
                Distancias[customer_2][solution[cluster][info_pos2[1]+1]]+\
                Distancias[solution[cluster][info_pos2[1]-1]][customer_1]+\
                Distancias[customer_2][solution[cluster][info_pos1[1]+1]]-\
                Distancias[customer_1][solution[cluster][info_pos1[1]+1]]-\
                Distancias[solution[cluster][info_pos2[1]-1]][customer_2];

        }

    }
    if(info_pos1[1]!=solution[cluster].size()-1 && info_pos2[1]==0){
        //cout<<"Case 4"<<endl;

        if(info_pos1[1]==info_pos2[1]+1){

            result = Distancias[0][customer_1]+Distancias[customer_2][solution[cluster][info_pos1[1]+1]]-\
                Distancias[0][customer_2]-Distancias[customer_1][solution[cluster][info_pos1[1]+1]];
        }else{

            result = Distancias[customer_2][solution[cluster][info_pos1[1]+1]]+Distancias[0][customer_1]-\
                Distancias[customer_1][solution[cluster][info_pos1[1]+1]]-Distancias[0][customer_2]+\
                Distancias[solution[cluster][info_pos1[1]-1]][customer_2]+\
                Distancias[solution[cluster][info_pos2[1]+1]][customer_1]-\
                Distancias[customer_2][solution[cluster][info_pos2[1]+1]]-\
                Distancias[solution[cluster][info_pos1[1]-1]][customer_1];
        }

    }
    if(info_pos1[1]==solution[cluster].size()-1 && info_pos2[1]!=solution[cluster].size()-1){

        if(info_pos1[1]==info_pos2[1]+1){

            result = Distancias[solution[cluster][info_pos2[1]-1]][customer_1]+Distancias[0][customer_2]-\
                Distancias[0][customer_1]-Distancias[solution[cluster][info_pos2[1]-1]][customer_2];
        }else{

            result = Distancias[solution[cluster][info_pos2[1]-1]][customer_1]+\
                Distancias[solution[cluster][info_pos2[1]+1]][customer_1]+\
                Distancias[customer_2][solution[cluster][info_pos1[1]-1]]+Distancias[0][customer_2]-\
                Distancias[0][customer_1]-Distancias[solution[cluster][info_pos1[1]-1]][customer_1]-\
                Distancias[solution[cluster][info_pos2[1]-1]][customer_2]-\
                Distancias[customer_2][solution[cluster][info_pos2[1]+1]];
        }
    }
    if(info_pos2[1]==solution[cluster].size()-1 && info_pos1[1]!=solution[cluster].size()-1){

        if(info_pos2[1]==info_pos1[1]+1){

            result = Distancias[solution[cluster][info_pos1[1]-1]][customer_2]+Distancias[0][customer_1]-\
                Distancias[solution[cluster][info_pos1[1]-1]][customer_1]-Distancias[0][customer_2];
        }else{

            result = Distancias[solution[cluster][info_pos1[1]-1]][customer_2]+\
                Distancias[solution[cluster][info_pos1[1]+1]][customer_2]+\
                Distancias[customer_1][solution[cluster][info_pos2[1]-1]]+Distancias[0][customer_1]-\
                Distancias[solution[cluster][info_pos1[1]-1]][customer_1]-\
                Distancias[customer_1][solution[cluster][info_pos1[1]+1]]-Distancias[0][customer_2]-\
                Distancias[solution[cluster][info_pos2[1]-1]][customer_2];
        }
    }
    if(info_pos1[1]!=0 && info_pos1[1]!=solution[cluster].size()-1 && info_pos2[1]!=0 && info_pos2[1]!=solution[cluster].size()-1){
        //cout<<"Case 5"<<endl;

        if(info_pos1[1]==info_pos2[1]+1){

            result = Distancias[customer_2][solution[cluster][info_pos1[1]+1]]+\
                    Distancias[solution[cluster][info_pos2[1]-1]][customer_1]-\
                    Distancias[solution[cluster][info_pos2[1]-1]][customer_2]-\
                    Distancias[customer_1][solution[cluster][info_pos1[1]+1]];
        }
        if(info_pos2[1]==info_pos1[1]+1){

            result = Distancias[customer_1][solution[cluster][info_pos2[1]+1]]+\
                    Distancias[solution[cluster][info_pos1[1]-1]][customer_2]-\
                    Distancias[solution[cluster][info_pos1[1]-1]][customer_1]-\
                    Distancias[customer_2][solution[cluster][info_pos2[1]+1]];
        }
        if(info_pos1[1]!=info_pos2[1]+1 && info_pos2[1]!=info_pos1[1]+1){

            if(info_pos2[1]>info_pos1[1]){

                result = Distancias[solution[cluster][info_pos1[1]-1]][customer_2]+\
                    Distancias[customer_1][solution[cluster][info_pos2[1]+1]]-\
                    Distancias[solution[cluster][info_pos1[1]-1]][customer_1]-\
                    Distancias[customer_2][solution[cluster][info_pos2[1]+1]]+\
                    Distancias[customer_1][solution[cluster][info_pos2[1]-1]]+\
                    Distancias[customer_2][solution[cluster][info_pos1[1]+1]]-\
                    Distancias[customer_1][solution[cluster][info_pos1[1]+1]]-\
                    Distancias[solution[cluster][info_pos2[1]-1]][customer_2];

            }else{

                result = Distancias[solution[cluster][info_pos1[1]-1]][customer_2]+\
                    Distancias[customer_2][solution[cluster][info_pos1[1]+1]]+\
                    Distancias[customer_1][solution[cluster][info_pos2[1]-1]]+\
                    Distancias[solution[cluster][info_pos2[1]+1]][customer_1]-\
                    Distancias[solution[cluster][info_pos2[1]-1]][customer_2]-\
                    Distancias[customer_2][solution[cluster][info_pos2[1]+1]]-\
                    Distancias[solution[cluster][info_pos1[1]-1]][customer_1]-\
                    Distancias[customer_1][solution[cluster][info_pos1[1]+1]];

            }

        }
    }



    //cout<<"RESULT CALCULATED!"<<endl;

    int temp_customer = solution[cluster][info_pos1[1]];
    solution[cluster][info_pos1[1]]=customer_2;
    solution[cluster][info_pos2[1]]=temp_customer;
    data_to_return.solution=solution;
    data_to_return.result=result;

    return data_to_return;

}


/*****************************************************************************************************************************/


/*************************************One-Route Operator*********************************************************************/

struct OneRouteReult{

    dist_t result;
    vector<vector<int>> solution;
    list<vector<vector<int>>> demand;
    vector<dist_t> excess_clusters;
    dist_t excess;
};

OneRouteReult OneRouteOpt(int customer, vector<vector<dist_t>> Distancias, list<vector<vector<int>>> demand,
                         vector<vector<int>> solution, vector<dist_t> excess_clusters,
                         dist_t Exceed1, vector<nclients> C_Data, mdcvfp mydata){

        OneRouteReult data_to_return;

        data_to_return.solution=vector<vector<int>>();
        data_to_return.demand=list<vector<vector<int>>>();
        data_to_return.result=0.0;
        data_to_return.excess_clusters=vector<dist_t>();
        data_to_return.excess=0.0;

        vector<int> info_node = ClusterOfCustomer(solution,customer);

        dist_t result{0.0};

        if(info_node[1]==0){

            result = Distancias[0][customer]+Distancias[0][solution[info_node[0]][info_node[1]+1]]-\
                Distancias[customer][solution[info_node[0]][info_node[1]+1]];

        }else if(info_node[1]!=0 && info_node[1]!=(solution[info_node[0]].size()-1)){

            result = 2*Distancias[0][customer]+\
                Distancias[solution[info_node[0]][info_node[1]-1]][solution[info_node[0]][info_node[1]+1]]-\
                Distancias[solution[info_node[0]][info_node[1]-1]][customer]-\
                Distancias[customer][solution[info_node[0]][info_node[1]+1]];

        }else{

            result = Distancias[customer][0]+Distancias[solution[info_node[0]][info_node[1]-1]][0]-\
                Distancias[solution[info_node[0]][info_node[1]-1]][customer];

        }

        int temp{0};
        temp = solution[info_node[0]][info_node[1]];

        if(info_node[1]!=0){
            solution[info_node[0]].erase(solution[info_node[0]].begin()+\
                                         info_node[1]);
        }else{
            solution[info_node[0]].erase(solution[info_node[0]].begin());
        }

        vector<int> vector_temp={temp};
        solution.push_back(vector_temp);


        list<vector<vector<int>>>::iterator it1 = demand.begin();
        if(info_node[0]!=0){
            advance(it1,info_node[0]);
        }
        vector<vector<int>> vector_it1 = (*it1);

        vector<int> temp_it1 = vector_it1[info_node[1]];
        if(info_node[1]!=0){
            vector_it1.erase(vector_it1.begin()+info_node[1]);
        }else{
            vector_it1.erase(vector_it1.begin());
        }

        (*it1)=vector_it1;


        vector<vector<int>> vector_temp2=vector<vector<int>>();
        vector<int> vector_temp3={1,1,1};
        vector_temp2.push_back(vector_temp3);
        demand.push_back(vector_temp2);
        excess_clusters.push_back(C_Data[customer-1].dem-mydata.volumeVehicle);


        dist_t Exceed2{0.0};

        dist_t temp_exc1 = excess_clusters[info_node[0]]-C_Data[customer-1].dem;

        excess_clusters[info_node[0]]=temp_exc1;

        for(int i{0};i<excess_clusters.size();i++){

            if(excess_clusters[i]>0.0){

                Exceed2+=excess_clusters[i];
            }
        }

        result=result-Exceed1+(Exceed2*mydata.factor_demand_dist);

        data_to_return.solution=solution;
        data_to_return.result=result;
        data_to_return.excess=(Exceed2*mydata.factor_demand_dist);
        data_to_return.demand=demand;
        data_to_return.excess_clusters=excess_clusters;

        return data_to_return;


}

/*****************************************************************************************************************************/

int main(int argc, char**argv) {

    srand (time(0));

    /********************How to run the code**************************************************/
    /*
        1. run make
        2. ./sVRP {folder where the input is} {input} {inputinfo}

    example: ./sVRP hola testInput 3l_cvrp01.txt
    */
    /*****************************************************************************************/

	//Especificar la ruta donde se encuentran las instancias que se van a correr
	char directorio[200] = "instanciasCVRP-CLP/";
	//char directorio2[300] = "/home/akavallo/Dropbox/CPLEX2018/IntanciasCPLEX/";
	char directorio2[300] = "./IntanciasCPLEX/";

	string archivo2 = argv[3];
	char archivo[100];
	copy(archivo2.begin(), (archivo2.size() >= 100 ? archivo2.begin() + 100 : archivo2.end()), archivo);

	cout << archivo << endl;
	cout  << archivo2 << endl;
	cout << directorio<< endl;


	//Cargar el listado de archivos (instancias) presentes en el directorio especificado

	cargarInstanciaCVRP_CLP(strcat(directorio , archivo), casoEstudio);

	//Salida de prueba de la estructura que recibe la instancia
	cout<<endl<<"Nombre del archivo: "<< strcat(directorio, archivo) <<endl;
	mostrarInstanciaCVRP_CLP(casoEstudio);
	cout << " " << endl;


#ifdef MDVRP
    string testname = argc > 1 ? argv[1] : "./test/mdvrp/data-01";
#else
    string testname = argc > 1 ? argv[1] : "../test/cflrp/instancia_3_7";
//    string testname = argc > 1 ? argv[1] : "instancia_2_1";
#endif
    //cout<<r_distance2(1.79242,1.83741,-77.1642,-76.9663)<<"**************"<<endl;
	string filename = argv[2];


    //try {

        vector <string> my_colors;
        my_colors.push_back("#FF0000");
        my_colors.push_back("#00FF00");
        my_colors.push_back("#0000FF");
        my_colors.push_back("#FFFF00");
        my_colors.push_back("#00FFFF");
        my_colors.push_back("#FF00FF");
        my_colors.push_back("#5E5E5E");
        my_colors.push_back("#000080");
        my_colors.push_back("#000000");
        my_colors.push_back("#008000");
        my_colors.push_back("#008080");
        my_colors.push_back("#800080");
        my_colors.push_back("#800000");
        my_colors.push_back("#643200");
        my_colors.push_back("#C86400");
        my_colors.push_back("#964B00");

#ifdef MDVRP
        mdcvfp mydata; // Create structure
        mydata.penalty_reloc = 2.0; // Fixed cost to relocate a customer
        int mydata_tmp; // Temporal string to store the values which are not necessary

        ifstream data_input((testname + "/" + filename + ".txt").c_str());

        // Check if there is any problem with the file
        if (! data_input.is_open() ) {
            cout << "Can not open file " << (testname + "/" + filename + ".txt") << endl;
            return EXIT_FAILURE;
        }
        data_input >> mydata_tmp;
        if (mydata_tmp != 2) {
            cout << "Problem Type Different of 2 (MDVRP)" << endl;
            return EXIT_FAILURE;
        }
        data_input >> mydata.nvehicles;

        cout << "check nvehicles" << endl;
        data_input >> mydata.ncustomers; // Number of customers
        data_input >> mydata.ndepots; // Number of depots

        // Data per depot
        mydata.depots.resize(mydata.ndepots);
        for (int i=0; i<mydata.ndepots; ++i) {
            data_input >> mydata.depots[i].max_duration;
            data_input >> mydata.depots[i].max_load;
        }

        // Data for customers
        mydata.customers.resize(mydata.ncustomers+(mydata.ncustomers*3)+mydata.ndepots);
        for (int i=0; i<mydata.ncustomers; ++i) {
            data_input >> mydata.customers[i].id;
            data_input >> mydata.customers[i].x;
            data_input >> mydata.customers[i].y;
            data_input >> mydata.customers[i].service;
            data_input >> mydata.customers[i].demand;
            data_input >> mydata.customers[i].frequency;
        }

        for (int ii=0; ii<mydata.ndepots; ++ii) {
            int i=ii+mydata.ncustomers;//+(mydata.ncustomers*3);
            data_input >> mydata.customers[i].id;
            data_input >> mydata.customers[i].x;
            data_input >> mydata.customers[i].y;
            data_input >> mydata.customers[i].service;
            data_input >> mydata.customers[i].demand;
            data_input >> mydata.customers[i].frequency;
        }
        /**Reading of additional parameters**/
        // Example in folder hola --> testInput

        data_input>>mydata.numberTypeWaste; // Number of type of wastes
        cout<<"Types of waste: "<<mydata.numberTypeWaste<<endl;
        data_input>>mydata.heightVehicle; // Height of the vehicle
        data_input>>mydata.widthVehicle; // Width of the vehicle
        data_input>>mydata.lengthVehicle; // Length of the vehicle
        cout<<mydata.heightVehicle<<" "<<mydata.widthVehicle<<" "<<mydata.lengthVehicle<<endl;
        mydata.volumeVehicle=float(mydata.heightVehicle)*float(mydata.widthVehicle)*float(mydata.lengthVehicle);

        // Skip the parameters of the depots
        for(int i{0};i<mydata.ndepots;i++){
            data_input>>mydata_tmp;
            data_input>>mydata_tmp;
        }

        // Reading of the info of boxes
        int kk{0};
        //srand(time(NULL));
        for(int i{0};i<mydata.ncustomers;i++){
            //cout<<"Customer "<<i;
            data_input>>mydata_tmp;
            data_input>>mydata.customers[i].numberBoxes;
            //cout<<" has "<<mydata.customers[i].numberBoxes<<" boxes"<<endl;
            int type_of_waste{1};
            for(int j{0};j<mydata.customers[i].numberBoxes;j++){
                mydata.boxes.resize(mydata.boxes.size()+mydata.customers[i].numberBoxes);
                mydata.boxes[kk].customerId=i;
                mydata.boxes[kk].boxId=j;
                data_input>>mydata.boxes[kk].heightBox;
                data_input>>mydata.boxes[kk].widthBox;
                data_input>>mydata.boxes[kk].lengthBox;
                data_input>>mydata.boxes[kk].fragilityBox;
                //mydata.boxes[kk].wasteType=rand()%mydata.numberTypeWaste+1;
                mydata.boxes[kk].wasteType=type_of_waste;
                type_of_waste++;
                //cout<<"Box "<<mydata.boxes[kk].boxId<<". Waste "<<mydata.boxes[kk].wasteType<<endl;
                //cout<<"Height= "<<mydata.boxes[kk].heightBox<<", Width= "<<mydata.boxes[kk].widthBox<<", Length= "<<mydata.boxes[kk].lengthBox<<endl;
                kk+=1;
            }
        }

        // Calculation of the demandVolumen per customer
        for(int i{0};i<mydata.boxes.size();i++){
            mydata.customers[mydata.boxes[i].customerId].demandVolume+=mydata.boxes[i].heightBox*mydata.boxes[i].widthBox*mydata.boxes[i].lengthBox;
        }

        //data_input.close();
#endif

#ifdef ENTERA
        bool Round = 1;
		//cout << "entero" << endl;
#else
        bool Round = 0;
#endif
        /**Copy of the structure to variables**/
        int Ndepots; // Number of depots
        int Ncostumers; // Number of customers
        int Veh_Cap; // Vehicle capacity
        float Route_Cost; // Route cost
        double Partial_Variable = 0;
        double Partial_Open = 0;
        double Partial_Fix_Route = 0;
#ifdef MDVRP
        Ndepots = mydata.ndepots;
        Ncostumers = mydata.ncustomers;
        Veh_Cap=mydata.heightVehicle*mydata.lengthVehicle*mydata.widthVehicle;
        cout<<"Vehicle capacity: "<<Veh_Cap<<endl;
        Route_Cost = 0;
#else
        ifstream Parameter_information((testname + "/" +  "parameter.txt").c_str());
        Parameter_information >> Ndepots; // Numero de depositos
        Parameter_information >> Ncostumers; // Numero de clientes
        Parameter_information >> Veh_Cap; // Capacidad de vehiculos
        Parameter_information >> Route_Cost; // Costo de uso de ruta
        Parameter_information.close();
#endif
        // PARAMETROS GRANULAR
        float Beta; // Beta inicial de parametro granular
        float Newbeta; // Beta para ampliar lista candidatos
        float g; // Parametro de penalizacion
        float Inc_pen; // Incremento de la penalidad
        float Red_pen; // Reduccion de la penalidad
        float Valor_nmovbeta; // Numero veces de Ncostumers para cambio de beta
        float Valor_nmoviter; // Numero veces de Ncostumers para devolverse a beta anterior
        float Valor_perturb; // Numero de veces Perturbacion
        float Valor_div; // Numero de veces Diversificacion
        float Valor_proute; // Porcentaje sobre solucion inicial para ruta
        float Valor_pdepot; // Porcentaje sobre solucion inicial para depot
        float Valor_pmax; // Porcentaje sobre solucion incial para maximo cambio de penalidad
        int Nmovpen; // Numero de iteraciones factibles
        int Repair; // Numero de veces que se efectua split

#ifdef VALUESHARDCODED
        Beta = 1.25f;
        Newbeta = 2.20f;
        g = 0.01;
        Inc_pen = 2.0f;
        Red_pen = 0.5f;
        Valor_nmovbeta = 2;
        Valor_nmoviter = 1;
        Valor_perturb = 1;
        Valor_div = 3;
        Valor_proute = 0.01;
        Valor_pdepot = 0.01;
        Valor_pmax = 0.05;
        Nmovpen = 10;
        Repair = 3;
#else
#ifdef MDVRP
        ifstream values_information("../test/mdvrp/values_file.txt");
#else
        ifstream values_information("../test/cflrp/values_file.txt");
//        ifstream values_information(testname + "/" + "values_file.txt");
#endif
        string text;
        values_information >> text >> Beta;
        if(text != "Beta") {
            cout << "error file" << endl;
            return EXIT_FAILURE;
        }
        values_information >> text >> Newbeta;
        if(text != "Newbeta") {
            cout << "error file" << endl;
            return EXIT_FAILURE;
        }
        values_information >> text >> g;
        if(text != "g") {
            cout << "error file" << endl;
            return EXIT_FAILURE;
        }
        values_information >> text >> Inc_pen;
        if(text != "Inc_pen") {
            cout << "error file" << endl;
            return EXIT_FAILURE;
        }
        values_information >> text >> Red_pen;
        if(text != "Red_pen") {
            cout << "error file" << endl;
            return EXIT_FAILURE;
        }
        values_information >> text >> Valor_nmovbeta;
        if(text != "Valor_nmovbeta") {
            cout << "error file" << endl;
            return EXIT_FAILURE;
        }
        values_information >> text >> Valor_nmoviter;
        if(text != "Valor_nmoviter") {
            cout << "error file" << endl;
            return EXIT_FAILURE;
        }
        values_information >> text >> Valor_perturb;
        if(text != "Valor_perturb") {
            cout << "error file" << endl;
            return EXIT_FAILURE;
        }
        values_information >> text >> Valor_div;
        if(text != "Valor_div") {
            cout << "error file" << endl;
            return EXIT_FAILURE;
        }
        values_information >> text >> Valor_proute;
        if(text != "Valor_proute") {
            cout << "error file" << endl;
            return EXIT_FAILURE;
        }
        values_information >> text >> Valor_pdepot;
        if(text != "Valor_pdepot") {
            cout << "error file" << endl;
            return EXIT_FAILURE;
        }
        values_information >> text >> Valor_pmax;
        if(text != "Valor_pmax") {
            cout << "error file" << endl;
            return EXIT_FAILURE;
        }
        values_information >> text >> Nmovpen;
        if(text != "Nmovpen") {
            cout << "error file" << endl;
            return EXIT_FAILURE;
        }
        values_information >> text >> Repair;
        if(text != "Repair") {
            cout << "error file" << endl;
            return EXIT_FAILURE;
        }
#endif

        int valor_Ciclos = 10; // Numero de veces Ciclos
        int Nmovbeta = round(Valor_nmovbeta * Ncostumers); // Numero de iteracciones para el cambio de beta
        int Perturb = round(Valor_perturb * Ncostumers); // Numero de iteraciones perturbar
        int Diversification = round(Valor_div * Ncostumers); // Numero de iteraciones diversificar
        int ciclos = valor_Ciclos * Ncostumers; // Numero de iteracciones
        float Oldbeta = (float) Beta; // Beta anterior

        // Contadores y debug
        int vueltas = 0;
        int Nmoviter = 0; // Numero de iteracciones desarrolladas despues del cambio de beta
        int contador_depot = 0; // contador de movimientos factibles e infactibles
        int contador_route = 0; // contador de movimientos factibles e infactibles
        int cont_mejora = 0; // contador de movimientos de mejora o no de la funcion objetivo
        int cont_nomov = 0; // contador de no movimientos realizados
        bool estado_depot; // 0 -> infactible y 1 -> factible
        bool estado_route; // 0 -> infactible y 1 -> factible
        bool mejora = 0; // 0 -> no mejora, 1 -> mejora
        bool nomov = 0; // 0 -> No hay movimiento, 1 -> Hay movimiento

        double Total_Dem = 0;
        vector <nclients> C_Data(Ncostumers+(3*Ncostumers)); // Data structure to save the customer data
#ifdef MDVRP

        for (int i = 0; i < Ncostumers; i++){
            C_Data[i].id = i+1;
            C_Data[i].x = mydata.customers[i].x;
            C_Data[i].y = mydata.customers[i].y;
            C_Data[i].dem = mydata.customers[i].demandVolume;
            Total_Dem += C_Data[i].dem;
        }
#else
        // Lectura de archivos
        ifstream Routes_Information((testname + "/" +  "costumer.txt").c_str());
        for (int i = 0; i < Ncostumers; i++){
            Routes_Information >> C_Data[i].id;
            Routes_Information >> C_Data[i].x;
            Routes_Information >> C_Data[i].y;
            Routes_Information >> C_Data[i].dem;
            Total_Dem += C_Data[i].dem;
        }
        Routes_Information.close();
#endif
        vector <ndepots> D_Data(Ndepots); // Data structure to save the depot Data
#ifdef MDVRP
        for (int i = 0; i < Ndepots; i++){
            int ii = i + mydata.ncustomers;
            D_Data[i].id = i+1;
            D_Data[i].x = mydata.customers[ii].x;
            D_Data[i].y = mydata.customers[ii].y;
            D_Data[i].cap = mydata.customers[ii].demand;
            D_Data[i].o_cost = 0; // Costo for openning the depot
        }
#else
        ifstream Depots_Information((testname + "/" +  "depot.txt").c_str());
        for (int i = 0; i < Ndepots; i++){
            Depots_Information >> D_Data[i].id;
            Depots_Information >> D_Data[i].x;
            Depots_Information>> D_Data[i].y;
            Depots_Information >> D_Data[i].cap;
            Depots_Information >> D_Data[i].o_cost;
        }
        Depots_Information.close();
#endif

//        /*****************Adding the relocation points of each customer to the mydata structure****************/
//        // This part is random
//
//        int count_relocation{0};
//        for(int i{0};i<mydata.customers.size()-1;i++){
//
//            mydata.relocation_points.resize(mydata.relocation_points.size()+3);
//            vector< vector<int> > new_points = GenerateNPoints(3,4,mydata.customers[i].x,mydata.customers[i].y);
//
//            for(int j{0};j<new_points.size();j++){
//
//                mydata.relocation_points[count_relocation].customerId = i;
//                mydata.relocation_points[count_relocation].x = new_points[j][0];
//                mydata.relocation_points[count_relocation].y = new_points[j][1];
//                count_relocation+=1;
//
//            }
//        }
//        /******************************************************************************************************/

        /*************************Reading relocation points****************************************************/

        for(int i{0};i<(mydata.ncustomers*3);i++){

            mydata.relocation_points.resize(mydata.relocation_points.size()+1);
            data_input>>mydata.relocation_points[i].customerId;
            data_input>>mydata.relocation_points[i].x;
            data_input>>mydata.relocation_points[i].y;

        }

        data_input.close();

        /******************************************************************************************************/

        /*************************Adding relocation points to C_Data*******************************************/

        int size_of_C_Data = C_Data.size();
        //count_relocation=0;
        int count_relocation{0};
        for (int i = (Ncostumers); i < (Ncostumers+(Ncostumers*3)); i++){
            C_Data[i].id = i+1;
            C_Data[i].x = mydata.relocation_points[count_relocation].x;
            C_Data[i].y = mydata.relocation_points[count_relocation].y;
            // The demand of the relocation point is the same of the current point
            C_Data[i].dem = mydata.customers[mydata.relocation_points[count_relocation].customerId].demandVolume;
            count_relocation++;
        }

        /******************************************************************************************************/

        /*************************Create the file with the problem*********************************************/

        ofstream problem_file("PrintProblem.csv");
        problem_file<<"Index,X,Y"<<endl;
        int count_to_print{0};
        for(int depot{0};depot<D_Data.size();depot++){
            problem_file<<count_to_print<<","<<D_Data[depot].x<<","<<D_Data[depot].y<<endl;
            ++count_to_print;
        }
        for(int customer{0};customer<C_Data.size();customer++){
            problem_file<<count_to_print<<","<<C_Data[customer].x<<","<<C_Data[customer].y<<endl;
            ++count_to_print;
        }
        problem_file.close();

        /********************************************************************************************************/

        vector <double> A_dem;
        vector <float> Capacidades(Ndepots);
        vector <double> Capacities(Ndepots);
        vector < vector <int> > Clusters;
        vector < list <int> > Routes(Clusters.size(),list <int>());
        vector < list <int> > Rutas_iniciales(Clusters.size(),list <int>());
        size_t Nclusters = (int) ceil ((double)Total_Dem / Veh_Cap);
        cout<<"NUMBER OF CLUSTERS: "<<Nclusters<<endl;
        double funcion_obj = 0;
        char dll_exe[] = "lkh_main";

        // Distance matrix of complete graph
        vector < vector <dist_t> > Distancias((Ndepots + Ncostumers + (3*Ncostumers)),vector <dist_t>());
        for (int i = 0; i < Ndepots; ++i) {

            for (int j = 0; j < Ndepots; ++j) {

                if (Round == 0){

                    Distancias[i].push_back((dist_t)sqrt(pow(D_Data[i].x - D_Data[j].x,2) + pow(D_Data[i].y - D_Data[j].y,2)));

                } else {

                    Distancias[i].push_back((dist_t)ceil((sqrt(pow(D_Data[i].x - D_Data[j].x,2)
                                                             + pow(D_Data[i].y - D_Data[j].y,2)))*100));

                }
            }
        }

        for (int i = 0; i < Ndepots; ++i) {

            for (int j = Ndepots; j < (Ndepots + Ncostumers + (3*Ncostumers)); ++j) {

                if (Round == 0){

                    Distancias[i].push_back((dist_t)sqrt(pow(D_Data[i].x - C_Data[j - Ndepots].x,2)
                                                       + pow(D_Data[i].y - C_Data[j - Ndepots].y,2)));

                } else {

                    Distancias[i].push_back((dist_t)ceil((sqrt(pow(D_Data[i].x - C_Data[j - Ndepots].x,2)
                                                             + pow(D_Data[i].y - C_Data[j - Ndepots].y,2)))*100));

                }
            }
        }

        for (int i = Ndepots; i < (Ndepots + Ncostumers + (3*Ncostumers)); ++i) {

            for (int j = 0; j < Ndepots; ++j) {

                if (Round == 0){

                  Distancias[i].push_back((dist_t)sqrt(pow(C_Data[i - Ndepots].x - D_Data[j].x,2)
                                                     + pow(C_Data[i - Ndepots].y - D_Data[j].y,2)));

                } else {

                    Distancias[i].push_back((dist_t) ceil((sqrt(pow(C_Data[i - Ndepots].x - D_Data[j].x,2)
                                                              + pow(C_Data[i - Ndepots].y - D_Data[j].y,2)))*100));

                }
            }
        }

        for (int i = Ndepots; i < (Ndepots + Ncostumers + (3*Ncostumers)); ++i) {

            for (int j = Ndepots; j < (Ndepots + Ncostumers + (3*Ncostumers)); ++j) {

                if (Round == 0){

                   Distancias[i].push_back((dist_t)sqrt(pow(C_Data[i - Ndepots].x - C_Data[j - Ndepots].x,2)
                                                      + pow(C_Data[i - Ndepots].y - C_Data[j - Ndepots].y,2)));

                } else {

                    Distancias[i].push_back((dist_t)ceil((sqrt(pow(C_Data[i - Ndepots].x - C_Data[j - Ndepots].x,2)
                                                             + pow(C_Data[i - Ndepots].y - C_Data[j - Ndepots].y,2)))*100));

                }
            }
        }

        int count_distances{0};
        dist_t sum_distances{0.0};

        for(int i{0};i<Distancias.size();i++){

            for(int j{0};j<Distancias[i].size();j++){

                if(Distancias[i][j]>0){

                    count_distances+=1;
                    sum_distances+=Distancias[i][j];

                }
            }
        }

        dist_t average_distances = sum_distances/float(count_distances);

        PrintDistanceMatrix(Distancias,"[","]");

        int reloc_points_reference{75};

        cout<<"[";
        for(int i{0};i<75;i++){

            cout<<"[";

            for(int j{0};j<300;j++){

                if(i==j || j==reloc_points_reference ||
                   j==reloc_points_reference+1 ||
                   j==reloc_points_reference+2){

                    cout<<"1";

                }else{

                    cout<<"0";

                }

                if(j!=299){

                    cout<<",";

                }

            }
            cout<<"],"<<endl;
            reloc_points_reference+=3;
        }
        cout<<"]"<<endl;

        for(int i{0};i<C_Data.size();i++){

            cout<<C_Data[i].dem<<",";
        }

        cout<<endl;

#ifdef NOLKH
        string tmpstr;
        int nroute;
        ifstream fileinitial((testname + "/" +  "rutas.txt").c_str());
        Clusters.clear();
        fileinitial >> tmpstr;
        if (tmpstr != "Rutas") {
            cout << "error Rutas";
            throw tmpstr;
        }
        fileinitial >> nroute;
        for (int i = 0; i < nroute; ++i ) {
            int nnode;
            fileinitial >> tmpstr >> nnode ;
            if (tmpstr != "Ruta") {
                cout << "error Ruta" << i << endl;
                throw tmpstr;
            }
            Clusters.push_back(vector<int>(nnode, 0));
            A_dem.push_back(0);
            for (int j = 0; j < nnode; ++j) {
                int node;
                fileinitial >> node;
                Clusters[i][j] = node;
            }
        }
        fileinitial >> tmpstr;
        if (tmpstr != "EOF") {
            cout << "error EOF";
            throw tmpstr;
        }

        Routes.resize(Clusters.size());
        for (size_t i = 0; i < Clusters.size(); ++i) {
            Routes[i].assign(Clusters[i].begin(),Clusters[i].end());
        }
        Rutas_iniciales.assign(Routes.begin(),Routes.end());

        for (size_t i = 0; i < Clusters.size(); ++i) {
            for (size_t j = 1; j < Clusters[i].size(); ++ j) {
                A_dem[i] += C_Data[Clusters[i][j] - (Ndepots + 1)].dem;
            }
        }

        for (size_t i = 0; i < Routes.size(); ++i){
            Capacidades[Routes[i].front()-1] += (float) A_dem[i];
        }
        Capacities.assign(Capacidades.begin(),Capacidades.end());

        Partial_Variable = disrutas(Distancias,Routes);
        Partial_Fix_Route = (Route_Cost * Routes.size());

        for (size_t i = 0; i < Capacidades.size(); i++){
            if (Capacidades[i] > 0.01){
                Partial_Open += D_Data[i].o_cost;
            }
        }

        funcion_obj = (Partial_Variable + Partial_Fix_Route);
#else
        // ALGORITMO DE INICIALIZACION

        double Cplex_Time = 0;
        double Lkh_Time = 0;
        clock_t t_inicial = clock();

        // Seleccion de clusters usando heuristica
        ofstream Problem((testname + "/" +  "Giant_Problem.txt").c_str());
        Problem << "NAME : " << "Giant_Problem.txt" << endl;
        Problem << "COMMENT : " << endl;
        Problem << "TYPE : TSP" << endl;
        Problem << "DIMENSION : " << Ncostumers << endl;
        Problem << "EDGE_WEIGHT_TYPE : EUC_2D" << endl;
        Problem << "NODE_COORD_SECTION" << endl;
        Problem << 1 << '\t' << C_Data[0].x << '\t' << C_Data[0].y << '\t' << endl;
        int Counter = 1;
        for (int i = 1; i < Ncostumers; ++i){
            Problem << ++Counter << '\t' << C_Data[i].x << '\t' << C_Data[i].y << endl;
        }
        Problem << "EOF" << endl;

        ofstream Parameters((testname + "/" +  "Giant_Parameters.txt").c_str());
        Parameters << "PROBLEM_FILE = " << testname << "/" <<  "Giant_Problem.txt" << endl;
        Parameters << "TRACE_LEVEL = 0" << endl;
        Parameters << "GAIN23 = NO" << endl;
        Parameters << "RUNS = 1" << endl;
        Parameters << "OUTPUT_TOUR_FILE " << testname << "/" <<  "Giant_Tour.txt" << endl;
        Problem.close();
        Parameters.close();

        /*************** Solution LKH of the Giant tour *********************/

        char dll_par[1024];
        strcpy(dll_par, (testname+string("/Giant_Parameters.txt")).c_str());
        char *dll_argv[] = { dll_exe , dll_par };
        clock_t inicio_lkh = clock();
        lkh_main(2 , dll_argv);
        Lkh_Time += (clock()- inicio_lkh);

        /*********************************************************************/

        int Nhead = 6;
        ifstream Solution((testname + "/" +  "Giant_Tour.txt").c_str());

        // Skip Nhead lines in the file. The information of the Giant Tour is written in these lines
        for (int k = 0; k < Nhead; ++k) {

            char Mytemp[1024];
            Solution.getline(Mytemp, 1024);

        }

        // Write one line for each customer
        vector <int> Sequence(Ncostumers);
        for(int k = 0 ; k < Ncostumers; ++k){

            Solution >> Sequence[k];

        }

        double Minor_Objective = FLT_MAX;
        double Objective = 0;

		int infact = 0;

        /***********Loop to build the clusters****************/

        int total_customer_volume{0};
        for(int cust{0};cust<mydata.customers.size();++cust){

            total_customer_volume+=mydata.customers[cust].demandVolume;
        }

        cout<<"TOTAL CUSTOMER DEMAND: "<<total_customer_volume<<endl;

        dist_t average_demand = float(total_customer_volume)/float(mydata.ncustomers);

        mydata.factor_demand_dist = (100*average_distances)/average_demand;

        cout<<"FACTOR DEMAND/DISTANCE = "<<mydata.factor_demand_dist<<endl;

        list < vector <int> > partial_clusters; // List of clusters
        list < vector <vector <int> > > partial_recollection_types; // List of type of recollection
        partial_clusters.push_back(vector <int>());
        partial_recollection_types.push_back(vector <vector <int> >());
        vector <double> partial_demand(9999,0); // Vector with 9999 entries full with zero
		list< vector<int> >::iterator itGroups = partial_clusters.begin();
		list< vector< vector<int> > >::iterator itRecTypes = partial_recollection_types.begin();
		size_t Group = 0;
		int temp_demand{0};
        vector <int> temp_rec = {1,1,1};

        for(int i = 0; i < Ncostumers; ++i){

            //cout<<i<< " PARTIAL SEQUENCE..."<<endl;
            vector <int> partial_sequence;
            vector <vector <int> > partial_recTypes_sequence;
            partial_sequence.reserve(Ncostumers); // Change size of the vector Partial_Sequence to Ncustomers
            partial_recTypes_sequence.reserve(Ncostumers);

            for (size_t j = i; j < Sequence.size(); ++j) {

                partial_sequence.push_back(Sequence[j]);
                vector <int> temp_vector(mydata.numberTypeWaste,1);
                partial_recTypes_sequence.push_back(temp_vector);

            }

            for (int j = 0; j < i; ++j) {

                partial_sequence.push_back(Sequence[j]);
                vector <int> temp_vector(mydata.numberTypeWaste,1);
                partial_recTypes_sequence.push_back(temp_vector);

            }

            while(partial_sequence.size()>0){

                if(temp_demand != 0){


                    for(int i{0};i<temp_rec.size();i++){cout<<temp_rec[i]<<", ";}

                    cout<<endl;
                   (*itGroups).push_back(partial_sequence.front());
                   vector <int> temp_vector_to_push = OneForZeros(temp_rec);

                   for(int i{0};i<temp_vector_to_push.size();i++){cout<<temp_vector_to_push[i]<<", ";}

                    cout<<endl;
                   (*itRecTypes).push_back(temp_vector_to_push);
                   partial_demand[Group]+=temp_demand;
                   vector_erase(partial_sequence,partial_sequence.front());
                   partial_recTypes_sequence.erase(partial_recTypes_sequence.begin());
                   temp_demand=0;
                   temp_rec={1,1,1};

                }

                else{

                    if(partial_demand[Group]+C_Data[partial_sequence.front()-1].dem <= Veh_Cap){

                        (*itGroups).push_back(partial_sequence.front());
                        (*itRecTypes).push_back(partial_recTypes_sequence.front());
                        partial_demand[Group]+=C_Data[partial_sequence.front()-1].dem;
                        vector_erase(partial_sequence,partial_sequence.front());
                        partial_recTypes_sequence.erase(partial_recTypes_sequence.begin());
                        temp_demand=0;

                    }else{

                        vector <vector<int>> optimal_combination_vector;
                        int available_capacity = Veh_Cap - partial_demand[Group];
                        optimal_combination_vector= CombinationToLoad(available_capacity,mydata.boxes,mydata.numberTypeWaste,
                                                                      partial_sequence.front()-1,mydata);

                        optimal_combination_vector=vector<vector<int>>(); //---> CHANGE TO DISABLE SPLIT DEMAND

                        if(optimal_combination_vector.size()==0){

                            partial_clusters.push_back(vector<int>());
                            partial_recollection_types.push_back(vector< vector<int> >());
                            temp_demand=0;
                            ++itGroups;
                            ++itRecTypes;
                            ++Group;

                        }else{

                            (*itGroups).push_back(partial_sequence.front());
                            temp_rec = optimal_combination_vector[0];

                            for(int i{0};i<temp_rec.size();i++){cout<<temp_rec[i]<<", ";}

                            cout<<endl;
                            (*itRecTypes).push_back(temp_rec);
                            temp_demand=C_Data[partial_sequence.front()-1].dem-optimal_combination_vector[1][0];
                            cout<<temp_demand<<endl;
                            partial_clusters.push_back(vector<int>());
                            partial_recollection_types.push_back(vector< vector<int> >());
                            ++itGroups;
                            ++itRecTypes;
                            ++Group;

                        }
                    }
                }
            }

            if(i<(Ncostumers-1)){

                partial_clusters.push_back(vector<int>());
                partial_recollection_types.push_back(vector< vector<int> >());
                temp_demand=0;
                ++itGroups;
                ++itRecTypes;
                ++Group;

            }
        }
        /*******************************************************************************/

		/**Test to know if all the cluster can be loaded in the vehicle**/

        int count_cluster{1};
        list< vector< vector<int>>>::iterator itRecTypes2 = partial_recollection_types.begin();

        for(auto& cluster : partial_clusters){

            vector< vector<int>> temp_rec_vector = (*itRecTypes2);
            int recollected{0};

            for(int cust{0};cust<cluster.size();cust++){

                for(int waste_type{1};waste_type<=mydata.numberTypeWaste;waste_type++){

                    if(temp_rec_vector[cust][waste_type-1]==1){

                        recollected+=volumeWasteType(mydata.boxes,waste_type,cluster[cust]-1,mydata);

                    }
                }
            }


            if(recollected > Veh_Cap){

                cout<<"Error in cluster "<<count_cluster<<"!"<<endl;

            }

            count_cluster+=1;
            itRecTypes2++;

        }

        /*****************************************************************************/

        cout<<"SIZE OF PARTIAL RECOLLECTION TYPE!!"<<partial_recollection_types.size()<<endl;

        /*************** Writing the distance of each cluster*************************/

		vector < double > partial_distances(partial_clusters.size(),0);
		size_t k = 0;

		for (auto& cluster_k :  partial_clusters) {

			partial_distances[k]+=Distancias[0][cluster_k[0]];
			partial_distances[k]+=Distancias[cluster_k[cluster_k.size()-1]][0];

			for (int j = 1; j < cluster_k.size(); j++){

				partial_distances[k]+=Distancias[cluster_k[j-1]][cluster_k[j]];

			}

			++k;
		}

		Group = partial_clusters.size();

		/*****************************************************************************/

		/*****************Writing the file of the solution clusters******************/

		ofstream solucion((testname + "/" + filename +  "cluster.txt").c_str());

		k = 0;
		list<vector< vector<int> > >::iterator itPrint = partial_recollection_types.begin();

		for (auto& cluster_k :  partial_clusters) {

            vector<vector<int>> temp_vector = (*itPrint);

			for (size_t l = 0; l < cluster_k.size(); ++l) {

				solucion << cluster_k[l] << "/";

				for(int type{0};type<mydata.numberTypeWaste;type++){

                    if(type != (mydata.numberTypeWaste-1)){

                        solucion<<temp_vector[l][type]<<",";

                    }
                    else{

                        solucion<<temp_vector[l][type]<<" ";

                    }
				}
			}

			solucion << partial_distances[k]<<endl;
			++k;
			++itPrint;

		}

		solucion << "EOF" << endl;
		solucion.close();
		/****************************************************************************/


		/***************Mathematical model to define routes*************************/

        list <vector <int> > optimal_clusters; // List of optimal cluster after mathematical model
        list <vector <vector <int> > > optimal_rec_types; // List of optimal recollection types

		IloEnv env;

		try{

            IloCplex cplex(env);

            IloModel model(env);

            IloInt total_clusters, number_customers, number_depots;

            total_clusters = partial_clusters.size();

            number_customers = Ncostumers;

            number_depots = Ndepots;

            cout<<total_clusters<<", "<<number_customers<<", "<<number_depots<<endl;

            /**Variables**/

            // yl --> 1 if the cluster l is chosen, 0 if isn't chosen
            IloBoolVarArray cluster_var(env,total_clusters);

            for(int cluster{0};cluster<total_clusters;cluster++){

                stringstream var_name;
                var_name<<"y("<<cluster<<")";
                cluster_var[cluster].setName(var_name.str().c_str());

            }

            /**Objective**/

            // Objective --> Minimize total distance
            IloExpr total_distance(env);

            for(int k{0};k<total_clusters;k++){

                IloInt distance = partial_distances[k];
                total_distance+=distance*cluster_var[k];

            }

            model.add(IloMinimize(env, total_distance));


            /**Constraints**/

            // Restriction that gives values to xj with yl
            for(int customer{1};customer<=number_customers;customer++){

                for(int waste_type{0};waste_type<mydata.numberTypeWaste;waste_type++){

                    IloExpr suma_customer_waste(env);
                    list< vector< vector<int>>>::iterator itRecollection = partial_recollection_types.begin();
                    IloInt cluster_index = 0;

                    for(auto& cluster : partial_clusters){

                        vector< vector<int>> current_rec_types = (*itRecollection);

                        for(int customer_in_cluster{0};customer_in_cluster<cluster.size();customer_in_cluster++){

                            if(cluster[customer_in_cluster]==customer &&
                               current_rec_types[customer_in_cluster][waste_type]==1){

                                suma_customer_waste+=cluster_var[cluster_index];
                                break;

                               }
                        }

                        cluster_index+=1;
                        ++itRecollection;

                    }

                    model.add(suma_customer_waste==1);

                }
            }


            // Number of selected cluster must be less than number of vehicles
            IloExpr number_clusters(env);

            for(int l{0};l<total_clusters;l++){

                number_clusters+=cluster_var[l];

            }

            IloInt number_minimum_clusters = int(Nclusters);
            model.add(number_clusters>=(number_minimum_clusters-1));
            model.add(number_clusters<=mydata.nvehicles);
            number_clusters.end();

            cplex.clearModel();
            cplex.setOut(env.getNullStream());
            cplex.extract(model);
            cplex.setParam(IloCplex::RepeatPresolve,3);
            cplex.exportModel ((testname + "/" +  "modelo.lp").c_str());
            env.out()<<"Binary variables: "<<cplex.getNbinVars()<<endl;
            env.out()<<"Integer variables: "<<cplex.getNintVars()<<endl;
            env.out()<<"Number of constraints: "<<cplex.getNrows()<<endl;
            env.out()<<"Number of resources: "<<cplex.getNcols()<<endl;

            clock_t inicio_cplex = clock();

            if (!cplex.solve()) {

                env.error() << "It couldn't be solved!" << endl;

            }

            Cplex_Time += (clock()- inicio_cplex);

            Objective = cplex.getObjValue();
            cout<<"The minimun distance is = "<<Objective<<endl;
            cout<<"The solution time was = "<<Cplex_Time<<endl;

            /*****Print solution clusters and put them into optimal clusters vector****/

            cout<<"SOLUTION CLUSTERS"<<endl;

            optimal_clusters.push_back(vector<int> ());
            optimal_rec_types.push_back(vector<vector<int>>());
            list <vector <int>>::iterator it_opt_clusters = optimal_clusters.begin();
            list <vector<vector<int>>>::iterator it_opt_rec_types = optimal_rec_types.begin();
            list <vector <int>>::iterator it_clusters = partial_clusters.begin();
            list <vector<vector<int>>>::iterator it_rec_types = partial_recollection_types.begin();

            for(int cluster{0};cluster<total_clusters;cluster++){

                if(cplex.getValue(cluster_var[cluster])==1){

                    cout<<"y("<<cluster<<") = "<<cplex.getValue(cluster_var[cluster])<<". ";
                    vector<int> temp_cluster = (*it_clusters);
                    vector<vector<int>> temp_rec_cluster = (*it_rec_types);

                    for(int i{0};i<temp_cluster.size();i++){

                        (*it_opt_clusters).push_back(temp_cluster[i]);
                        (*it_opt_rec_types).push_back(temp_rec_cluster[i]);

                    }

                    optimal_clusters.push_back(vector<int>());
                    optimal_rec_types.push_back(vector<vector<int>>());
                    ++it_opt_clusters;
                    ++it_opt_rec_types;

                }

                ++it_clusters;
                ++it_rec_types;

            }

            optimal_clusters.pop_back();
            optimal_rec_types.pop_back();
            cout<<endl;

            /*****************************************************************************/

		}catch (IloException& ex){

		    cerr<<"Error cplex: "<<ex<<endl;

		}catch (...){

		    cerr<<"Error cpp"<<endl;
		}

		env.end();



        /**************************** Applying LKH to each cluster ********************************************/

        vector < vector <int>> post_lkh_clusters;
        list <vector <int>>::iterator it_opt_clusters = optimal_clusters.begin();

        for(int opt_cluster{0};opt_cluster<optimal_clusters.size();opt_cluster++){

            vector <int> temp_vector = (*it_opt_clusters);
            post_lkh_clusters.push_back(temp_vector);
            ++it_opt_clusters;

        }

        ofstream ClustersFile("ClustersFile.csv");
        ClustersFile<<"Cluster,X,Y"<<endl;

        for(int cluster{0};cluster<post_lkh_clusters.size();cluster++){

            for(int customer{0};customer<post_lkh_clusters[cluster].size();customer++){

                ClustersFile<<cluster<<","<<C_Data[post_lkh_clusters[cluster][customer]-1].x\
                <<","<<C_Data[post_lkh_clusters[cluster][customer]-1].y<<endl;
            }
        }

        ClustersFile.close();


        /*
        double Lkh_Time2 = 0;
        clock_t t_inicial2 = clock();

        vector < vector <int>> post_lkh_clusters;
        list <vector <int>>::iterator it_opt_clusters = optimal_clusters.begin();
        for(int opt_cluster{0};opt_cluster<optimal_clusters.size();opt_cluster++){

            vector <int> temp_vector = (*it_opt_clusters);
            stringstream nn;
            nn<<testname<<"/summary_lkh/"<<"cluster_"<<opt_cluster<<".txt";
            ofstream file(nn.str().c_str());
            //file << "NAME : " << nn.str() << endl;
            file << "NAME : " <<"cluster_" <<opt_cluster<<".txt"<< endl;
            file << "COMMENT : " << endl;
            file << "TYPE : TSP" << endl;
            file << "DIMENSION : " << temp_vector.size()<< endl;
            file << "EDGE_WEIGHT_TYPE : EUC_2D" << endl;
            file << "NODE_COORD_SECTION" << endl;

            for(int counter{0};counter<temp_vector.size();counter++){

                file<<counter+1<<'\t'<<C_Data[temp_vector[counter]-1].x<<'\t'<<C_Data[temp_vector[counter]-1].y<<endl;

            }

            file<<"EOF"<<endl;

            file.close();


            stringstream name;
            name<<testname<<"/summary_lkh/"<<"cluster_"<<opt_cluster<<"_parameters.txt";
            ofstream file2(name.str().c_str());
            file2 << "PROBLEM_FILE = " << nn.str() << endl;
            file2 << "TRACE_LEVEL = 0" << endl;
            file2 << "GAIN23 = NO" << endl;
            file2 << "RUNS = 1" << endl;
            file2 << "OUTPUT_TOUR_FILE " << testname << "/summary_lkh/" << "partial_cluster_" << opt_cluster<<".txt"<<endl;

            file2.close();


            if(temp_vector.size()>=3){
                char dll_exe2[] = "lkh_main";
                char dll_par2[1024];
                strcpy(dll_par2, name.str().c_str());
                char *dll_argv[] = { dll_exe2 , dll_par2 };
                inicio_lkh = clock();
                lkh_main(2 , dll_argv);
                Lkh_Time += (clock()- inicio_lkh);



                vector <int> temp_vector2;
                stringstream tour_name;
                tour_name<<testname << "/summary_lkh/" << "partial_cluster_" << opt_cluster<<".txt";
                ifstream tour(tour_name.str().c_str());

                for(int k{0};k<6;k++){

                    char Mytemp[1024];
                    tour.getline(Mytemp, 1024);

                }

                for(int l{0};l<temp_vector.size();l++){

                    int temp_customer;
                    tour>>temp_customer;
                    temp_vector2.push_back(temp_vector[temp_customer-1]);

                }

                post_lkh_clusters.push_back(temp_vector2);

            }else{

                post_lkh_clusters.push_back(temp_vector);

            }

            ++it_opt_clusters;

        }*/

        /********************************Relocation process**************************/

        // create a matrix full with zeros to put the relocations points
        vector < vector <int>> relocation_decision_matrix;

        for(int i{0};i<Ncostumers;i++){

            vector <int> temp_decision_matrix;

            for(int j{0};j<3;j++){

                temp_decision_matrix.push_back(0);

            }

            relocation_decision_matrix.push_back(temp_decision_matrix);

        }



        int size_C_Data=C_Data.size(); // Constant to be used below

//        for(int cluster{0};cluster<post_lkh_clusters.size();cluster++){
//
//                cout<<"-------------CLUSTER "<<cluster<<"------------"<<endl;
//                cout<<"Size of cluster post lkh = "<<post_lkh_clusters[cluster].size()<<endl;
//                vector <vector<int>> vector_result;
//                vector_result = Vertices(post_lkh_clusters[cluster],C_Data,D_Data); // x and y coordinates are extracted
//                vector <int> x_vertices = vector_result[0]; // x coordinate
//                vector <int> y_vertices = vector_result[1]; // y coordinate
//                int x_vertices_2[x_vertices.size()];
//                copy(x_vertices.begin(),x_vertices.end(),x_vertices_2); // x coordinate to Array
//                int y_vertices_2[y_vertices.size()];
//                copy(y_vertices.begin(),y_vertices.end(),y_vertices_2); // y coordinate to Array
//
//                for(int customer{0};customer<post_lkh_clusters[cluster].size();customer++){
//
//                        cout<<"-------------> CUSTOMER "<<customer<<endl;
//
//                        // Relocations points of the current customer
//                        vector <int> relocation_points_customer = RelocationPoints(post_lkh_clusters[cluster][customer]-1,
//                                                                                   mydata.relocation_points);
//
//                        int points_inside_route{0};
//                        vector <vector<int>> reloc_points_inside_route;
//
//                        cout<<"Number of relocation points of customer "<<customer<<" in cluster "<<cluster<<": "<<relocation_points_customer.size()<<endl;
//
//                        for(int relocation_point{0};relocation_point<relocation_points_customer.size();relocation_point++){
//
//                                vector<int> temp;
//
//                                // verify if the point is inside the route
//                                int inside_the_route=InsideARoute(post_lkh_clusters[cluster].size()+1,x_vertices_2,y_vertices_2,
//                                                                  mydata.relocation_points[relocation_points_customer[relocation_point]].x,
//                                                                  mydata.relocation_points[relocation_points_customer[relocation_point]].y);
//
//                                // if point is inside the route, then it is added to a vector
//                                if(inside_the_route==1){
//
//                                    points_inside_route+=inside_the_route;
//                                    temp.push_back(relocation_points_customer[relocation_point]);
//                                    temp.push_back(relocation_point);
//                                    reloc_points_inside_route.push_back(temp);
//
//                                }
//                        }
//
//                        cout<<"There are "<<points_inside_route<<" inside th route of cluster "<<cluster<<endl;
//
//                        // if there is only one point, modify the route
//                        if(points_inside_route==1){
//                            /*
//                            C_Data[size_C_Data].id=size_C_Data+1;
//                            C_Data[size_C_Data].x=mydata.relocation_points[reloc_points_inside_route[0][0]].x;
//                            C_Data[size_C_Data].y=mydata.relocation_points[reloc_points_inside_route[0][0]].y;
//                            C_Data[size_C_Data].dem=C_Data[post_lkh_clusters[cluster][customer]-1].dem;
//                            */
//                            relocation_decision_matrix[post_lkh_clusters[cluster][customer]-1][reloc_points_inside_route[0][1]]=1;
//                            post_lkh_clusters[cluster][customer]=(reloc_points_inside_route[0][0]+1+Ncostumers);
//                            //size_C_Data+=1;
//                            vector_result = Vertices(post_lkh_clusters[cluster],C_Data,D_Data);
//                            x_vertices=vector_result[0];
//                            y_vertices=vector_result[1];
//                            copy(x_vertices.begin(),x_vertices.end(),x_vertices_2);
//                            copy(y_vertices.begin(),y_vertices.end(),y_vertices_2);
//
//                        // if there are more points, verify what point is closer and modify the route
//                        }else if(points_inside_route>1){
//
//                            float min_distance{999999}; // variable to save the minimum distance
//                            int best_point{0}; // variable to save the best relocation point
//
//                            for(int point{0};point<reloc_points_inside_route.size();point++){
//
//                                // if the current customer is the first customer of the cluster
//                                if(customer==0){
//
//                                    float temp_distance=DistanceToALine(mydata.relocation_points[reloc_points_inside_route[point][0]].x,
//                                                                      mydata.relocation_points[reloc_points_inside_route[point][0]].y,
//                                                                      0,true,post_lkh_clusters[cluster][customer+1],false,C_Data,D_Data);
//                                    cout<<temp_distance<<"<------------"<<endl;
//                                    if(temp_distance<min_distance){
//                                        min_distance=temp_distance;
//                                        best_point=point;
//
//                                    }
//                                }
//
//                                // if the current customer is the last customer of the cluster
//                                else if(customer==post_lkh_clusters[cluster].size()-1){
//
//                                    float temp_distance=DistanceToALine(mydata.relocation_points[reloc_points_inside_route[point][0]].x,
//                                                                      mydata.relocation_points[reloc_points_inside_route[point][0]].y,
//                                                                      post_lkh_clusters[cluster][customer-1],false,0,true,C_Data,D_Data);
//                                    cout<<temp_distance<<"<----------------"<<endl;
//                                    if(temp_distance<min_distance){
//
//                                        min_distance=temp_distance;
//                                        best_point=point;
//
//                                    }
//                                }
//
//                                else{
//
//                                    float temp_distance=DistanceToALine(mydata.relocation_points[reloc_points_inside_route[point][0]].x,
//                                                                      mydata.relocation_points[reloc_points_inside_route[point][0]].y,
//                                                                      post_lkh_clusters[cluster][customer-1],false,
//                                                                      post_lkh_clusters[cluster][customer+1],false,C_Data,D_Data);
//                                    cout<<temp_distance<<"<----------------"<<endl;
//                                    if(temp_distance<min_distance){
//
//                                        min_distance=temp_distance;
//                                        best_point=point;
//
//                                    }
//                                }
//                                /*
//                                C_Data[size_C_Data].id=size_C_Data+1;
//                                C_Data[size_C_Data].x=mydata.relocation_points[reloc_points_inside_route[best_point][0]].x;
//                                C_Data[size_C_Data].y=mydata.relocation_points[reloc_points_inside_route[best_point][0]].y;
//                                C_Data[size_C_Data].dem=C_Data[post_lkh_clusters[cluster][customer]-1].dem;
//                                relocation_decision_matrix[post_lkh_clusters[cluster][customer]-1][reloc_points_inside_route[best_point][1]]=1;
//                                post_lkh_clusters[cluster][customer]=size_C_Data+1;
//                                size_C_Data+=1;
//                                vector_result = Vertices(post_lkh_clusters[cluster],C_Data,D_Data);
//                                cout<<"TEST"<<endl;
//                                x_vertices=vector_result[0];
//                                y_vertices=vector_result[1];
//                                copy(x_vertices.begin(),x_vertices.end(),x_vertices_2);
//                                copy(y_vertices.begin(),y_vertices.end(),y_vertices_2);*/
//
//                            }
//                            /*
//                            C_Data[size_C_Data].id=size_C_Data+1;
//                            C_Data[size_C_Data].x=mydata.relocation_points[reloc_points_inside_route[best_point][0]].x;
//                            C_Data[size_C_Data].y=mydata.relocation_points[reloc_points_inside_route[best_point][0]].y;
//                            C_Data[size_C_Data].dem=C_Data[post_lkh_clusters[cluster][customer]-1].dem;
//                            */
//                            relocation_decision_matrix[post_lkh_clusters[cluster][customer]-1][reloc_points_inside_route[best_point][1]]=1;
//                            post_lkh_clusters[cluster][customer]=(reloc_points_inside_route[best_point][0]+1+Ncostumers);
//                            //size_C_Data+=1;
//                            vector_result = Vertices(post_lkh_clusters[cluster],C_Data,D_Data);
//                            x_vertices=vector_result[0];
//                            y_vertices=vector_result[1];
//                            copy(x_vertices.begin(),x_vertices.end(),x_vertices_2);
//                            copy(y_vertices.begin(),y_vertices.end(),y_vertices_2);
//                            cout<<"TEST"<<endl;
//                        }
//
//                        // vector of points is deleted
//                        reloc_points_inside_route.clear();
//                        cout<<"DELETED"<<endl;
//
//                }
//
//        }


        //PrintMatrix(relocation_decision_matrix,"[","]");
        PrintMatrix(post_lkh_clusters,"(",")");

        vector<dist_t> test;
        test=FunctionObjective(post_lkh_clusters,Distancias,optimal_rec_types,mydata);
        //test={Objective,1};

        cout<<"INTIAL FUNCTION OBJECTIVE = "<<test[0]<<endl;
        cout<<"Size of reloc points vector = "<<mydata.relocation_points.size()<<endl;

        dist_t temp_excess{0.0};
        dist_t temp_rcost{0.0};

        dist_t total_cost_reloc{0.0};

        vector<dist_t> excess_per_cluster;

        for(int i{0};i<post_lkh_clusters.size();i++){

            dist_t temp{0.0};

            for(int j{0};j<post_lkh_clusters[i].size();j++){

                temp+=mydata.customers[post_lkh_clusters[i][j]-1].demandVolume;
            }

            excess_per_cluster.push_back(temp-mydata.volumeVehicle);
            //cout<<"------------- "<<temp-mydata.volumeVehicle<<endl;
        }


        ResultInsertion temp_struct;
        temp_struct.solution=vector<vector<int>>();
        temp_struct.result=0.0;
        temp_struct.reloc_cost=0.0;

        for(int cluster{0};cluster<post_lkh_clusters.size();cluster++){

            for(int customer{0};customer<post_lkh_clusters[cluster].size();customer++){

                if(customer==0){

                    temp_struct=Insertion(post_lkh_clusters,Distancias,0,post_lkh_clusters[cluster][customer],
                              post_lkh_clusters[cluster][customer+1],mydata);

                    if(temp_struct.result<0){

                       test[0]+=temp_struct.result;
                       post_lkh_clusters=temp_struct.solution;
                       temp_rcost+=temp_struct.reloc_cost;

                    }

                }else if(customer==post_lkh_clusters[cluster].size()-1){

                    temp_struct=Insertion(post_lkh_clusters,Distancias,post_lkh_clusters[cluster][customer-1],
                                       post_lkh_clusters[cluster][customer],0,mydata);

                    if(temp_struct.result<0){

                       test[0]+=temp_struct.result;
                       post_lkh_clusters=temp_struct.solution;
                       temp_rcost+=temp_struct.reloc_cost;

                    }

                }else{

                    temp_struct=Insertion(post_lkh_clusters,Distancias,post_lkh_clusters[cluster][customer-1],
                                       post_lkh_clusters[cluster][customer],post_lkh_clusters[cluster][customer+1],mydata);

                    if(temp_struct.result<0){

                       test[0]+=temp_struct.result;
                       post_lkh_clusters=temp_struct.solution;
                       temp_rcost+=temp_struct.reloc_cost;

                    }

                }
            }
        }


        ResultSwap result_swap;
        result_swap.result=0.0;
        result_swap.solution=vector<vector<int>>();
        result_swap.excess=0.0;
        result_swap.excess_clusters=vector<dist_t>();


        for(int cluster1{0};cluster1<post_lkh_clusters.size();cluster1++){

            for(int cust1{0};cust1<post_lkh_clusters[cluster1].size();cust1++){

                for(int cluster2{cluster1+1};cluster2<post_lkh_clusters.size();cluster2++){

                    for(int cust2{0};cust2<post_lkh_clusters[cluster2].size();cust2++){

                        if(post_lkh_clusters[cluster1][cust1]!=post_lkh_clusters[cluster2][cust2] &&
                           cluster1!=cluster2){

                            result_swap=SwapInterRoute(post_lkh_clusters[cluster1][cust1],
                                                       post_lkh_clusters[cluster2][cust2],
                                                       post_lkh_clusters, Distancias,optimal_rec_types,
                                                       mydata,temp_excess, excess_per_cluster, C_Data);

                            //cout<<"The result is = "<<result_swap.result<<endl;

                            if(result_swap.result<0){

                                cout<<"Changed!"<<endl;
                                test[0]+=result_swap.result;
                                post_lkh_clusters=result_swap.solution;
                                temp_excess=result_swap.excess;
                                excess_per_cluster=result_swap.excess_clusters;

                            }
                        }
                    }
                }
            }
        }


        ResultInsertionRoute result_insertion_route2;
        result_insertion_route2.result=0.0;
        result_insertion_route2.solution=vector<vector<int>>();
        result_insertion_route2.excess=0.0;
        result_insertion_route2.excess_clusters=vector<dist_t>();

        for(int cluster1{0};cluster1<post_lkh_clusters.size();cluster1++){

            for(int cust1{0};cust1<post_lkh_clusters[cluster1].size();cust1++){

                for(int cluster2{cluster1+1};cluster2<post_lkh_clusters.size();cluster2++){

                    for(int cust2{0};cust2<post_lkh_clusters[cluster2].size();cust2++){

                        if(post_lkh_clusters[cluster1][cust1]!=post_lkh_clusters[cluster2][cust2] &&
                           cluster1!=cluster2){

                            result_insertion_route2=InsertionRouteOpt(post_lkh_clusters[cluster1][cust1],
                                                       post_lkh_clusters[cluster2][cust2],
                                                       post_lkh_clusters, Distancias,optimal_rec_types,
                                                       mydata,temp_excess,excess_per_cluster, C_Data);

                            //cout<<"The result is = "<<result_insertion_route2.result<<endl;

                            if(result_insertion_route2.result<0){

                                test[0]+=result_insertion_route2.result;
                                post_lkh_clusters=result_insertion_route2.solution;
                                optimal_rec_types=result_insertion_route2.demand;
                                temp_excess=result_insertion_route2.excess;
                                excess_per_cluster=result_insertion_route2.excess_clusters;

                            }
                        }
                    }
                }
            }
        }

        ResultTwoOpt result_two_opt2;
        result_two_opt2.solution=vector<vector<int>>();
        result_two_opt2.result=0.0;

        for(int clust{0};clust<post_lkh_clusters.size();clust++){

            for(int cust1{0};cust1<post_lkh_clusters[clust].size();cust1++){

                for(int cust2{0};cust2<post_lkh_clusters[clust].size();cust2++){

                    result_two_opt2=TwoOptOperator(post_lkh_clusters[clust][cust1],
                                                   post_lkh_clusters[clust][cust2],
                                                   post_lkh_clusters, Distancias,mydata);

                    if(result_two_opt2.result<0){

                        test[0]+=result_two_opt2.result;
                        post_lkh_clusters=result_two_opt2.solution;

                    }
                }
            }
        }

        dist_t first_phase_fo=test[0];
        dist_t first_reloc_cost=temp_rcost;
        dist_t first_phase_temp_excess=temp_excess;

        vector<dist_t> first_phase_excess_clusters=excess_per_cluster;
        vector<vector<int>> first_phase_solution = post_lkh_clusters;
        list<vector<vector<int>>> first_phase_demand = optimal_rec_types;

        cout<<"IMPROVED OBJECTIVE FUNCTION = "<<test[0]<<endl;
        cout<<PrintMatrix(post_lkh_clusters,"[","]")<<endl;

        struct best_solution{

            vector<vector<int>> solution;
            dist_t objective_function;
            list<vector<vector<int>>> rec_types;
            dist_t excess;
            dist_t reloc_cost;
            vector<dist_t> excess_clusters;

        };

        best_solution bestSolution;

        bestSolution.solution=post_lkh_clusters;
        bestSolution.objective_function=first_phase_fo;
        bestSolution.rec_types=optimal_rec_types;
        bestSolution.excess=temp_excess;
        bestSolution.reloc_cost=temp_rcost;
        bestSolution.excess_clusters=excess_per_cluster;

        vector<int> tracking_fo;
        tracking_fo.push_back(bestSolution.objective_function);


        /**********************************Improvement phase***************************************************/

        cout<<"IMPROVEMENT PHASE..."<<endl;

        int max_iterations = (1000000/pow(mydata.ncustomers,2))*10;

        cout<<"NUMBERS OF ITERATIONS = "<<max_iterations<<endl;

        int it_perturbation=0.1*max_iterations;

        tabu tabu_list;

        vector<int> selected_operators = {0,0,0,0,0,0};

        vector<vector<int>> distr_prob_excess = {{1,1,1},{1,1,1},{1,1,1}};

        int size_prob_excess = 9;

        int increments_fo{0};

        for(int it{0};it<max_iterations;it++){

            //cout<<"ITERATION "<<it<<"...."<<endl;

            //cout<<"Size of Tabu List = "<<tabu_list.lista.size()<<endl;

            tabu_list.refresh();

            int temp_increment{0};

            //tabu_list.mostrar();

            //cout<<"Tabu updated..."<<endl;

            float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

            //cout<<"THE RANDOM NUMBER IS "<<r<<endl;

            string chosen_operator="";
            dist_t improvement = 9999999999.0;
            int node_o{0};
            int node_d{0};

            dist_t best_insertion=9999999999.0;
            int node_o_insertion=0;
            int node_d_insertion=0;

            dist_t best_two_opt = 9999999999.0;
            int node_o_two_opt = 0;
            int node_d_two_opt = 0;

            dist_t best_swapinter=9999999999.0;
            int node_o_swapinter=0;
            int node_d_swapinter=0;

            dist_t best_insertion_route = 9999999999.0;
            int node_o_insertion_route = 0;
            int node_d_insertion_route = 0;



            if(r<=0.5){

                /**********Estimation of Insertion****************************/

                //cout<<"--> Estimation of Insertion <--"<<endl;

                dist_t temp_imp=0.0;

                for(int clust{0};clust<post_lkh_clusters.size();clust++){

                    for(int cust{0};cust<post_lkh_clusters[clust].size();cust++){

                        if(cust==0){

                            temp_imp=InsertionEstimator(Distancias,0,post_lkh_clusters[clust][cust],
                                                        post_lkh_clusters[clust][cust+1],mydata);

                        }

                        else if(cust==post_lkh_clusters[clust].size()-1){

                            temp_imp=InsertionEstimator(Distancias,post_lkh_clusters[clust][cust-1],post_lkh_clusters[clust][cust],
                                                        0,mydata);

                        }else{

                            temp_imp=InsertionEstimator(Distancias,post_lkh_clusters[clust][cust-1],post_lkh_clusters[clust][cust],
                                                        post_lkh_clusters[clust][cust+1],mydata);

                        }

                        if(temp_imp<improvement && tabu_list.check(post_lkh_clusters[clust][cust],0)==false){

                            chosen_operator="Insertion";
                            improvement=temp_imp;
                            node_o=post_lkh_clusters[clust][cust];
                            node_d=0;
                            if(temp_imp>0){temp_increment=1;}

                        }

                        if(temp_imp<best_insertion){

                            best_insertion=temp_imp;
                            node_o_insertion=post_lkh_clusters[clust][cust];
                            node_d_insertion=0;

                        }
                    }
                }

                /********************************************************************************************/

                /****************************Estimation of Two-Opt**********************************************/

                //cout<<"--> Estimation of Two-Opt<--"<<endl;

                for(int cluster1{0};cluster1<post_lkh_clusters.size();cluster1++){

                    for(int cust1{0};cust1<post_lkh_clusters[cluster1].size();cust1++){

                        for(int cust2{0};cust2<post_lkh_clusters[cluster1].size();cust2++){

                            if(post_lkh_clusters[cluster1][cust1]!=post_lkh_clusters[cluster1][cust2]){

                                temp_imp=TwoOptEstimator(post_lkh_clusters[cluster1][cust1],
                                                         post_lkh_clusters[cluster1][cust2],
                                                         post_lkh_clusters,Distancias,
                                                         mydata);

                                if(temp_imp<improvement && tabu_list.check(post_lkh_clusters[cluster1][cust1],
                                                                           post_lkh_clusters[cluster1][cust2])==false){

                                    improvement=temp_imp;
                                    chosen_operator="Two-Opt";
                                    node_o=post_lkh_clusters[cluster1][cust1];
                                    node_d=post_lkh_clusters[cluster1][cust2];
                                    if(temp_imp>0){temp_increment=1;}

                                }

                                if(temp_imp<best_two_opt){

                                    best_two_opt=temp_imp;
                                    node_o_two_opt=post_lkh_clusters[cluster1][cust1];
                                    node_d_two_opt=post_lkh_clusters[cluster1][cust2];

                                }
                            }
                        }
                    }
                }


                /***********************************************************************************************/

            }else{

                /****************Estimation of Swap Inter-route*********************************************/

                //cout<<"--> Estimation of Swap Inter-Route <--"<<endl;
                dist_t temp_imp=0.0;

                for(int cluster1{0};cluster1<post_lkh_clusters.size();cluster1++){

                    for(int cust1{0};cust1<post_lkh_clusters[cluster1].size();cust1++){

                        for(int cluster2{0};cluster2<post_lkh_clusters.size();cluster2++){

                            for(int cust2{0};cust2<post_lkh_clusters[cluster2].size();cust2++){

                                if(post_lkh_clusters[cluster1][cust1]!=post_lkh_clusters[cluster2][cust2] &&
                                   cluster1!=cluster2){

                                    temp_imp=SwapInterRouteEstimator(post_lkh_clusters[cluster1][cust1],
                                                                         post_lkh_clusters[cluster2][cust2],
                                                                         post_lkh_clusters, Distancias,
                                                                         optimal_rec_types, mydata,temp_excess,
                                                                         excess_per_cluster, C_Data);

                                    if(temp_imp<improvement && tabu_list.check(post_lkh_clusters[cluster1][cust1],
                                                                               post_lkh_clusters[cluster2][cust2])==false){

                                        improvement=temp_imp;
                                        chosen_operator="Swap Inter-Route";
                                        node_o=post_lkh_clusters[cluster1][cust1];
                                        node_d=post_lkh_clusters[cluster2][cust2];
                                        if(temp_imp>0){temp_increment=1;}

                                    }

                                    if(temp_imp<best_swapinter){

                                        best_swapinter=temp_imp;
                                        node_o_swapinter=post_lkh_clusters[cluster1][cust1];
                                        node_d_swapinter=post_lkh_clusters[cluster2][cust2];

                                    }
                                }
                            }
                        }
                    }
                }

                /***********************************************************************************************/

                /*****************************Estimation of Insertion-to-route**********************************/

                //cout<<"--> Estimation of Insertion-to-route<--"<<endl;

                for(int cluster1{0};cluster1<post_lkh_clusters.size();cluster1++){

                    for(int cust1{0};cust1<post_lkh_clusters[cluster1].size();cust1++){

                        for(int cluster2{cluster1+1};cluster2<post_lkh_clusters.size();cluster2++){

                            for(int cust2{0};cust2<post_lkh_clusters[cluster2].size();cust2++){

                                if(post_lkh_clusters[cluster1][cust1]!=post_lkh_clusters[cluster2][cust2] &&
                                   cluster1!=cluster2){

                                    temp_imp=InsertionRouteEstimator(post_lkh_clusters[cluster1][cust1],
                                                                     post_lkh_clusters[cluster2][cust2],
                                                                     post_lkh_clusters, Distancias,
                                                                     optimal_rec_types, mydata,temp_excess,
                                                                     excess_per_cluster, C_Data);

                                    if(temp_imp<improvement && tabu_list.check(post_lkh_clusters[cluster1][cust1],
                                                                               post_lkh_clusters[cluster2][cust2])==false){

                                        improvement=temp_imp;
                                        chosen_operator="Insertion-to-route";
                                        node_o=post_lkh_clusters[cluster1][cust1];
                                        node_d=post_lkh_clusters[cluster2][cust2];
                                        if(temp_imp>0){temp_increment=1;}

                                    }

                                    if(temp_imp<best_insertion_route){

                                        best_insertion_route=temp_imp;
                                        node_o_insertion_route=post_lkh_clusters[cluster1][cust1];
                                        node_d_insertion_route=post_lkh_clusters[cluster2][cust2];

                                    }
                                }
                            }
                        }
                    }
                }

                /************************************************************************************************/
            }


//            /**********Estimation of Insertion****************************/
//
//            dist_t best_insertion=9999.0;
//            int node_o_insertion=0;
//            int node_d_insertion=0;
//
//            cout<<"--> Estimation of Insertion <--"<<endl;
//
//            dist_t temp_imp=0.0;
//            for(int clust{0};clust<post_lkh_clusters.size();clust++){
//                for(int cust{0};cust<post_lkh_clusters[clust].size();cust++){
//                    if(cust==0){
//                        temp_imp=InsertionEstimator(Distancias,0,post_lkh_clusters[clust][cust],
//                                                    post_lkh_clusters[clust][cust+1],mydata);
//                    }
//                    else if(cust==post_lkh_clusters[clust].size()-1){
//                        temp_imp=InsertionEstimator(Distancias,post_lkh_clusters[clust][cust-1],post_lkh_clusters[clust][cust],
//                                                    0,mydata);
//                    }else{
//                        temp_imp=InsertionEstimator(Distancias,post_lkh_clusters[clust][cust-1],post_lkh_clusters[clust][cust],
//                                                    post_lkh_clusters[clust][cust+1],mydata);
//                    }
//                    if(temp_imp<improvement && tabu_list.check(post_lkh_clusters[clust][cust],0)==false){
//                        chosen_operator="Insertion";
//                        improvement=temp_imp;
//                        node_o=post_lkh_clusters[clust][cust];
//                        node_d=0;
//                    }
//                    if(temp_imp<best_insertion){
//                        best_insertion=temp_imp;
//                        node_o_insertion=post_lkh_clusters[clust][cust];
//                    }
//                }
//            }
//
//            cout<<"ESTIMATION OF INSERTION: "<<best_insertion<<" --> "<<node_o_insertion<<endl;
//
//            /********************************************************************************************/
//
//            /****************************Estimation of Two-Opt**********************************************/
//
//            dist_t best_two_opt = 9999.0;
//            int node_o_two_opt = 0;
//            int node_d_two_opt = 0;
//
//            cout<<"--> Estimation of Two-Opt<--"<<endl;
//
//            //dist_t temp_imp=0.0;
//            for(int cluster1{0};cluster1<post_lkh_clusters.size();cluster1++){
//                for(int cust1{0};cust1<post_lkh_clusters[cluster1].size();cust1++){
//                    for(int cust2{0};cust2<post_lkh_clusters[cluster1].size();cust2++){
//                        if(post_lkh_clusters[cluster1][cust1]!=post_lkh_clusters[cluster1][cust2]){
//
//                            temp_imp=TwoOptEstimator(post_lkh_clusters[cluster1][cust1],
//                                                     post_lkh_clusters[cluster1][cust2],
//                                                     post_lkh_clusters,Distancias,
//                                                     mydata);
//
//
//                            if(temp_imp<improvement && tabu_list.check(post_lkh_clusters[cluster1][cust1],
//                                                                       post_lkh_clusters[cluster1][cust2])==false){
//                                improvement=temp_imp;
//                                chosen_operator="Two-Opt";
//                                node_o=post_lkh_clusters[cluster1][cust1];
//                                node_d=post_lkh_clusters[cluster1][cust2];
//                            }
//                            if(temp_imp<best_two_opt){
//                                best_two_opt=temp_imp;
//                                node_o_two_opt=post_lkh_clusters[cluster1][cust1];
//                                node_d_two_opt=post_lkh_clusters[cluster1][cust2];
//                            }
//                        }
//                    }
//                }
//            }
//
//            cout<<"ESTIMATION OF TWO-OPT: "<<best_two_opt<<" --> "<<node_o_two_opt<<", "<<node_d_two_opt<<endl;
//
//
//            /***********************************************************************************************/
//
//            /****************Estimation of Swap Inter-route*********************************************/
//
//            dist_t best_swapinter=9999.0;
//            int node_o_swapinter=0;
//            int node_d_swapinter=0;
//
//            cout<<"--> Estimation of Swap Inter-Route <--"<<endl;
//
//            //dist_t temp_imp=0.0;
//            for(int cluster1{0};cluster1<post_lkh_clusters.size();cluster1++){
//                for(int cust1{0};cust1<post_lkh_clusters[cluster1].size();cust1++){
//                    for(int cluster2{0};cluster2<post_lkh_clusters.size();cluster2++){
//                        for(int cust2{0};cust2<post_lkh_clusters[cluster2].size();cust2++){
//                            if(post_lkh_clusters[cluster1][cust1]!=post_lkh_clusters[cluster2][cust2] &&
//                               cluster1!=cluster2){
//
//                                temp_imp=SwapInterRouteEstimator(post_lkh_clusters[cluster1][cust1],
//                                                                     post_lkh_clusters[cluster2][cust2],
//                                                                     post_lkh_clusters, Distancias,
//                                                                     optimal_rec_types, mydata);
//
//                                if(temp_imp<improvement && tabu_list.check(post_lkh_clusters[cluster1][cust1],
//                                                                           post_lkh_clusters[cluster2][cust2])==false){
//                                    improvement=temp_imp;
//                                    chosen_operator="Swap Inter-Route";
//                                    node_o=post_lkh_clusters[cluster1][cust1];
//                                    node_d=post_lkh_clusters[cluster2][cust2];
//                                }
//                                if(temp_imp<best_swapinter){
//                                    best_swapinter=temp_imp;
//                                    node_o_swapinter=post_lkh_clusters[cluster1][cust1];
//                                    node_d_swapinter=post_lkh_clusters[cluster2][cust2];
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//
//            cout<<"ESTIMATION SWAP INTER-ROUTE: "<<best_swapinter<<" --> "<<node_o_swapinter<<", "<<node_d_swapinter<<endl;
//
//            /***********************************************************************************************/
////
////            /*****************************Estimation of Insertion-to-route**********************************/
////
////            dist_t best_insertion_route = 9999.0;
////            int node_o_insertion_route = 0;
////            int node_d_insertion_route = 0;
////
////            cout<<"--> Estimation of Insertion-to-route<--"<<endl;
////
////            //dist_t temp_imp=0.0;
////            for(int cluster1{0};cluster1<post_lkh_clusters.size();cluster1++){
////                for(int cust1{0};cust1<post_lkh_clusters[cluster1].size();cust1++){
////                    for(int cluster2{0};cluster2<post_lkh_clusters.size();cluster2++){
////                        for(int cust2{0};cust2<post_lkh_clusters[cluster2].size();cust2++){
////                            if(post_lkh_clusters[cluster1][cust1]!=post_lkh_clusters[cluster2][cust2] &&
////                               cluster1!=cluster2){
////
////                                temp_imp=InsertionRouteEstimator(post_lkh_clusters[cluster1][cust1],
////                                                                 post_lkh_clusters[cluster2][cust2],
////                                                                 post_lkh_clusters, Distancias,
////                                                                 optimal_rec_types, mydata);
////
////                                if(temp_imp<improvement && tabu_list.check(post_lkh_clusters[cluster1][cust1],
////                                                                           post_lkh_clusters[cluster2][cust2])==false){
////                                    improvement=temp_imp;
////                                    chosen_operator="Insertion-to-route";
////                                    node_o=post_lkh_clusters[cluster1][cust1];
////                                    node_d=post_lkh_clusters[cluster2][cust2];
////                                }
////                                if(temp_imp<best_insertion_route){
////                                    best_insertion_route=temp_imp;
////                                    node_o_insertion_route=post_lkh_clusters[cluster1][cust1];
////                                    node_d_insertion_route=post_lkh_clusters[cluster2][cust2];
////                                }
////                            }
////                        }
////                    }
////                }
////            }
////
////            cout<<"ESTIMATION OF INSERTION-TO-ROUTE: "<<best_insertion_route<<" --> "<<node_o_insertion_route;
////            cout<<", "<<node_d_insertion_route<<endl;
////
////            /************************************************************************************************/

            /*****************************Perturbation******************************************************/
            /*
            if(it==it_perturbation){

                int random_operator = rand() % 4 +1;
                cout<<"RANDOM OPERATOR "<<random_operator<<endl;

                if(random_operator==1){

                    chosen_operator="Insertion";
                    node_o=node_o_insertion;
                    node_d=node_d_insertion;

                }else if(random_operator==2){

                    chosen_operator="Swap Inter-Route";
                    node_o=node_o_swapinter;
                    node_d=node_d_swapinter;

                }else if(random_operator==3){

                    chosen_operator="Insertion-to-route";
                    node_o=node_o_insertion_route;
                    node_d=node_d_insertion_route;

                }else if(random_operator==4){

                    chosen_operator="Two-Opt";
                    node_o=node_o_two_opt;
                    node_d=node_d_two_opt;

                }
                it_perturbation+=8;
            }*/

            /***********************************************************************************************/

            /***********************************************************************************************/

            //cout<<">>>>>>>>>>>>>>>>> "<<chosen_operator<<" <<<<<<<<<<<<<<<<<<<< "<<node_o<<", "<<node_d<<endl;

            if(chosen_operator=="Insertion"){

                selected_operators[0]+=1;

                //PrintMatrix(post_lkh_clusters,"[","]");

                ResultInsertion result_insertion2;

                result_insertion2.result=0.0;
                result_insertion2.solution=vector<vector<int>>();
                result_insertion2.reloc_cost=0.0;

                vector<int> info_node = ClusterOfCustomer(post_lkh_clusters,node_o);

                if(info_node[1]==0){

                    result_insertion2=Insertion(post_lkh_clusters,Distancias,
                                            0,post_lkh_clusters[info_node[0]][info_node[1]],
                                            post_lkh_clusters[info_node[0]][info_node[1]+1],
                                            mydata);


                }else if(info_node[1]==post_lkh_clusters[info_node[0]].size()-1){

                   result_insertion2=Insertion(post_lkh_clusters,Distancias,
                                            post_lkh_clusters[info_node[0]][info_node[1]-1],
                                            post_lkh_clusters[info_node[0]][info_node[1]],0,
                                            mydata);

                }else{

                    result_insertion2=Insertion(post_lkh_clusters,Distancias,
                                            post_lkh_clusters[info_node[0]][info_node[1]-1],
                                            post_lkh_clusters[info_node[0]][info_node[1]],
                                            post_lkh_clusters[info_node[0]][info_node[1]+1],
                                            mydata);

                }

                test[0]=test[0]+result_insertion2.result;
                post_lkh_clusters=result_insertion2.solution;
                temp_rcost+=result_insertion2.reloc_cost;
                optimal_rec_types=optimal_rec_types;
                temp_excess=temp_excess;
                excess_per_cluster=excess_per_cluster;



                tabu_list.add(node_o,node_d);


                //cout<<"RESULT: "<<result_insertion2.result<<endl;

                tracking_fo.push_back(test[0]);

                if(test[0]<bestSolution.objective_function){

                    bestSolution.solution=post_lkh_clusters;
                    bestSolution.objective_function=test[0];
                    bestSolution.reloc_cost=temp_rcost;
                    bestSolution.rec_types=optimal_rec_types;
                    bestSolution.excess=temp_excess;
                    bestSolution.excess_clusters=excess_per_cluster;

                }

            }else if(chosen_operator=="Swap Inter-Route"){

                selected_operators[2]+=1;

                //PrintMatrix(post_lkh_clusters,"[","]");

                ResultSwap result_swap2;

                result_swap2.result=0.0;
                result_swap2.solution=vector<vector<int>>();
                result_swap2.demand=list<vector<vector<int>>>();
                result_swap2.excess=0.0;

                result_swap2=SwapInterRoute(node_o,node_d,post_lkh_clusters,Distancias,
                                            optimal_rec_types,mydata,temp_excess,
                                            excess_per_cluster, C_Data);

                test[0]=test[0]+result_swap2.result;
                post_lkh_clusters=result_swap2.solution;
                optimal_rec_types=result_swap2.demand;
                temp_excess=result_swap2.excess;
                excess_per_cluster=result_swap2.excess_clusters;


                tabu_list.add(node_o,node_d);

                //cout<<"RESULT: "<<result_swap2.result<<endl;

                tracking_fo.push_back(test[0]);

                if(test[0]<bestSolution.objective_function){

                    bestSolution.solution=post_lkh_clusters;
                    bestSolution.objective_function=test[0];
                    bestSolution.rec_types=optimal_rec_types;
                    bestSolution.excess=temp_excess;
                    bestSolution.reloc_cost=temp_rcost;
                    bestSolution.excess_clusters=excess_per_cluster;

                }

            }else if(chosen_operator=="Insertion-to-route"){

                selected_operators[3]+=1;

                //PrintMatrix(post_lkh_clusters,"[","]");

                ResultInsertionRoute result_insertion_route;

                result_insertion_route.result=0.0;
                result_insertion_route.solution=vector<vector<int>>();
                result_insertion_route.demand=list<vector<vector<int>>>();
                result_insertion_route.excess=0.0;
                result_insertion_route.excess_clusters=vector<dist_t>();

                result_insertion_route=InsertionRouteOpt(node_o,node_d,post_lkh_clusters,Distancias,
                                                         optimal_rec_types,mydata,temp_excess,
                                                         excess_per_cluster, C_Data);

                test[0]=test[0]+result_insertion_route.result;
                post_lkh_clusters=result_insertion_route.solution;
                optimal_rec_types=result_insertion_route.demand;
                temp_excess=result_insertion_route.excess;
                excess_per_cluster=result_insertion_route.excess_clusters;
                temp_rcost=temp_rcost;

                tabu_list.add(node_o,node_d);

                //cout<<"RESULT: "<<result_insertion_route.result<<endl;

                tracking_fo.push_back(test[0]);

                if(test[0]<bestSolution.objective_function){

                    bestSolution.solution=post_lkh_clusters;
                    bestSolution.objective_function=test[0];
                    bestSolution.rec_types=optimal_rec_types;
                    bestSolution.excess=temp_excess;
                    bestSolution.reloc_cost=temp_rcost;
                    bestSolution.excess_clusters=excess_per_cluster;

                }

            }else if(chosen_operator=="Two-Opt"){

                selected_operators[1]+=1;

                //PrintMatrix(post_lkh_clusters,"[","]");

                ResultTwoOpt result_two_opt;

                result_two_opt.result=0.0;
                result_two_opt.solution=vector<vector<int>>();

                result_two_opt=TwoOptOperator(node_o,node_d,post_lkh_clusters,Distancias,
                                              mydata);

                test[0]=test[0]+result_two_opt.result;
                post_lkh_clusters=result_two_opt.solution;
                optimal_rec_types=optimal_rec_types;
                temp_rcost=temp_rcost;
                temp_excess=temp_excess;
                excess_per_cluster=excess_per_cluster;

                tabu_list.add(node_o,node_d);

                //cout<<"RESULT: "<<result_two_opt.result<<endl;

                tracking_fo.push_back(test[0]);

                if(test[0]<bestSolution.objective_function){

                    bestSolution.solution=post_lkh_clusters;
                    bestSolution.objective_function=test[0];
                    bestSolution.reloc_cost=temp_rcost;
                    bestSolution.rec_types=optimal_rec_types;
                    bestSolution.excess=temp_excess;
                    bestSolution.excess_clusters=excess_per_cluster;

                }

            }else{

//                cout<<"Kein Operator"<<endl;
                selected_operators[4]+=1;
//                cout<<"Relocation = "<<best_insertion<<endl;
//                cout<<"Two-Opt = "<<best_two_opt<<endl;
//                cout<<"Swap = "<<best_swapinter<<endl;
//                cout<<"Insertion = "<<best_insertion_route<<endl;

            }

            int prob_random = rand() % (size_prob_excess)+1;

            int ref_prob_excess=0;

            float limit{0.0};

            if(prob_random<=distr_prob_excess[0].size()){

                limit = 0.25;

            }else if (prob_random<=distr_prob_excess[1].size()){

                limit=0.5;
                ref_prob_excess=1;

            }else{

                limit=0.75;
                ref_prob_excess=2;

            }

            float r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

            if(temp_excess>0.0 && r2<limit){

                int temp_chosen{0};

                for(int i{0};i<excess_per_cluster.size();i++){

                    if(excess_per_cluster[i]>0.0){

                        temp_chosen=i;
                        break;

                    }
                }


                int random_cust = rand()%(post_lkh_clusters[temp_chosen].size());


                OneRouteReult temp_one_route;

                temp_one_route=OneRouteOpt(post_lkh_clusters[temp_chosen][random_cust],Distancias,
                                           optimal_rec_types,post_lkh_clusters,excess_per_cluster,temp_excess,
                                           C_Data,mydata);

                if(temp_one_route.result<0.0){

                    distr_prob_excess[ref_prob_excess].push_back(1);

                }


                test[0]+=temp_one_route.result;
                post_lkh_clusters=temp_one_route.solution;
                optimal_rec_types=temp_one_route.demand;
                temp_excess=temp_one_route.excess;
                excess_per_cluster=temp_one_route.excess_clusters;

                selected_operators[5]+=1;

            }

            increments_fo+=temp_increment;

            if(increments_fo==int(0.1*max_iterations)){


                test[0]=bestSolution.objective_function;
                post_lkh_clusters=bestSolution.solution;
                optimal_rec_types=bestSolution.rec_types;
                temp_excess=bestSolution.excess;
                excess_per_cluster=bestSolution.excess_clusters;
                temp_rcost=bestSolution.reloc_cost;


                increments_fo=0;
            }


            //cout<<"BEST SOLUTION: "<<bestSolution.objective_function<<"<****************"<<endl;
            //cout<<"TEST[0]: "<<test[0]<<"<****************"<<endl;
        }


        for(int clust{0};clust<bestSolution.solution.size();clust++){

            if(bestSolution.solution[clust].size()==0){

                vector<vector<int>>::iterator iter = bestSolution.solution.begin();
                bestSolution.solution.erase(iter+clust);
                list<vector<vector<int>>>::iterator iter2 = bestSolution.rec_types.begin();
                advance(iter2,clust);
                bestSolution.rec_types.erase(iter2);

            }
        }


        cout<<"IMPROVEMENT PHASE F.O = "<<first_phase_fo<<endl;
        cout<<"IMPROVEMENT RELOC COST = "<<first_reloc_cost<<endl;
        cout<<"BEST OBJECTIVE FUNCTION = "<<bestSolution.objective_function<<endl;
        cout<<"EXCESS OF BEST SOLUTION = "<<bestSolution.excess<<endl;
        cout<<"RELOC COST OF BEST SOLUTION = "<<bestSolution.reloc_cost<<endl;
        cout<<PrintMatrix(bestSolution.solution,"[","]")<<endl;

        dist_t max_dist{0};

        for(int i{0};i<Distancias.size();i++){

            for(int j{0};j<Distancias[i].size();j++){

                if(Distancias[i][j]>max_dist){max_dist=Distancias[i][j];}

            }
        }

        int max_demand{0};

        for(int i{0};i<mydata.boxes.size();i++){

            int volume = mydata.boxes[i].heightBox*\
                        mydata.boxes[i].widthBox*\
                        mydata.boxes[i].lengthBox;

            if(volume>max_demand){max_demand=volume;}

        }

        cout<<"The max distance is "<<max_dist<<endl;
        cout<<"The max demand is "<<max_demand<<endl;

        for(int i{0};i<selected_operators.size();i++){

            cout<<selected_operators[i]<<" ";
        }

        cout<<endl;

        ofstream RelocFile("RelocFile.csv");
        RelocFile<<"Cluster,X,Y"<<endl;

        for(int cluster{0};cluster<bestSolution.solution.size();cluster++){

            for(int customer{0};customer<bestSolution.solution[cluster].size();customer++){

                RelocFile<<cluster<<","<<C_Data[bestSolution.solution[cluster][customer]-1].x\
                <<","<<C_Data[bestSolution.solution[cluster][customer]-1].y<<endl;

            }
        }

        RelocFile.close();

        ofstream TrackingFile("TrackingFile.csv");
        TrackingFile<<"F.O"<<endl;

        for(int point{0};point<tracking_fo.size();point++){

            TrackingFile<<tracking_fo[point]<<endl;

        }

        TrackingFile.close();

        clock_t clock_final=clock();
        double alg_time=double(clock_final-t_inicial)/CLOCKS_PER_SEC;

        cout<<"TIME ELAPSED = "<<alg_time<<endl;

        for(int i{0};i<distr_prob_excess.size();i++){

            cout<<distr_prob_excess[i].size()<<endl;

        }

        /*****************************Part to write instances***********************************************/

        /*
        string answer{""};
        cout<<"Do you want to write this relocation points in the current instance? [Y/N] ";
        cin>>answer;

        if(answer=="Y"){

            ofstream instance((testname + "/" + filename + ".txt").c_str(),std::ios::app);

            for(int i{0};i<mydata.relocation_points.size();i++){

                instance<<mydata.relocation_points[i].customerId\
                <<" "<<mydata.relocation_points[i].x\
                <<" "<<mydata.relocation_points[i].y<<endl;

            }

            instance.close();
        }*/

        /************************************************************************************************/

#endif
        cout<<"FINAL"<<endl;

		return 0;
}

