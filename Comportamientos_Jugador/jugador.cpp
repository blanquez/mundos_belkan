#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>
/*
PUNTOS A REVISAR
- Algoritmo de busqueda para el reto
- Choque con los aldeanos
- Busqueda reactiva(metodo pulgarcito,
		 intentar guardar camino hecho, intentar evitar agua)
*/

// Este es el método principal que debe contener los 4 Comportamientos_Jugador
// que se piden en la práctica. Tiene como entrada la información de los
// sensores y devuelve la acción a realizar.
Action ComportamientoJugador::think(Sensores sensores) {
	Action sigAccion;
	switch(ultimaAccion){
		case actTURN_R: brujula = (brujula+1)%4; break;
		case actTURN_L: brujula = (brujula+3)%4; break;
		case actFORWARD:
			switch(brujula){
				case 0: fil--; break;
				case 1: col++; break;
				case 2: fil++; break;
				case 3: col--; break;
			}
		break;
	}

	fil_reac=fil;
	col_reac=col;

	if(sensores.nivel == 4){
		if(!tengo_coordenadas){
			estado aux;

			if(sensores.mensajeF != -1){
				fil = sensores.mensajeF;
				col = sensores.mensajeC;
				ultimaAccion = actIDLE;
				hayPlan = false;
				tengo_coordenadas = true;
				escribirMapa(sensores, mapaResultado);

				int diferenciaF = fil_reac - fil;
      			int diferenciaC = col_reac - col;
      			for (int i = 0; i < mapaResultado.size(); i++) {
       				for (int j = 0; j < mapaResultado[0].size(); j++) {
          				mapaResultado[i][j] = mapaReactivo[diferenciaF + i][diferenciaC + j];
        			}
      			}
			}
			else{
				escribirMapa(sensores, mapaReactivo);
				if(!hay_Plan_react){
					for(int i=0;i<16;i++) if(sensores.terreno[i]=='K'){
						actual.fila = fil;
						actual.columna = col;
						actual.orientacion = brujula;
						crearDestino(i,aux);
						hay_Plan_react=true;
						pathFinding(sensores.nivel, actual, aux, plan, mapaReactivo);
					}
				}

				if(hay_Plan_react && plan.size()>0){
					if(sensores.terreno[2]=='P' || sensores.terreno[2]=='D' || sensores.terreno[2]=='M'){
						hay_Plan_react=false;
						sigAccion=actTURN_R;
					}
					else{
						sigAccion = plan.front();
						plan.erase(plan.begin());
					}
				}
				else{
					if((sensores.terreno[12]=='P' || sensores.terreno[12]=='M' || sensores.terreno[12]=='D') && mapaPulgarcito[fil][col]!=9998){
						mapaPulgarcito[fil][col]=9998;
						switch(brujula){
							case 0: mapaPulgarcito[fil-3][col]=9999; break;
							case 1: mapaPulgarcito[fil][col+3]=9999; break;
							case 2: mapaPulgarcito[fil+3][col]=9999; break;
							case 3: mapaPulgarcito[fil][col-3]=9999; break;
						}
						switch(brujula){
							case 0: if(mapaPulgarcito[fil][col-1]<mapaPulgarcito[fil][col+1]) sigAccion=actTURN_L;
									else sigAccion=actTURN_R;
							break;
							case 1: if(mapaPulgarcito[fil-1][col]<mapaPulgarcito[fil+1][col]) sigAccion=actTURN_L;
									else sigAccion=actTURN_R;
							break;
							case 2: if(mapaPulgarcito[fil][col+1]<mapaPulgarcito[fil][col-1]) sigAccion=actTURN_L;
									else sigAccion=actTURN_R;
							break;
							case 3: if(mapaPulgarcito[fil+1][col]<mapaPulgarcito[fil-1][col]) sigAccion=actTURN_L;
									else sigAccion=actTURN_R;
							break;
						}
					}
					else if(sensores.terreno[2]=='P' || sensores.terreno[2]=='M' || sensores.terreno[2]=='D'){
						switch(brujula){
							case 0: mapaPulgarcito[fil-1][col]=9999; break;
							case 1: mapaPulgarcito[fil][col+1]=9999; break;
							case 2: mapaPulgarcito[fil+1][col]=9999; break;
							case 3: mapaPulgarcito[fil][col-1]=9999; break;
						}
						switch(brujula){
							case 0: if(mapaPulgarcito[fil][col-1]<mapaPulgarcito[fil][col+1]) sigAccion=actTURN_L;
									else sigAccion=actTURN_R;
							break;
							case 1: if(mapaPulgarcito[fil-1][col]<mapaPulgarcito[fil+1][col]) sigAccion=actTURN_L;
									else sigAccion=actTURN_R;
							break;
							case 2: if(mapaPulgarcito[fil][col+1]<mapaPulgarcito[fil][col-1]) sigAccion=actTURN_L;
									else sigAccion=actTURN_R;
							break;
							case 3: if(mapaPulgarcito[fil+1][col]<mapaPulgarcito[fil-1][col]) sigAccion=actTURN_L;
									else sigAccion=actTURN_R;
							break;
						}
					}
					else if(sensores.superficie[2]=='a') sigAccion = actIDLE;
					else{
						switch(brujula){
							case 0:
								if(mapaPulgarcito[fil-1][col]==9998){
									mapaPulgarcito[fil-1][col]=valor_pulg;
									valor_pulg++;
								}
								if(mapaPulgarcito[fil-1][col]<=mapaPulgarcito[fil][col-1] && mapaPulgarcito[fil-1][col]<=mapaPulgarcito[fil][col+1]){
									sigAccion=actFORWARD;
									mapaPulgarcito[fil][col]=valor_pulg;
									valor_pulg++;
								}
								else if(mapaPulgarcito[fil][col-1]<mapaPulgarcito[fil][col+1]) sigAccion=actTURN_L;
								else sigAccion=actTURN_R;
							break;
							case 1:
								if(mapaPulgarcito[fil][col+1]==9998){
									mapaPulgarcito[fil][col+1]=valor_pulg;
									valor_pulg++;
								}
								if(mapaPulgarcito[fil][col+1]<=mapaPulgarcito[fil-1][col] && mapaPulgarcito[fil][col+1]<=mapaPulgarcito[fil+1][col]){
									sigAccion=actFORWARD;
									mapaPulgarcito[fil][col]=valor_pulg;
									valor_pulg++;
								}
								else if(mapaPulgarcito[fil-1][col]<mapaPulgarcito[fil+1][col]) sigAccion=actTURN_L;
								else sigAccion=actTURN_R;
							break;
							case 2:
								if(mapaPulgarcito[fil+1][col]==9998){
									mapaPulgarcito[fil+1][col]=valor_pulg;
									valor_pulg++;
								}
								if(mapaPulgarcito[fil+1][col]<=mapaPulgarcito[fil][col+1] && mapaPulgarcito[fil+1][col]<=mapaPulgarcito[fil][col-1]){
									sigAccion=actFORWARD;
									mapaPulgarcito[fil][col]=valor_pulg;
									valor_pulg++;
								}
								else if(mapaPulgarcito[fil][col+1]<mapaPulgarcito[fil][col-1]) sigAccion=actTURN_L;
								else sigAccion=actTURN_R;
							break;
							case 3:
								if(mapaPulgarcito[fil][col-1]==9998){
									mapaPulgarcito[fil][col-1]=valor_pulg;
									valor_pulg++;
								}
								if(mapaPulgarcito[fil][col-1]<=mapaPulgarcito[fil+1][col] && mapaPulgarcito[fil][col-1]<=mapaPulgarcito[fil-1][col]){
									sigAccion=actFORWARD;
									mapaPulgarcito[fil][col]=valor_pulg;
									valor_pulg++;
								}
								else if(mapaPulgarcito[fil+1][col]<mapaPulgarcito[fil-1][col]) sigAccion=actTURN_L;
								else sigAccion=actTURN_R;
							break;
						}
					}
				}
			}
		}
		else{
			if(plan.front() == actFORWARD && (sensores.terreno[2]=='P' || sensores.terreno[2]=='D' || sensores.terreno[2]=='M')) hayPlan = false;

			if(sensores.destinoF != destino.fila || sensores.destinoC != destino.columna){
				destino.fila = sensores.destinoF;
				destino.columna = sensores.destinoC;
				hayPlan = false;
			}

			if(!hayPlan){
				actual.fila = fil;
				actual.columna = col;
				actual.orientacion = brujula;
				hayPlan = pathFinding(sensores.nivel, actual, destino, plan, mapaResultado);
			}

			if(hayPlan && plan.size()>0){
				if(sensores.superficie[2]=='a') sigAccion = actIDLE;
				else{
					sigAccion = plan.front();
					plan.erase(plan.begin());
				}
			}
			else sigAccion = actIDLE;

			if(!enAgua && sigAccion==actFORWARD && hayAgua(sensores)){
				actual.fila = fil;
				actual.columna = col;
				actual.orientacion = brujula;
				enAgua=true;
				hayPlan = pathFinding(sensores.nivel, actual, destino, plan, mapaResultado);
				sigAccion=actIDLE;
			}
			else if(!hayAgua(sensores)) enAgua=false;

			escribirMapa(sensores, mapaResultado);
		}
		ultimaAccion = sigAccion;
		return sigAccion;
	}	

	else{
		estoy_bien_situado = (fil==sensores.mensajeF && col==sensores.mensajeC);
		if(sensores.mensajeF != -1 && !estoy_bien_situado){
			fil = sensores.mensajeF;
			col = sensores.mensajeC;
			ultimaAccion = actIDLE;
			hayPlan = false;
		}

		if(sensores.destinoF != destino.fila || sensores.destinoC != destino.columna){
			destino.fila = sensores.destinoF;
			destino.columna = sensores.destinoC;
			hayPlan = false;
		}

		if(!hayPlan){
			actual.fila = fil;
			actual.columna = col;
			actual.orientacion = brujula;
			hayPlan = pathFinding(sensores.nivel, actual, destino, plan, mapaResultado);
		}

		if(hayPlan && plan.size()>0){
			sigAccion = plan.front();
			plan.erase(plan.begin());
		}
		else{
			if(sensores.terreno[2]=='P' || sensores.terreno[2]=='D' || sensores.terreno[2]=='M' || sensores.superficie[2]=='a') sigAccion = actTURN_R;
			else sigAccion = actFORWARD;
		}

		ultimaAccion = sigAccion;
		return sigAccion;
	}
}

