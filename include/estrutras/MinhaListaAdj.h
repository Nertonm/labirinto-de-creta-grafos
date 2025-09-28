#ifndef MINHA_LISTA_H
#define MINHA_LISTA_H

template <typename T>
class MinhaListaAdj {
public:
struct No{
    T dado;
    No* prox;
    No* ant;
    No(T d, No* p = nullptr, No* a = nullptr) : dado(d), prox(p), ant(a) {}
};

private:
    No* cabeca;
    No* cauda;
    int tamanho;
public:
    MinhaListaAdj() : cabeca(nullptr), cauda(nullptr), tamanho(0) {}
    
    ~MinhaListaAdj() {
        No* atual = cabeca;
        while (atual) {
            No* proximo = atual->prox;
            delete atual;
            atual = proximo;
        }
    }

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

    int size() const {
        return tamanho;
    }

    No* get_cabeca() const {
        return cabeca;
    }
};

#endif