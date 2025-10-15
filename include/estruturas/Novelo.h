/**
 * @file Novelo.h
 * @author Thiago Nerton
 * @brief Definição da classe genérica Novelo, uma estrutura de dados de pilha (LIFO).
 * @details Este arquivo contém a implementação de uma pilha usando uma lista encadeada,
 * utilizada pelo Prisioneiro para armazenar seu caminho de volta (backtracking).
 */

#ifndef NOVELO_H
#define NOVELO_H

#include "LinhaNovelo.h"
#include <vector>
#include <stdexcept>

/**
 * @class Novelo
 * @brief Implementação de uma pilha genérica (template).
 * @details Esta classe funciona como uma pilha (Last-In, First-Out), onde o último
 * "rastro" deixado é o primeiro a ser recuperado. É a base para a memória de
 * backtracking do Prisioneiro.
 * @tparam T O tipo de dado a ser armazenado na pilha.
 */
template <typename T>
class Novelo {
private:
    /// @brief Ponteiro para o topo da pilha (o último rastro adicionado).
    LinhaNovelo<T>* ultRastro;
    /// @brief Contador do número de elementos na pilha.
    int salasComRastro;

public:
    /**
     * @brief Construtor que cria um Novelo (pilha) vazio.
     */
    Novelo() : ultRastro(nullptr), salasComRastro(0) {}

    /**
     * @brief Destrutor que libera toda a memória alocada pelos nós da pilha.
     */
    ~Novelo() {
        while (!nenhumRastro()) {
            puxarRastro();
        }
    }

    /**
     * @brief Adiciona um novo elemento no topo da pilha (equivalente a `push`).
     * @param valor O valor a ser adicionado.
     */
    void criarRastro(const T& valor) {
        LinhaNovelo<T>* novoNo = new LinhaNovelo<T>(valor);
        novoNo->prx = ultRastro;
        ultRastro = novoNo;
        salasComRastro++;
    }

    /**
     * @brief Remove o elemento do topo da pilha (equivalente a `pop`).
     * @throws std::runtime_error se a pilha estiver vazia.
     */
    void puxarRastro() {
        if (nenhumRastro()) {
            throw std::runtime_error("Erro: Não tem rastros. Impossível puxar rastro.");
        }
        LinhaNovelo<T>* tmp = ultRastro;
        ultRastro = ultRastro->prx;
        delete tmp;
        salasComRastro--;
    }

    /**
     * @brief Acessa o elemento no topo da pilha sem removê-lo (equivalente a `top` ou `peek`).
     * @return Uma referência ao elemento do topo.
     * @throws std::runtime_error se a pilha estiver vazia.
     */
    T& topo() {
        if (nenhumRastro()) {
            throw std::runtime_error("Erro: Não tem rastros.");
        }
        return ultRastro->sala;
    }
    
    /**
     * @brief Acessa o elemento no topo da pilha (versão constante).
     * @return Uma referência constante ao elemento do topo.
     * @throws std::runtime_error se a pilha estiver vazia.
     */
    const T& topo() const {
        if (nenhumRastro()) {
            throw std::runtime_error("Erro: Não tem rastros.");
        }
        return ultRastro->sala;
    }

    /**
     * @brief Verifica se a pilha está vazia (equivalente a `isEmpty`).
     * @return `true` se a pilha estiver vazia, `false` caso contrário.
     */
    bool nenhumRastro() const {
        return ultRastro == nullptr;
    }

    /**
     * @brief Obtém o número de elementos na pilha (equivalente a `size`).
     * @return A quantidade de elementos atualmente na pilha.
     */
    int getSalasComRastro() const {
        return salasComRastro;
    }
};

#endif