bool ComportamientoJugador::hayAgua(Sensores sensores){
	return sensores.terreno[2]=='A' && sensores.terreno[6]=='A' && sensores.terreno[12]=='A';
}

void ComportamientoJugador::crearDestino(int val, estado &dest){
	switch (val){
	case 1:
		switch(brujula){
			case 0: dest.fila=fil-1; dest.columna=col-1; break;
			case 1: dest.fila=fil-1; dest.columna=col+1; break;
			case 2: dest.fila=fil+1; dest.columna=col+1; break;
			case 3: dest.fila=fil+1; dest.columna=col-1; break;
		}
	break;
	case 2:
		switch(brujula){
			case 0: dest.fila=fil-1; dest.columna=col; break;
			case 1: dest.fila=fil; dest.columna=col+1; break;
			case 2: dest.fila=fil+1; dest.columna=col; break;
			case 3: dest.fila=fil; dest.columna=col-1; break;
		}
	break;
	case 3:
		switch(brujula){
			case 0: dest.fila=fil-1; dest.columna=col+1; break;
			case 1: dest.fila=fil+1; dest.columna=col+1; break;
			case 2: dest.fila=fil+1; dest.columna=col-1; break;
			case 3: dest.fila=fil-1; dest.columna=col-1; break;
		}
	break;
	case 4:
		switch(brujula){
			case 0: dest.fila=fil-2; dest.columna=col-2; break;
			case 1: dest.fila=fil-2; dest.columna=col+2; break;
			case 2: dest.fila=fil+2; dest.columna=col+2; break;
			case 3: dest.fila=fil+2; dest.columna=col-2; break;
		}
	break;
	case 5:
		switch(brujula){
			case 0: dest.fila=fil-2; dest.columna=col-1; break;
			case 1: dest.fila=fil-1; dest.columna=col+2; break;
			case 2: dest.fila=fil+2; dest.columna=col+1; break;
			case 3: dest.fila=fil+1; dest.columna=col-2; break;
		}
	break;
	case 6:
		switch(brujula){
			case 0: dest.fila=fil-2; dest.columna=col; break;
			case 1: dest.fila=fil; dest.columna=col+2; break;
			case 2: dest.fila=fil+2; dest.columna=col; break;
			case 3: dest.fila=fil; dest.columna=col-2; break;
		}
	break;
	case 7:
		switch(brujula){
			case 0: dest.fila=fil-2; dest.columna=col+1; break;
			case 1: dest.fila=fil+1; dest.columna=col+2; break;
			case 2: dest.fila=fil+2; dest.columna=col-1; break;
			case 3: dest.fila=fil-1; dest.columna=col-2; break;
		}
	break;
	case 8:
		switch(brujula){
			case 0: dest.fila=fil-2; dest.columna=col+2; break;
			case 1: dest.fila=fil+2; dest.columna=col+2; break;
			case 2: dest.fila=fil+2; dest.columna=col-2; break;
			case 3: dest.fila=fil-2; dest.columna=col-2; break;
		}
	break;
	case 9:
		switch(brujula){
			case 0: dest.fila=fil-3; dest.columna=col-3; break;
			case 1: dest.fila=fil-3; dest.columna=col+3; break;
			case 2: dest.fila=fil+3; dest.columna=col+3; break;
			case 3: dest.fila=fil+3; dest.columna=col-3; break;
		}
	break;
	case 10:
		switch(brujula){
			case 0: dest.fila=fil-3; dest.columna=col-2; break;
			case 1: dest.fila=fil-2; dest.columna=col+3; break;
			case 2: dest.fila=fil+3; dest.columna=col+2; break;
			case 3: dest.fila=fil+2; dest.columna=col-3; break;
		}
	break;
	case 11:
		switch(brujula){
			case 0: dest.fila=fil-3; dest.columna=col-1; break;
			case 1: dest.fila=fil-1; dest.columna=col+3; break;
			case 2: dest.fila=fil+3; dest.columna=col+1; break;
			case 3: dest.fila=fil+1; dest.columna=col-3; break;
		}
	break;
	case 12:
		switch(brujula){
			case 0: dest.fila=fil-3; dest.columna=col; break;
			case 1: dest.fila=fil; dest.columna=col+3; break;
			case 2: dest.fila=fil+3; dest.columna=col; break;
			case 3: dest.fila=fil; dest.columna=col-3; break;
		}
	break;
	case 13:
		switch(brujula){
			case 0: dest.fila=fil-3; dest.columna=col+1; break;
			case 1: dest.fila=fil+1; dest.columna=col+3; break;
			case 2: dest.fila=fil+3; dest.columna=col-1; break;
			case 3: dest.fila=fil-1; dest.columna=col-3; break;
		}
	break;
	case 14:
		switch(brujula){
			case 0: dest.fila=fil-3; dest.columna=col+2; break;
			case 1: dest.fila=fil+2; dest.columna=col+3; break;
			case 2: dest.fila=fil+3; dest.columna=col-2; break;
			case 3: dest.fila=fil-2; dest.columna=col-3; break;
		}
	break;
	case 15:
		switch(brujula){
			case 0: dest.fila=fil-3; dest.columna=col+3; break;
			case 1: dest.fila=fil+3; dest.columna=col+3; break;
			case 2: dest.fila=fil+3; dest.columna=col-3; break;
			case 3: dest.fila=fil-3; dest.columna=col-3; break;
		}
	break;
	}
}

