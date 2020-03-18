#ifndef cambioTipoDato_H
#define cambioTipoDato_H

#include <vector> 
#include <string> 
using namespace std;

/////////////////////////
//Listado de prototipos//
/////////////////////////

int convertirValorStringEntero(string);
float convertirValorStringFloat(string);
double convertirValorStringDouble(string);
string convertirValorFloatString(float);
void StringExplode(string, string , vector<string>* );

#endif
