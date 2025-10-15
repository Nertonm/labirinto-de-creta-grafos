/**
 * @file Grafo.cpp
 * @author Thiago Nerton
 * @brief Implementação da classe Grafo.
 * @details Este arquivo contém a lógica para construir e manipular a estrutura
 * de dados do grafo que representa o labirinto, incluindo a adição de arestas
 * e a consulta de vizinhos e pesos.
 */

#include "labirinto/Grafo.h"
#include "utils/Logger.h"
#include <iostream>
#include "estruturas/MinhaListaAdj.h"
#include "estruturas/MeuPair.h"

/**
 * @brief Construtor padrão da classe Grafo.
 * @details Inicializa um grafo vazio, definindo os contadores de vértices e arestas
 * como 0 e o vértice de saída como -1 (inválido).
 */
Grafo::Grafo() : nV(0), nA(0), vSaida(-1) {
    // O construtor é um bom lugar para inicializar os membros com valores padrão.
    Logger::info(0.0, "Objeto Grafo criado.");
}

/**
 * @brief Destrutor da classe Grafo.
 */
Grafo::~Grafo() {
    Logger::info(0.0, "Objeto Grafo destruído.");
}

/**
 * @brief Adiciona uma aresta ponderada entre dois vértices.
 * @details Como o labirinto é representado por um grafo não-direcionado, a aresta
 * é adicionada nas listas de adjacência de ambos os vértices (u -> v e v -> u).
 * @param u O primeiro vértice da aresta.
 * @param v O segundo vértice da aresta.
 * @param peso O custo (distância/tempo) para percorrer a aresta.
 */
void Grafo::adicionar_aresta(int u, int v, int peso) {
    // Grafo não-direcionado: adiciona a aresta em ambos os sentidos
    adjacencias[u].inserir_no_fim(MeuPair<int, int>(v, peso));
    adjacencias[v].inserir_no_fim(MeuPair<int, int>(u, peso));
}

/**
 * @brief Define o vértice que representa a saída do labirinto.
 * @param vSaida O ID do vértice de saída.
 */
void Grafo::set_saida(int vSaida) {
    this->vSaida = vSaida;
}

/**
 * @brief Obtém o vértice que representa a saída do labirinto.
 * @return O ID do vértice de saída.
 */
int Grafo::get_saida() const {
    return vSaida;
}

/**
 * @brief Obtém o peso de uma aresta específica entre dois vértices.
 * @param u O vértice de origem.
 * @param v O vértice de destino.
 * @return O peso da aresta se ela existir.
 * @retval -1 Se a aresta entre u e v não for encontrada.
 */
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

/**
 * @brief Obtém a lista de adjacência de um determinado vértice.
 * @param vertice O ID do vértice cujos vizinhos são desejados.
 * @return Uma referência constante para a lista de adjacência do vértice.
 * Se o vértice não existir, retorna uma referência para uma lista estática vazia.
 */
const listaAdj<MeuPair<int, int>>& Grafo::get_vizinhos(int vertice) const {
    static const listaAdj<MeuPair<int, int>> vazia;
    auto it = adjacencias.find(vertice);
    if (it != adjacencias.end()) {
        return it->second;
    }
    return vazia;
}
