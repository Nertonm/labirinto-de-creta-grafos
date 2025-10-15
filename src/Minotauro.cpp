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
 * @details Inicializa os atributos do Minotauro e aloca espaço para suas
 * matrizes de memória com base no número de vértices do labirinto.
 * @param posInicial O vértice (sala) inicial onde o Minotauro é criado.
 * @param percepcao A distância máxima na qual o Minotauro pode detectar o prisioneiro.
 * @param labirinto Uma referência constante ao objeto Grafo que representa o labirinto.
 * @param nV O número total de vértices no labirinto.
 */
Minotauro::Minotauro(int posInicial, int percepcao, const Grafo& labirinto, int nV)
    : pos(posInicial), percepcao(percepcao), labirinto(labirinto), memoriaNumeroDeSalas(nV) {
    memoriaCaminho.resize(nV); // Inicializa o tamanho da matriz de caminhos
    memoriaDistancias.resize(nV); // Inicializa o tamanho da matriz de distâncias
}


/**
 * @brief Pré-calcula e memoriza os caminhos mínimos entre todos os pares de vértices.
 * @details Este método implementa o algoritmo de Floyd-Warshall. Ele preenche as
 * matrizes `memoriaDistancias` e `memoriaCaminho`, dando ao Minotauro
 * conhecimento onisciente do labirinto. Deve ser chamado uma única vez no início da simulação.
 * @post As matrizes de memória estarão preenchidas com as distâncias e os
 * próximos passos para todos os caminhos mínimos.
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
 * @param atual O vértice de origem.
 * @param dest O vértice de destino.
 * @return O primeiro vértice no caminho mais curto de 'atual' para 'dest'.
 * @retval -1 Se os índices forem inválidos.
 */
int Minotauro::lembrarProxPasso(int atual, int dest){
    // Verifica se os índices estão dentro dos limites
    if (atual < 0 || dest < 0 || atual >= memoriaNumeroDeSalas || dest >= memoriaNumeroDeSalas) {
    Logger::error(tempoMinotauro, "lembrarProxPasso: Indice fora do limite: atual={}, dest={}", Logger::LogSource::MINOTAURO, atual, dest);
        return -1;
    }
    // Retorna o próximo vértice no caminho mínimo de 'atual' para 'dest'
    int prox = memoriaCaminho[atual][dest];
    Logger::info(tempoMinotauro, "Minotauro recorda que a Sala {} tem um caminho para a Sala {}, sendo o próximo passo: {}", Logger::LogSource::MINOTAURO, atual, dest, prox);
    return prox;
}

/**
 * @brief Consulta a memória para encontrar a distância de um caminho mínimo.
 * @param atual O vértice de origem.
 * @param dest O vértice de destino.
 * @return A distância total (soma dos pesos) do caminho mais curto entre 'atual' e 'dest'.
 * @retval -1 Se os índices forem inválidos.
 */
int Minotauro::lembrarDist(int atual, int dest){
    // Verifica se os índices estão dentro dos limites
    if (atual < 0 || dest < 0 || atual >= memoriaNumeroDeSalas || dest >= memoriaNumeroDeSalas) {
    Logger::error(tempoMinotauro, "lembrarDist: Indice fora do limite: atual={}, dest={}", Logger::LogSource::MINOTAURO, atual, dest);
        return -1;
    }
    // Retorna a distância mínima de 'atual' para 'dest'
    int dist = memoriaDistancias[atual][dest];
    //Logger::(tempoMinotauro, "Minotauro na sala {} sente que o prisioneiro está na sala {}, sendo a distância até lá de {}", Logger::LogSource::MINOTAURO, atual, dest, dist);
    return dist;
}

/**
 * @brief Atualiza a posição atual do Minotauro.
 * @param prxVertice O novo vértice para o qual o Minotauro se moverá.
 */
void Minotauro::mover(int prxVertice) {
    // Logger::info(tempoMinotauro, "Minotauro::mover: de {} para {}", Logger::LogSource::MINOTAURO, pos, prxVertice);
    pos = prxVertice;
}

/**
 * @brief Obtém o alcance de percepção do Minotauro.
 * @return A distância máxima de percepção.
*/
int Minotauro::getPercepcao() const {
    return percepcao;
}

/**
 * @brief Obtém a posição atual do Minotauro.
 * @return O ID do vértice onde o Minotauro está localizado.
 */
int Minotauro::getPos() const {
    return pos;
}
