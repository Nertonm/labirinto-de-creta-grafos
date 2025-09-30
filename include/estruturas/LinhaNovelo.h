#ifndef LINHA_NOVELO_H
#define LINHA_NOVELO_H

#include <cstddef> // Para nullptr

template <typename T>
struct LinhaNovelo {
    T sala;
    LinhaNovelo<T>* prx;

    // Construtor para facilitar a criação
    LinhaNovelo(T valor) : sala(valor), prx(nullptr) {}
};

#endif