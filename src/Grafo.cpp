#include "labirinto/Grafo.h"
#include <iostream>
#include "estruturas/MinhaListaAdj.h"
#include "estruturas/MeuPair.h"

Grafo::Grafo() : nV(0), nA(0), vSaida(-1) {
    // O construtor é um bom lugar para inicializar os membros com valores padrão.
    std::cout << "Objeto Grafo criado." << std::endl;
}

Grafo::~Grafo() {
    std::cout << "Objeto Grafo destruído." << std::endl;
}

void Grafo::adicionar_aresta(int u, int v, int peso) {
    // Grafo não-direcionado: adiciona a aresta em ambos os sentidos
    adjacencias[u].inserir_no_fim(MeuPair<int, int>(v, peso));
    adjacencias[v].inserir_no_fim(MeuPair<int, int>(u, peso));
}

void Grafo::set_saida(int vSaida) {
    this->vSaida = vSaida;
}

int Grafo::get_saida() const {
    return vSaida;
}

const listaAdj<MeuPair<int, int>>& Grafo::get_vizinhos(int vertice) const {
    static const listaAdj<MeuPair<int, int>> vazia;
    auto it = adjacencias.find(vertice);
    if (it != adjacencias.end()) {
        return it->second;
    }
    return vazia;
}