void ComportamientoJugador::escribirMapa(Sensores sensores, std::vector< std::vector< unsigned char> > &mapa){
	mapa[fil][col] = sensores.terreno[0];
	if(brujula==0){
		mapa[fil-1][col-1] = sensores.terreno[1];
		mapa[fil-1][col] = sensores.terreno[2];
		mapa[fil-1][col+1] = sensores.terreno[3];
		mapa[fil-2][col-2] = sensores.terreno[4];
		mapa[fil-2][col-1] = sensores.terreno[5];
		mapa[fil-2][col] = sensores.terreno[6];
		mapa[fil-2][col+1] = sensores.terreno[7];
		mapa[fil-2][col+2] = sensores.terreno[8];
		mapa[fil-3][col-3] = sensores.terreno[9];
		mapa[fil-3][col-2] = sensores.terreno[10];
		mapa[fil-3][col-1] = sensores.terreno[11];
		mapa[fil-3][col] = sensores.terreno[12];
		mapa[fil-3][col+1] = sensores.terreno[13];
		mapa[fil-3][col+2] = sensores.terreno[14];
		mapa[fil-3][col+3] = sensores.terreno[15];
	}
	else if(brujula==1){
		mapa[fil-1][col+1] = sensores.terreno[1];
		mapa[fil][col+1] = sensores.terreno[2];
		mapa[fil+1][col+1] = sensores.terreno[3];
		mapa[fil-2][col+2] = sensores.terreno[4];
		mapa[fil-1][col+2] = sensores.terreno[5];
		mapa[fil][col+2] = sensores.terreno[6];
		mapa[fil+1][col+2] = sensores.terreno[7];
		mapa[fil+2][col+2] = sensores.terreno[8];
		mapa[fil-3][col+3] = sensores.terreno[9];
		mapa[fil-2][col+3] = sensores.terreno[10];
		mapa[fil-1][col+3] = sensores.terreno[11];
		mapa[fil][col+3] = sensores.terreno[12];
		mapa[fil+1][col+3] = sensores.terreno[13];
		mapa[fil+2][col+3] = sensores.terreno[14];
		mapa[fil+3][col+3] = sensores.terreno[15];
	}
	else if(brujula==2){
		mapa[fil+1][col+1] = sensores.terreno[1];
		mapa[fil+1][col] = sensores.terreno[2];
		mapa[fil+1][col-1] = sensores.terreno[3];
		mapa[fil+2][col+2] = sensores.terreno[4];
		mapa[fil+2][col+1] = sensores.terreno[5];
		mapa[fil+2][col] = sensores.terreno[6];
		mapa[fil+2][col-1] = sensores.terreno[7];
		mapa[fil+2][col-2] = sensores.terreno[8];
		mapa[fil+3][col+3] = sensores.terreno[9];
		mapa[fil+3][col+2] = sensores.terreno[10];
		mapa[fil+3][col+1] = sensores.terreno[11];
		mapa[fil+3][col] = sensores.terreno[12];
		mapa[fil+3][col-1] = sensores.terreno[13];
		mapa[fil+3][col-2] = sensores.terreno[14];
		mapa[fil+3][col-3] = sensores.terreno[15];
	}
	else{
		mapa[fil+1][col-1] = sensores.terreno[1];
		mapa[fil][col-1] = sensores.terreno[2];
		mapa[fil-1][col-1] = sensores.terreno[3];
		mapa[fil+2][col-2] = sensores.terreno[4];
		mapa[fil+1][col-2] = sensores.terreno[5];
		mapa[fil][col-2] = sensores.terreno[6];
		mapa[fil-1][col-2] = sensores.terreno[7];
		mapa[fil-2][col-2] = sensores.terreno[8];
		mapa[fil+3][col-3] = sensores.terreno[9];
		mapa[fil+2][col-3] = sensores.terreno[10];
		mapa[fil+1][col-3] = sensores.terreno[11];
		mapa[fil][col-3] = sensores.terreno[12];
		mapa[fil-1][col-3] = sensores.terreno[13];
		mapa[fil-2][col-3] = sensores.terreno[14];
		mapa[fil-3][col-3] = sensores.terreno[15];
	}
}


