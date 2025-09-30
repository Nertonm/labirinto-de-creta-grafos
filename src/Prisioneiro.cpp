#include "labirinto/Prisioneiro.h"
#include <iostream>
#include "estruturas/Novelo.h"

Prisioneiro::Prisioneiro(int salaInicial, int numSalas) {
    posAtual = salaInicial;
    visitados.resize(numSalas, false);

    visitados[posAtual] = true;
    caminho.push_back(posAtual);
    novelo.criarRastro({posAtual, 0});
}

int Prisioneiro::getPosAtual() const {
    return posAtual;
}

const std::vector<int>& Prisioneiro::getCaminho() const {
    return caminho;
}

void Prisioneiro::mover(const listaAdj<MeuPair<int, int>>& vizinhos,  int& tempo_restante) {
    auto no_vizinho = vizinhos.get_cabeca();

    // Procura um caminho (vizinho) que ainda não foi visitado
    while (no_vizinho != nullptr)
    {
        int proximo_vertice = no_vizinho->dado.primeiro;
        int peso_aresta = no_vizinho->dado.segundo;

        if (!visitados[proximo_vertice]) {
            novelo.criarRastro({posAtual, peso_aresta});

            tempo_restante -= peso_aresta;
            posAtual = proximo_vertice;

            visitados[posAtual] = true;

            caminho.push_back(posAtual);

            return;
        }
        no_vizinho = no_vizinho->prox;
    }
    // Se todos os vizinhos já foram visitados, faz backtracking
    if (!novelo.nenhumRastro()) {
        MeuPair<int, int> par = novelo.topo();
        novelo.puxarRastro();

        int vertice_volta = par.primeiro;
        int custo_retorno = par.segundo;

        tempo_restante -= custo_retorno;
        posAtual = vertice_volta;
        caminho.push_back(posAtual);
    }
}

Prisioneiro::~Prisioneiro() {
}