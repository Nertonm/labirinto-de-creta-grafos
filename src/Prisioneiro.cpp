#include "labirinto/Prisioneiro.h"
#include <iostream>
#include "estrutras/MinhaPilha.h"

Prisioneiro::Prisioneiro(int vertice_inicial, int total_vertices) {
    posicao_atual = vertice_inicial;
    visitados.resize(total_vertices, false);
    
    visitados[posicao_atual] = true;
    caminho_percorrido.push_back(posicao_atual);
    novelo_de_la.empilhar({posicao_atual, 0});
}

int Prisioneiro::get_posicao_atual() const {
    return posicao_atual;
}

const std::vector<int>& Prisioneiro::get_caminho_percorrido() const {
    return caminho_percorrido;
}

void Prisioneiro::mover(const MinhaListaAdj<MeuPair<int, int>>& vizinhos,  int& tempo_restante) {
    auto no_vizinho = vizinhos.get_cabeca();

    // Procura um caminho (vizinho) que ainda não foi visitado
    while (no_vizinho != nullptr)
    {
        int proximo_vertice = no_vizinho->dado.primeiro;
        int peso_aresta = no_vizinho->dado.segundo;

        if (!visitados[proximo_vertice]) {
            novelo_de_la.empilhar({posicao_atual, peso_aresta});

            tempo_restante -= peso_aresta;
            posicao_atual = proximo_vertice;

            visitados[posicao_atual] = true;

            caminho_percorrido.push_back(posicao_atual);

            return;
        }
        no_vizinho = no_vizinho->prox;
    }
    // Se todos os vizinhos já foram visitados, faz backtracking
    if (!novelo_de_la.estaVazia()) {
        MeuPair<int, int> par = novelo_de_la.topo();
        novelo_de_la.desempilhar();

        int vertice_volta = par.primeiro;
        int custo_retorno = par.segundo;

        tempo_restante -= custo_retorno;
        posicao_atual = vertice_volta;
        caminho_percorrido.push_back(posicao_atual);
    }
}

Prisioneiro::~Prisioneiro() {
}