// Llama al algoritmo de busqueda que se usará en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding (int level, const estado &origen, const estado &destino, list<Action> &plan, std::vector< std::vector< unsigned char> > &mapa){
	switch (level){
		case 1: cout << "Busqueda en profundad\n";
			      return pathFinding_Profundidad(origen,destino,plan);
						break;
		case 2: cout << "Busqueda en Anchura\n";
			      return pathFinding_Anchura(origen,destino,plan);
						break;
		case 3: cout << "Busqueda Costo Uniforme\n";
				  return pathFinding_Coste_Uniforme(origen,destino,plan,mapaResultado);
						break;
		case 4: cout << "Busqueda para el reto\n";
				  return pathFinding_Coste_Uniforme(origen,destino,plan,mapa);
						break;
	}
	cout << "Comportamiento sin implementar\n";
	return false;
}


//---------------------- Implementación de la busqueda en profundidad ---------------------------

// Dado el código en carácter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla){
	if (casilla=='P' or casilla=='M' or casilla =='D')
		return true;
	else
	  return false;
}


// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st, std::vector< std::vector< unsigned char> > &mapa){
	int fil=st.fila, col=st.columna;

  // calculo cual es la casilla de delante del agente
	switch (st.orientacion) {
		case 0: fil--; break;
		case 1: col++; break;
		case 2: fil++; break;
		case 3: col--; break;
	}

	// Compruebo que no me salgo fuera del rango del mapa
	if (fil<0 or fil>=mapa.size()) return true;
	if (col<0 or col>=mapa[0].size()) return true;

	// Miro si en esa casilla hay un obstaculo infranqueable
	if (!EsObstaculo(mapa[fil][col])){
		// No hay obstaculo, actualizo el parámetro st poniendo la casilla de delante.
    st.fila = fil;
		st.columna = col;
		return false;
	}
	else{
	  return true;
	}
}




