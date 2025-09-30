#ifndef NO_PILHA_H
#define NO_PILHA_H

#include <cstddef> // Para nullptr

template <typename T>
struct NoPilha {
    T dado;
    NoPilha<T>* proximo;

    // Construtor para facilitar a criação
    NoPilha(T valor) : dado(valor), proximo(nullptr) {}
};

#endif