#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>

struct estado {
  int fila;
  int columna;
  int orientacion;
  int coste;
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      // Inicializar Variables de Estado
      fil = col = mapaResultado.size()-1;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      actual.fila = fil;
			actual.columna = col;
			actual.orientacion = brujula;
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
      ultimaAccion = actIDLE;
      estoy_bien_situado = false;
      hayPlan = false;
      tengo_coordenadas = false;
      hay_Plan_react = false;
      mapaReactivo.resize(mapaResultado.size()*2);
      for(int i=0;i<mapaResultado.size()*2;i++){
        mapaReactivo[i].resize(mapaResultado[0].size()*2);
        for(int j=0;j<mapaResultado[0].size()*2;j++) mapaReactivo[i][j]='?';
      }
      for(int i=0;i<200;i++) for(int j=0;j<200;j++) mapaPulgarcito[i][j]=0;
      valor_pulg=1;
      contador_vuelta=0;
      for(int i=0;i<4;i++) vuelta[i]=0;
      enAgua=false;
      fil_reac=0;
      col_reac=0;
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
      fil = col = mapaResultado.size()-1;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
      ultimaAccion = actIDLE;
      estoy_bien_situado = false;
      hayPlan = false;
      tengo_coordenadas = false;
      hay_Plan_react = false;
      mapaReactivo.resize(mapaResultado.size()*2);
      for(int i=0;i<mapaResultado.size()*2;i++){
        mapaReactivo[i].resize(mapaResultado[0].size()*2);
        for(int j=0;j<mapaResultado[0].size()*2;j++) mapaReactivo[i][j]='?';
      }
      for(int i=0;i<200;i++) for(int j=0;j<200;j++) mapaPulgarcito[i][j]=0;
      valor_pulg=1;
      contador_vuelta=0;
      for(int i=0;i<4;i++) vuelta[i]=0;
      enAgua=false;
      fil_reac=0;
      col_reac=0;
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const estado &st, const list<Action> &plan);
    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}

  private:
    // Declarar Variables de Estado
    int fil, col, brujula;
    estado actual, destino;
    list<Action> plan;

    // Nuevas variables de estado
    Action ultimaAccion;
    bool estoy_bien_situado;
    bool hayPlan;
    bool tengo_coordenadas;
    bool hay_Plan_react;
    vector< vector< unsigned char> > mapaReactivo;
    int mapaPulgarcito[200][200];
    unsigned long int valor_pulg;
    int contador_vuelta;
    int vuelta[4];
    bool enAgua;
    int fil_reac;
    int col_reac;

    // Métodos privados de la clase
    bool pathFinding(int level, const estado &origen, const estado &destino, list<Action> &plan, std::vector< std::vector< unsigned char> > &mapa);
    bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Coste_Uniforme(const estado &origen, const estado &destino, list<Action> &plan, std::vector< std::vector< unsigned char> > &mapa);
    int calcularCoste(estado casilla, std::vector< std::vector< unsigned char> > &mapa);
    bool pathFinding_Nivel_2(const estado &origen, const estado &destino, list<Action> &plan);
    void PintaPlan(list<Action> plan);
    bool HayObstaculoDelante(estado &st, std::vector< std::vector< unsigned char> > &mapa);
    void escribirMapa(Sensores sensores, std::vector< std::vector< unsigned char> > &mapa);
    void crearDestino(int val, estado &dest);
    bool hayAgua(Sensores sensores);

};

#endif
