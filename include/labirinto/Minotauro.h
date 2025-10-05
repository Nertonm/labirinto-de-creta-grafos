/**
 * @file Minotauro.h
 * @author Thiago Nerton
 * @version 3.0
 * @brief Definição da classe Minotauro, o antagonista da simulação.
 * @details Este arquivo contém a declaração da classe Minotauro, seus atributos
 * (como posição e percepção) e os métodos que governam seu comportamento,
 * incluindo sua memória onisciente do labirinto.
 */

#pragma once

#include "Grafo.h"

/**
 * @class Minotauro
 * @brief Representa o Minotauro que caça o prisioneiro no labirinto.
 * @details A inteligência do Minotauro é baseada em seu conhecimento completo do
 * grafo do labirinto. Ele pré-calcula todos os caminhos mínimos
 * (usando Floyd-Warshall) para tomar decisões de perseguição eficientes.
 */
class Minotauro {
public:
    /**
     * @brief Construtor da classe Minotauro.
     * @param posInicial O vértice (sala) inicial onde o Minotauro é criado.
     * @param percepcao A distância máxima na qual o Minotauro pode detectar o prisioneiro.
     * @param labirinto Uma referência constante ao objeto Grafo que representa o labirinto.
     * @param nV O número total de vértices no labirinto.
     */
    Minotauro(int posInicial, int percepcao, const Grafo& labirinto, int nV);

    /**
     * @brief Pré-calcula e memoriza os caminhos mínimos entre todos os pares de vértices.
     * @details Este método implementa o algoritmo de Floyd-Warshall para popular as
     * matrizes internas `memoriaDistancias` e `memoriaCaminho`. Deve ser chamado
     * uma única vez, no início da simulação.
     * @post As matrizes de memória estarão preenchidas com as distâncias e os
     * próximos passos para todos os caminhos mínimos.
     */
    void lembrarCaminhos();

    /**
     * @brief Consulta a memória para encontrar o próximo passo em um caminho mínimo.
     * @param atual O vértice de origem.
     * @param dest O vértice de destino.
     * @return O primeiro vértice no caminho mais curto de 'atual' para 'dest'. Retorna -1 se o caminho for desconhecido.
     */
    int lembrarProxPasso(int atual, int dest);

    /**
     * @brief Consulta a memória para encontrar a distância de um caminho mínimo.
     * @param atual O vértice de origem.
     * @param dest O vértice de destino.
     * @return A distância total (soma dos pesos) do caminho mais curto entre 'atual' e 'dest'.
     */
    int lembrarDist(int atual, int dest);

    /**
     * @brief Atualiza a posição atual do Minotauro.
     * @param prxVertice O novo vértice para o qual o Minotauro se moverá.
     */
    void mover(int prxVertice);

    /**
     * @brief Obtém o alcance de percepção do Minotauro.
     * @return A distância máxima de percepção.
     */
    int getPercepcao() const;

    /**
     * @brief Obtém a posição atual do Minotauro.
     * @return O ID do vértice onde o Minotauro está localizado.
     */
    int getPos() const;

    /**
     * @brief Obtém o tempo global da simulação.
     * @return O tempo global atual.
     */
    int getTempoMinotauro() const { return tempoMinotauro; }

    /**
     * @brief Define o tempo global da simulação.
     * @param tempo O novo valor do tempo global.
     * @return O valor atualizado do tempo global.
     */
    void setTempoMinotauro(int tempo) { tempoMinotauro = tempo; }

private:
    /// @brief O vértice (sala) atual onde o Minotauro está.
    int pos;

    /// @brief A distância máxima na qual o prisioneiro pode ser detectado.
    int percepcao;

    /// @brief Matriz que armazena o próximo vértice no caminho mínimo de [origem][destino].
    std::vector<std::vector<int>>  memoriaCaminho;

    /// @brief Matriz que armazena a distância mínima do caminho de [origem][destino].
    std::vector<std::vector<int>> memoriaDistancias;

    /// @brief Referência constante ao grafo do labirinto. Permite a consulta sem modificação.
    const Grafo& labirinto;

    /// @brief Armazena o número total de salas para dimensionar as matrizes de memória.
    int memoriaNumeroDeSalas;

    /// @brief Tempo global da simulação.
    int tempoMinotauro;
};