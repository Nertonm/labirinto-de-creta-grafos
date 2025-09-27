#include "labirinto/Grafo.h"
#include <iostream>

Grafo::Grafo() : num_vertices(0), num_arestas(0), vertice_saida(-1) {
    // O construtor é um bom lugar para inicializar os membros com valores padrão.
    std::cout << "Objeto Grafo criado." << std::endl;
}

Grafo::~Grafo() {
    std::cout << "Objeto Grafo destruído." << std::endl;
}

void Grafo::adicionar_aresta(int u, int v, int peso) {
    // Grafo não-direcionado: adiciona a aresta em ambos os sentidos
    adjacencias[u].push_back({v, peso});
    adjacencias[v].push_back({u, peso});
}

void Grafo::set_saida(int vertice_saida) {
    vertice_saida = vertice_saida;
}

int Grafo::get_saida() const {
    return vertice_saida;
}
