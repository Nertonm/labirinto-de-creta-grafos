#ifndef FIB_NO_H
#define FIB_NO_H

#include <cstddef>

template <typename T>
class FibHeap; // Declaração "forward" necessária para o friend

template <typename T>
class FibNo {
private:
    T chave;
    int grau;
    bool marcado;
    FibNo<T>* pai;     
    FibNo<T>* filho;
    FibNo<T>* esquerda;
    FibNo<T>* direita;

    FibNo(T k) : chave(k), grau(0), marcado(false), pai(nullptr), filho(nullptr), esquerda(this), direita(this) {}

    // A linha mágica que resolve quase todos os erros:
    friend class FibHeap<T>; 
};

#endif