struct nodo{
	estado st;
	list<Action> secuencia;
};

struct ComparaEstados{
	bool operator()(const estado &a, const estado &n) const{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
	      (a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion))
			return true;
		else
			return false;
	}
};


// Implementación de la búsqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	stack<nodo> pila;											// Lista de Abiertos

  nodo current;
	current.st = origen;
	current.secuencia.empty();

	pila.push(current);

  while (!pila.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		pila.pop();
		generados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			pila.push(hijoTurnR);
		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			pila.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st, mapaResultado)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				pila.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la pila
		if (!pila.empty()){
			current = pila.top();
		}
	}

  cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}


	return false;
}

// Implementación de la búsqueda en anchura.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	queue<nodo> pila;					// Lista de Abiertos
	set<estado,ComparaEstados> abiertos;

  nodo current;
	current.st = origen;
	current.secuencia.empty();

	pila.push(current);

  while (!pila.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		pila.pop();
		generados.insert(current.st);
		abiertos.erase(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end() && abiertos.find(hijoTurnR.st) == abiertos.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			pila.push(hijoTurnR);
			abiertos.insert(hijoTurnR.st);
		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end() && abiertos.find(hijoTurnL.st) == abiertos.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			pila.push(hijoTurnL);
			abiertos.insert(hijoTurnL.st);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st, mapaResultado)){
			if (generados.find(hijoForward.st) == generados.end() && abiertos.find(hijoForward.st) == abiertos.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				pila.push(hijoForward);
				abiertos.insert(hijoTurnL.st);
			}
		}

		// Tomo el siguiente valor de la pila
		if (!pila.empty()){
			current = pila.front();
		}
	}

  cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}


	return false;
}

