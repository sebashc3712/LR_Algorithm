//Funciones para convertir variables de cierto tipo de datos
//Luis Miguel Escobar Falcón - Universidad Tecnológica de Pereira

//Librerías necesarias
#include <vector>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <sstream>
using namespace std;

//Función para convertir una variable de tipo string a double
int convertirValorStringEntero(string cadena){
	//Declaración de variables	
	int resultado;
	stringstream ss(cadena);
	ss >> resultado;		
	return resultado;
}

//Función para convertir una variable de tipo string a double
double convertirValorStringDouble(string cadena){
	//Declaración de variables	
	double resultado;
	stringstream ss(cadena);
	ss >> resultado;		
	return resultado;
}

//Función para convertir una variable de tipo string a float
float convertirValorStringFloat(string cadena){
	//Declaración de variables	
	float resultado;
	stringstream ss(cadena);
	ss >> resultado;		
	return resultado;
}

//Función para convertir una variable de tipo float a string

string convertirValorFloatString(float myFloat){
	//Declaración de variables
	ostringstream ss;
	ss << myFloat;
	string s(ss.str());
	return s;
}

//Función para dividir un string por algún criterio (separator)
void StringExplode(string str, string separator, vector<string>* results){
    int found;
    found = str.find_first_of(separator);
    while(found != string::npos){
        if(found > 0){
            results->push_back(str.substr(0,found));
        }
        str = str.substr(found+1);
        found = str.find_first_of(separator);
    }
    if(str.length() > 0){
        results->push_back(str);
    }
}


