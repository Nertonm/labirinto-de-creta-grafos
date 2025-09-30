#ifndef MINHA_PILHA_H
#define MINHA_PILHA_H

#include "PilhaNo.h"
#include <vector>
#include <stdexcept>

template <typename T>
class MinhaPilha {
private:
    NoPilha<T>* ponteiroTopo; // Ponteiro que sempre aponta para o topo da pilha
    int tamanhoPilha;
public:
    MinhaPilha() : ponteiroTopo(nullptr), tamanhoPilha(0) {}
    ~MinhaPilha() {
        while (!estaVazia()) {
            desempilhar();
        }
    }

    void empilhar(const T& valor) {
        NoPilha<T>* novoNo = new NoPilha<T>(valor);
        novoNo->proximo = ponteiroTopo;
        ponteiroTopo = novoNo;
        tamanhoPilha++;
    }

    void desempilhar() {
        if (estaVazia()) {
            throw std::runtime_error("Erro: A pilha está vazia. Impossível desempilhar.");
        }
        NoPilha<T>* temp = ponteiroTopo;
        ponteiroTopo = ponteiroTopo->proximo;
        delete temp;
        tamanhoPilha--;
    }

    T& topo() {
        if (estaVazia()) {
            throw std::runtime_error("Erro: A pilha está vazia.");
        }
        return ponteiroTopo->dado;
    }
    
    const T& topo() const {
        if (estaVazia()) {
            throw std::runtime_error("Erro: A pilha está vazia.");
        }
        return ponteiroTopo->dado;
    }

    bool estaVazia() const {
        return ponteiroTopo == nullptr;
    }

    int tamanho() const {
        return tamanhoPilha;
    }

};
#endif // MINHA_PILHA_H