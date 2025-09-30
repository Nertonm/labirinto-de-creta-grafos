#ifndef NOVELO_H
#define NOVELO_H

#include "LinhaNovelo.h"
#include <vector>
#include <stdexcept>

template <typename T>
class Novelo {
private:
    LinhaNovelo<T>* ultRastro; // Ponteiro que sempre aponta para o ultimo rastro
    int salasComRastro;
public:
    Novelo() : ultRastro(nullptr), salasComRastro(0) {}
    ~Novelo() {
        while (!nenhumRastro()) {
            puxarRastro();
        }
    }

    void criarRastro(const T& valor) {
        LinhaNovelo<T>* novoNo = new LinhaNovelo<T>(valor);
        novoNo->prx = ultRastro;
        ultRastro = novoNo;
        salasComRastro++;
    }

    void puxarRastro() {
        if (nenhumRastro()) {
            throw std::runtime_error("Erro: Não tem rastros. Impossível puxar rastro.");
        }
        LinhaNovelo<T>* tmp = ultRastro;
        ultRastro = ultRastro->prx;
        delete tmp;
        salasComRastro--;
    }

    T& topo() {
        if (nenhumRastro()) {
            throw std::runtime_error("Erro: Não tem rastros.");
        }
        return ultRastro->sala;
    }
    
    const T& topo() const {
        if (nenhumRastro()) {
            throw std::runtime_error("Erro: Não tem rastros.");
        }
        return ultRastro->sala;
    }

    bool nenhumRastro() const {
        return ultRastro == nullptr;
    }

    int getSalasComRastro() const {
        return salasComRastro;
    }

};

#endif 