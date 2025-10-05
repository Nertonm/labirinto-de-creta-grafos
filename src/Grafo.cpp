#include "labirinto/Grafo.h"
#include "utils/Logger.h"
#include <iostream>
#include "estruturas/MinhaListaAdj.h"
#include "estruturas/MeuPair.h"

Grafo::Grafo() : nV(0), nA(0), vSaida(-1) {
    // O construtor é um bom lugar para inicializar os membros com valores padrão.
    Logger::info(0.0, "Objeto Grafo criado.");
}

Grafo::~Grafo() {
    Logger::info(0.0, "Objeto Grafo destruído.");
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

int Grafo::getPesoAresta(int u, int v) const {
    auto it = adjacencias.find(u);
    if (it != adjacencias.end()) {
        const auto& lista = it->second;
        for (auto no = lista.get_cabeca(); no != nullptr; no = no->prox) {
            if (no->dado.primeiro == v) {
                return no->dado.segundo; // Retorna o peso da aresta
            }
        }
    }
    return -1; // Retorna -1 se a aresta não existir
}

const listaAdj<MeuPair<int, int>>& Grafo::get_vizinhos(int vertice) const {
    static const listaAdj<MeuPair<int, int>> vazia;
    auto it = adjacencias.find(vertice);
    if (it != adjacencias.end()) {
        return it->second;
    }
    return vazia;
}
