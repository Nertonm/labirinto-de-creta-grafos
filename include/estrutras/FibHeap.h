#ifndef FIB_HEAP_H
#define FIB_HEAP_H

#include "FibNo.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <stdexcept> // Para runtime_error
#include <algorithm> // Para swap


template <typename T>
class FibHeap {
private:
    FibNo<T>* minNo; // Ponteiro para a raiz com a menor chave
    int numNos;      // Número total de nós no heap

    void consolidar();
    void linkar(FibNo<T>* y, FibNo<T>* x);
    void cortar(FibNo<T>* x, FibNo<T>* y);
    void corteCascata(FibNo<T>* y);
    void adicionarListaRaiz(FibNo<T>* node);

public:
    FibHeap() : minNo(nullptr), numNos(0) {}
    ~FibHeap();

    bool ehVazio() const { return minNo == nullptr; }
    int getTamanho() const { return numNos; }
    const T& getMin() const;

    void inserir(const T& key);
    T extrairMin();
    void diminuirChave(FibNo<T>* no, const T& novoValor);

    void unir(FibHeap<T>& outroHeap);
};


template <typename T>
const T& FibHeap<T>::getMin() const {
    if (ehVazio()) throw std::runtime_error("Heap vazio");
    return minNo->chave;
}

// Insere um nó. Custo: O(1)
template <typename T>
void FibHeap<T>::inserir(const T& valor){
    FibNo<T>* novoNo = new FibNo<T>(valor);
    adicionarListaRaiz(novoNo);
    if (minNo == nullptr || novoNo->chave < minNo->chave){
        minNo = novoNo;
    }

    numNos++;
}

// Junta um heap a este. Custo: O(1)
template <typename T>
void FibHeap<T>::unir(FibHeap& otherHeap) {
    if (otherHeap.minNo == nullptr) return;
    if (minNo == nullptr) {
        minNo = otherHeap.minNo;
        numNos = otherHeap.numNos;
        return;
    }

    // Concatena as duas listas de raízes
    minNo->direita->esquerda = otherHeap.minNo->esquerda;
    otherHeap.minNo->esquerda->direita = minNo->direita;
    minNo->direita = otherHeap.minNo;
    otherHeap.minNo->esquerda = minNo;

    // Atualiza o mínimo
    if (otherHeap.minNo->chave < minNo->chave) {
        minNo = otherHeap.minNo;
    }
    numNos += otherHeap.numNos;
}

// Custo Amortizado: O(log n)
template <typename T>
T FibHeap<T>::extrairMin() {
    FibNo<T>* z = minNo;
    if (z == nullptr) {
        throw std::runtime_error("Heap está vazio");
    }

    T minValue = z->chave;

    // Promove os filhos de minNo para a lista de raízes
    if (z->filho != nullptr) {
        FibNo<T>* atual = z->filho;
        do {
            atual->pai = nullptr;
            atual = atual->direita;
        } while (atual != z->filho);

        // Adiciona a lista de filhos à lista de raízes
        minNo->direita->esquerda = z->filho->esquerda;
        z->filho->esquerda->direita = minNo->direita;
        minNo->direita = z->filho;
        z->filho->esquerda = minNo;
    }

    // Remove minNo da lista de raízes
    z->esquerda->direita = z->direita;
    z->direita->esquerda = z->esquerda;

    // Define novo minNo e consolida
    if (z == z->direita) { // Se era o único nó
        minNo = nullptr;
    } else {
        minNo = z->direita; // Ponto de partida temporário
        consolidar();      
    }
    numNos--;
    delete z;
    return minValue;
}

// Função auxiliar para juntar árvores de mesmo grau
template <typename T>
void FibHeap<T>::linkar(FibNo<T>* y, FibNo<T>* x) {
    // Remove y da lista de raízes
    y->esquerda->direita = y->direita;
    y->direita->esquerda = y->esquerda;
    y->pai = x;

    // Torna y um filho de x
    if (x->filho == nullptr) {
        x->filho = y;
        y->direita = y;
        y->esquerda = y;
    } else {
        y->esquerda = x->filho;
        y->direita = x->filho->direita;
        x->filho->direita->esquerda = y;
        x->filho->direita = y;
    }
    x->grau++;
    y->marcado = false;
}

// Processo de reorganização
template <typename T>
void FibHeap<T>::consolidar() {
    int maxDegree = static_cast<int>(floor(log(numNos) / log(1.618)));
    std::vector<FibNo<T>*> TabelaGraus(maxDegree + 2, nullptr);

    std::vector<FibNo<T>*> raizes;
    FibNo<T>* atual = minNo;
    if (atual == nullptr) return;
    do {
        raizes.push_back(atual);
        atual = atual->direita;
    } while (atual != minNo);

    for (FibNo<T>* x : raizes) {
        int d = x->grau;
        while (TabelaGraus[d] != nullptr) {
            FibNo<T>* y = TabelaGraus[d];
            if (x->chave > y->chave) {
                std::swap(x, y);
            }
            linkar(y, x);
            TabelaGraus[d] = nullptr;
            d++;
        }
        TabelaGraus[d] = x;
    }

    minNo = nullptr;
    for (int i = 0; i < TabelaGraus.size(); ++i) {
        if (TabelaGraus[i] != nullptr) {
            adicionarListaRaiz(TabelaGraus[i]);
            if (minNo == nullptr || TabelaGraus[i]->chave < minNo->chave) {
                minNo = TabelaGraus[i];
            }
        }
    }
}

// Adiciona um nó à lista de raízes (circular)
template <typename T>
void FibHeap<T>::adicionarListaRaiz(FibNo<T>* node) {
    if (minNo == nullptr) {
        minNo = node;
        node->esquerda = node;
        node->direita = node;
    } else {
        node->direita = minNo->direita;
        node->esquerda = minNo;
        minNo->direita->esquerda = node;
        minNo->direita = node;
    }
    node->pai = nullptr;
}

// Diminuir Chave. Custo Amortizado: O(1)
template <typename T>
void FibHeap<T>::diminuirChave(FibNo<T>* no, const T& novoValor) {
    if (novoValor > no->chave) {
        throw std::invalid_argument("Nova chave é maior que a chave atual.");
    }
    no->chave = novoValor;
    FibNo<T>* y = no->pai;

    // Se a propriedade do heap for violada
    if (y != nullptr && no->chave < y->chave) {
        cortar(no, y);
        corteCascata(y);
    }
    if (no->chave < minNo->chave) {
        minNo = no;
    }
}

// Corta o nó x de seu pai y
template<typename T>
void FibHeap<T>::cortar(FibNo<T>* x, FibNo<T>* y) {
    // Remove x da lista de filhos de y
    if (x == x->direita) {
        y->filho = nullptr;
    } else {
        x->direita->esquerda = x->esquerda;
        x->esquerda->direita = x->direita;
        if (y->filho == x) {
            y->filho = x->direita;
        }
    }
    y->grau--;

    // Adiciona x à lista de raízes
    adicionarListaRaiz(x);
    x->marcado = false;
}

// Realiza o corte em cascata
template<typename T>
void FibHeap<T>::corteCascata(FibNo<T>* y) {
    FibNo<T>* z = y->pai;
    if (z != nullptr) {
        if (!y->marcado) { // Se não está marcado, marque-o
            y->marcado = true;
        } else { // Se já está marcado, corte-o também
            cortar(y, z);
            corteCascata(z);
        }
    }
}

template<typename T>
FibHeap<T>::~FibHeap() {
    while(minNo != nullptr) {
        extrairMin();
    }
}

#endif 
