/**
 * @file Minotauro.cpp
 * @author Thiago Nerton
 * @version 3.0
 * @brief Implementação da classe Minotauro.
 * @details Este arquivo contém a lógica de programação para os métodos da classe
 * Minotauro, incluindo a implementação do algoritmo de Floyd-Warshall para
 * o conhecimento do labirinto e as ações de movimento e consulta de memória.
 */

#include "labirinto/Minotauro.h"
#include "utils/Logger.h"
#include "labirinto/Grafo.h"
#include <limits> 
#include <iostream>

/// @brief Define um valor para representar a distância infinita, útil na inicialização de algoritmos de caminho mínimo.
const int INF = std::numeric_limits<int>::max();


/**
 * @brief Construtor da classe Minotauro.
 */
Minotauro::Minotauro(int posInicial, int percepcao, const Grafo& labirinto, int nV)
    : pos(posInicial), percepcao(percepcao), labirinto(labirinto), memoriaNumeroDeSalas(nV) {
    memoriaCaminho.resize(nV); // Inicializa o tamanho da matriz de caminhos
    memoriaDistancias.resize(nV); // Inicializa o tamanho da matriz de distâncias
}


/**
 * @brief Pré-calcula e memoriza os caminhos mínimos entre todos os pares de vértices.
 */
void Minotauro::lembrarCaminhos() {
    int n = memoriaNumeroDeSalas;
    // Inicializa as matrizes de distâncias e caminhos
    memoriaDistancias.assign(n, std::vector<int>(n, INF));
    memoriaCaminho.assign(n, std::vector<int>(n, -1));

    // Preenche as matrizes com as distâncias iniciais e os próximos passos
    for (int u = 0; u < n; ++u) {
        memoriaDistancias[u][u] = 0; // Distância para si mesmo é zero
        const auto& vizinhos = labirinto.get_vizinhos(u); 
        for (auto no = vizinhos.get_cabeca(); no != nullptr; no = no->prox) {
            int v = no->dado.primeiro;
            int peso = no->dado.segundo;
            memoriaDistancias[u][v] = peso;  // Distância direta entre u e v
            memoriaCaminho[u][v] = v;  // Próximo passo de u para v é v
        }
    }

    // Algoritmo de Floyd-Warshall para encontrar os caminhos mínimos entre todos os pares de vértices
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                // Verifica se o caminho passando por k é mais curto
                if (memoriaDistancias[i][k] != INF && memoriaDistancias[k][j] != INF && 
                    memoriaDistancias[i][k] + memoriaDistancias[k][j] < memoriaDistancias[i][j]) {
                    // Se sim, Atualiza a distância e o caminho
                    memoriaDistancias[i][j] = memoriaDistancias[i][k] + memoriaDistancias[k][j];
                    memoriaCaminho[i][j] = memoriaCaminho[i][k];
                }
            }
        }
    }
}

/**
 * @brief Consulta a memória para encontrar o próximo passo em um caminho mínimo.
 */
int Minotauro::lembrarProxPasso(int atual, int dest){
    // Verifica se os índices estão dentro dos limites
    if (atual < 0 || dest < 0 || atual >= memoriaNumeroDeSalas || dest >= memoriaNumeroDeSalas) {
    Logger::error(0.0, "lembrarProxPasso: Indice fora do limite: atual={}, dest={}", Logger::LogSource::MINOTAURO, atual, dest);
        return -1;
    }
    // Retorna o próximo vértice no caminho mínimo de 'atual' para 'dest'
    int prox = memoriaCaminho[atual][dest];
    Logger::info(0.0, "Minotauro::lembrarProxPasso: atual={}, dest={}, prox={}", Logger::LogSource::MINOTAURO, atual, dest, prox);
    return prox;
}

/**
 * @brief Consulta a memória para encontrar a distância de um caminho mínimo.
 */
int Minotauro::lembrarDist(int atual, int dest){
    // Verifica se os índices estão dentro dos limites
    if (atual < 0 || dest < 0 || atual >= memoriaNumeroDeSalas || dest >= memoriaNumeroDeSalas) {
    Logger::error(0.0, "lembrarDist: Indice fora do limite: atual={}, dest={}", Logger::LogSource::MINOTAURO, atual, dest);
        return -1;
    }
    // Retorna a distância mínima de 'atual' para 'dest'
    int dist = memoriaDistancias[atual][dest];
    Logger::info(0.0, "Minotauro::lembrarDist: atual={}, dest={}, dist={}", Logger::LogSource::MINOTAURO, atual, dest, dist);
    return dist;
}

/**
 * @brief Atualiza a posição atual do Minotauro.
 */
void Minotauro::mover(int prxVertice) {
    Logger::info(0.0, "Minotauro::mover: de {} para {}", Logger::LogSource::MINOTAURO, pos, prxVertice);
    pos = prxVertice;
}

/**
 * @brief Obtém o alcance de percepção do Minotauro.
 */
int Minotauro::getPercepcao() const {
    return percepcao;
}


/**
 * @brief Obtém a posição atual do Minotauro.
 */
int Minotauro::getPos() const {
    return pos;
}