int ComportamientoJugador::calcularCoste(estado casilla, std::vector< std::vector< unsigned char> > &mapa){
	int coste = casilla.coste;
	int film = casilla.fila;
	int colm = casilla.columna;
	unsigned char valor = mapa[film][colm];
	switch(valor){
		case 'A':
			coste += 10;
			break;
		case 'B': 
			coste += 5;
			break;
		case 'T': 
			coste += 2;
			break;
		case '?':
			coste += 4;
			break;
		default:
			coste++;
			break;
	}
	return coste;
}

struct mycomp{
		bool operator() (const nodo& lhs, const nodo& rhs){
			return lhs.st.coste > rhs.st.coste;
		}
	};

// Implementación de la búsqueda con coste uniforme.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Coste_Uniforme(const estado &origen, const estado &destino, list<Action> &plan, std::vector< std::vector< unsigned char> > &mapa) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	priority_queue<nodo,vector<nodo>,mycomp> pila;	// Lista de Abiertos
	set<estado,ComparaEstados> abiertos;

  nodo current;
	current.st = origen;
	current.secuencia.empty();
	current.st.coste = 0;

	pila.push(current);

  while (!pila.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		pila.pop();
		generados.insert(current.st);
		abiertos.erase(current.st);
		//cout << "Coste: " << current.st.coste << endl;

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end() && abiertos.find(hijoTurnR.st) == abiertos.end()){
			hijoTurnR.st.coste = calcularCoste(hijoTurnR.st, mapa);
			hijoTurnR.secuencia.push_back(actTURN_R);
			pila.push(hijoTurnR);
			abiertos.insert(hijoTurnR.st);
		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end() && abiertos.find(hijoTurnL.st) == abiertos.end()){
			hijoTurnL.st.coste = calcularCoste(hijoTurnL.st, mapa);
			hijoTurnL.secuencia.push_back(actTURN_L);
			pila.push(hijoTurnL);
			abiertos.insert(hijoTurnL.st);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st, mapa)){
			if (generados.find(hijoForward.st) == generados.end() && abiertos.find(hijoForward.st) == abiertos.end()){
				hijoForward.st.coste = calcularCoste(hijoForward.st, mapa);
				hijoForward.secuencia.push_back(actFORWARD);
				pila.push(hijoForward);
				abiertos.insert(hijoForward.st);
			}
		}

		// Tomo el siguiente valor de la pila
		if (!pila.empty()){
			current = pila.top();
		}
	}

  	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		if(mapa == mapaResultado) VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}


	return false;
}


// Sacar por la términal la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan) {
	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			cout << "A ";
		}
		else if (*it == actTURN_R){
			cout << "D ";
		}
		else if (*it == actTURN_L){
			cout << "I ";
		}
		else {
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}



void AnularMatriz(vector<vector<unsigned char> > &m){
	for (int i=0; i<m[0].size(); i++){
		for (int j=0; j<m.size(); j++){
			m[i][j]=0;
		}
	}
}


// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan){
  AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			switch (cst.orientacion) {
				case 0: cst.fila--; break;
				case 1: cst.columna++; break;
				case 2: cst.fila++; break;
				case 3: cst.columna--; break;
			}
			mapaConPlan[cst.fila][cst.columna]=1;
		}
		else if (*it == actTURN_R){
			cst.orientacion = (cst.orientacion+1)%4;
		}
		else {
			cst.orientacion = (cst.orientacion+3)%4;
		}
		it++;
	}
}



int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}
