/**
 * @file MinhaListaAdj.h
 * @brief Definição de uma classe de lista duplamente encadeada genérica.
 * @details Este arquivo contém a implementação de uma lista customizada, usada
 * para criar as listas de adjacência na representação do grafo.
 */

#ifndef MINHA_LISTA_H
#define MINHA_LISTA_H

/**
 * @class listaAdj
 * @brief Implementação de uma lista duplamente encadeada genérica.
 * @details Esta classe gerencia uma coleção de nós ligados por ponteiros `prox` (próximo)
 * e `ant` (anterior), permitindo a inserção eficiente no final da lista.
 * @tparam T O tipo de dado a ser armazenado na lista.
 */
template <typename T>
class listaAdj {
public:
    /**
     * @struct No
     * @brief Representa um nó individual dentro da lista duplamente encadeada.
     */
    struct No{
        T dado;
        No* prox;
        No* ant;
        /** 
         * @brief Construtor do nó.
         * @param d O dado a ser armazenado.
         * @param p Ponteiro para o próximo nó (padrão: nullptr).
         * @param a Ponteiro para o nó anterior (padrão: nullptr).
         */
        No(T d, No* p = nullptr, No* a = nullptr) : dado(d), prox(p), ant(a) {}
    };

private:
    No* cabeca;
    No* cauda;
    int tamanho;

public:
    /**
     * @brief Construtor que cria uma lista vazia.
     */
    listaAdj() : cabeca(nullptr), cauda(nullptr), tamanho(0) {}

    /**
     * @brief Construtor de cópia (deletado).
     * @details A cópia é proibida para evitar problemas de gerenciamento de memória,
     * como a liberação dupla (double free) dos ponteiros.
     */
    listaAdj(const listaAdj&) = delete;

    /**
     * @brief Operador de atribuição de cópia (deletado).
     * @details Proibido pelos mesmos motivos do construtor de cópia.
     */
    listaAdj& operator=(const listaAdj&) = delete;
    
    /**
     * @brief Destrutor que libera a memória de todos os nós da lista.
     */
    ~listaAdj() {
        No* atual = cabeca;
        while (atual) {
            No* proximo = atual->prox;
            delete atual;
            atual = proximo;
        }
    }

    /**
     * @brief Insere um novo elemento no final da lista.
     * @details Esta operação tem complexidade de tempo constante, O(1), graças
     * ao ponteiro `cauda`.
     * @param dado O valor a ser inserido.
     */
    void inserir_no_fim(T dado) {
        No* novo_no = new No(dado, nullptr, cauda);
        if (!cabeca) {
            cabeca = cauda = novo_no;
        } else {
            cauda->prox = novo_no;
            cauda = novo_no;
        }
        tamanho++;
    }

    /**
     * @brief Retorna o número de elementos na lista.
     * @return O tamanho atual da lista.
     */
    int size() const {
        return tamanho;
    }

    /**
     * @brief Obtém um ponteiro para o primeiro nó da lista.
     * @details Útil para iniciar a iteração manual sobre os elementos da lista.
     * Ex: `for (auto no = lista.get_cabeca(); no != nullptr; no = no->prox) { ... }`
     * @return Um ponteiro para o nó `cabeca`.
     */
    No* get_cabeca() const {
        return cabeca;
    }
};

#endif