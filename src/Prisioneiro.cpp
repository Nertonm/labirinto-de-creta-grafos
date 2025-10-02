#include "labirinto/Prisioneiro.h"
#include <iostream>
#include "estruturas/Novelo.h"


Prisioneiro::Prisioneiro(int salaInicial, int kitsDeComida) {
    pos = salaInicial;
    this->kitsDeComida = kitsDeComida;
    visitados.resize(TamanhoDoNovelo::tamanhoNovelo, false);
    visitados[pos] = true;

    caminho.push_back(pos);
    novelo.criarRastro({pos, 0});
}

int Prisioneiro::getPos() const {
    return pos;
}

const std::vector<int>& Prisioneiro::getCaminho() const {
    return caminho;
}

void Prisioneiro::mover(const listaAdj<MeuPair<int, int>>& vizinhos) {
    auto no_vizinho = vizinhos.get_cabeca();

    // Procura um caminho (vizinho) que ainda não foi visitado
    while (no_vizinho != nullptr)
    {
        int proximo_vertice = no_vizinho->dado.primeiro;
        int peso_aresta = no_vizinho->dado.segundo;

        if (!visitados[proximo_vertice]) {
            novelo.criarRastro({pos, peso_aresta});
            pos = proximo_vertice;
            visitados[pos] = true;
            caminho.push_back(pos);
            kitsDeComida -= peso_aresta; // Consome kits de comida
            return;
        }
        no_vizinho = no_vizinho->prox;
    }
    // Se todos os vizinhos já foram visitados, faz backtracking
    if (!novelo.nenhumRastro()) {
        MeuPair<int, int> par = novelo.topo();
        novelo.puxarRastro();
        int vertice_volta = par.primeiro;
        pos = vertice_volta;
        caminho.push_back(pos);
        kitsDeComida -= par.segundo; // Consome kits de comida
    }
}

Prisioneiro::~Prisioneiro() {
}