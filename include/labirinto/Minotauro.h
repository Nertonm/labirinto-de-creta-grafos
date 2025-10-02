#pragma once

#include "Grafo.h"

class Minotauro {
public:
    Minotauro(int posInicial, int percepcao, const Grafo& labirinto, int nV);

    int getPos() const;
    void lembrarCaminhos();
    int lembrarProxPasso(int atual, int dest);
    int lembrarDist(int atual, int dest);
    void mover(int prxVertice);

private:
    int pos;
    int percepcao;
    std::vector<std::vector<int>>  memoriaCaminho;
    std::vector<std::vector<int>> memoriaDistancias;
    const Grafo& labirinto;
    int memoriaNumeroDeSalas;
};