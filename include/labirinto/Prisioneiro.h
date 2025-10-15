/**
 * @file Prisioneiro.h
 * @author
 * @brief Definição da classe Prisioneiro, o protagonista da simulação.
 * @details Este arquivo contém a declaração da classe Prisioneiro, seus atributos
 * (como posição, kits de comida) e os métodos que governam seu comportamento
 * de exploração e backtracking no labirinto.
 */

#pragma once

#include <vector>
#include "estruturas/MinhaListaAdj.h"
#include "estruturas/MeuPair.h"
#include "estruturas/Novelo.h"

// Declaração antecipada da classe Grafo para evitar inclusão circular
class Grafo;

/**
 * @class Prisioneiro
 * @brief Representa o agente que tenta escapar do labirinto.
 * @details A inteligência do Prisioneiro é baseada na exploração de caminhos
 * desconhecidos (busca em profundidade) e na capacidade de retornar por onde veio
 * usando uma estrutura de "novelo de lã" (pilha) quando encontra becos sem saída.
 */
class Prisioneiro {
public:
    /**
     * @struct HistoricoPrisioneiro
     * @brief Registra um evento significativo na jornada do prisioneiro.
     */
    struct HistoricoPrisioneiro {
        std::string tipo;
        std::string descricao;
        int posicao;
        int tempo;
    };

    /**
     * @struct TamanhoDoNovelo
     * @brief Define uma constante para o tamanho máximo do vetor de visitados.
     * @details Usado para pré-alocar o vetor que marca as salas já visitadas.
     */
    struct TamanhoDoNovelo {
    static const int tamanhoNovelo = 1000;
    };

    /**
     * @brief Construtor da classe Prisioneiro.
     * @param posInicial O vértice (sala) onde o prisioneiro inicia sua jornada.
     * @param supDias A quantidade inicial de kits de comida que o prisioneiro carrega.
     */
    Prisioneiro(int posInicial, int supDias);

    /**
     * @brief Adiciona um acontecimento ao histórico detalhado do prisioneiro.
     * @param tipo A categoria do evento.
     * @param descricao Uma descrição textual do evento.
     * @param posicao A sala onde o evento ocorreu.
     * @param tempo O custo associado ao evento.
     */
    void registrarAcontecimento(const std::string& tipo, const std::string& descricao, int posicao, int tempo);

    /**
     * @brief Destrutor da classe Prisioneiro.
     */
    ~Prisioneiro();

    /**
     * @brief Executa a lógica de movimento do prisioneiro para um novo vértice.
     * @details Prioriza mover-se para uma sala adjacente não visitada. Se não houver, tenta o backtracking.
     * @param vizinhos A lista de adjacências do vértice atual.
     * @return O custo (peso da aresta) do movimento realizado. Retorna 0 se nenhum movimento for possível.
     */
    int mover(const listaAdj<MeuPair<int, int>>& vizinhos);

    /**
     * @brief Obtém a posição atual (vértice) do prisioneiro.
     * @return O ID do vértice atual.
     */
    int getPos() const;

    /**
     * @brief Obtém o caminho percorrido pelo prisioneiro.
     * @return Uma referência constante ao vetor com a sequência de vértices visitados.
     */
    const std::vector<int>& getCaminho() const;

    /**
     * @brief Obtém a quantidade de kits de comida restantes.
     * @return A quantidade atual de kits de comida.
     */
    int getKitsDeComida() const;

    /**
     * @brief Verifica se um vértice já foi visitado.
     * @param vertice O ID do vértice a ser verificado.
     * @return `true` se o vértice foi visitado, `false` caso contrário.
     */
    bool foiVisitado(int vertice) const; // Método público adicionado

    /**
     * @brief Executa a lógica de backtracking para a sala anterior.
     * @return O custo do movimento de retorno. Retorna 0 se não for possível voltar.
     */
    int voltarAtras();

    /**
     * @brief Obtém o tempo global da simulação na perspectiva do prisioneiro.
     * @return O valor atual do tempo.
     */
    int getTempoPrisioneiro() const { return tempoPrisioneiro; }

    /**
     * @brief Define o tempo global da simulação para o prisioneiro.
     * @param tempo O novo valor do tempo.
     */
    void setTempoPrisioneiro(int tempo) { tempoPrisioneiro = tempo; }

private:
    int pos; // Vértice atual do prisioneiro
    int kitsDeComida; // Número de kits de comida disponíveis
    int tempoPrisioneiro; // Tempo atual do prisioneiro
    Novelo<MeuPair<int, int>> novelo; // Simula o fio de lã para o backtracking
    std::vector<int> caminho; // Histórico dos vértices visitados
    std::vector<HistoricoPrisioneiro> historico; // Histórico detalhado dos acontecimentos
    std::vector<bool> visitados; // Marca os vértices já visitados
};