// MeuPair.h
#ifndef MEU_PAIR_H
#define MEU_PAIR_H

template <typename T1, typename T2>
struct MeuPair {
    T1 primeiro;
    T2 segundo;

    // Construtor para facilitar a criação
    MeuPair(T1 p, T2 s) : primeiro(p), segundo(s) {}

    // Construtor padrão (compiler friendly)
    MeuPair() = default;
};

#endif