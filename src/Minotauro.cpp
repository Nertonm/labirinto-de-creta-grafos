#include "labirinto/Minotauro.h"
#include "labirinto/Grafo.h"
#include <limits> 
#include <iostream>

const int INF = std::numeric_limits<int>::max();

Minotauro::Minotauro(int posInicial, int percepcao, const Grafo& labirinto, int nV)
    : pos(posInicial), percepcao(percepcao), labirinto(labirinto), memoriaNumeroDeSalas(nV) {
    memoriaCaminho.resize(nV);
    memoriaDistancias.resize(nV);
}

int Minotauro::getPos() const {
    return pos;
}

void Minotauro::lembrarCaminhos() {
    int n = memoriaNumeroDeSalas;
    memoriaDistancias.assign(n, std::vector<int>(n, INF));
    memoriaCaminho.assign(n, std::vector<int>(n, -1));

    for (int u = 0; u < n; ++u) {
        memoriaDistancias[u][u] = 0;
        const auto& vizinhos = labirinto.get_vizinhos(u);
        for (auto no = vizinhos.get_cabeca(); no != nullptr; no = no->prox) {
            int v = no->dado.primeiro;
            int peso = no->dado.segundo;
            memoriaDistancias[u][v] = peso;
            memoriaCaminho[u][v] = v;
        }
    }

    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (memoriaDistancias[i][k] != INF && memoriaDistancias[k][j] != INF && 
                    memoriaDistancias[i][k] + memoriaDistancias[k][j] < memoriaDistancias[i][j]) {
                    memoriaDistancias[i][j] = memoriaDistancias[i][k] + memoriaDistancias[k][j];
                    memoriaCaminho[i][j] = memoriaCaminho[i][k];
                }
            }
        }
    }
}

int Minotauro::lembrarProxPasso(int atual, int dest){
    if (atual < 0 || dest < 0 || atual >= memoriaNumeroDeSalas || dest >= memoriaNumeroDeSalas) {
        std::cerr << "[ERRO] lembrarProxPasso: Indice fora do limite: atual=" << atual << ", dest=" << dest << std::endl;
        return -1;
    }
    int prox = memoriaCaminho[atual][dest];
    std::clog << "[DEBUG] Minotauro::lembrarProxPasso: atual=" << atual << ", dest=" << dest << ", prox=" << prox << std::endl;
    return prox;
}

int Minotauro::lembrarDist(int atual, int dest){
    if (atual < 0 || dest < 0 || atual >= memoriaNumeroDeSalas || dest >= memoriaNumeroDeSalas) {
        std::cerr << "[ERRO] lembrarDist: Indice fora do limite: atual=" << atual << ", dest=" << dest << std::endl;
        return -1;
    }
    int dist = memoriaDistancias[atual][dest];
    std::clog << "[DEBUG] Minotauro::lembrarDist: atual=" << atual << ", dest=" << dest << ", dist=" << dist << std::endl;
    return dist;
}

int Minotauro::getPercepcao() const {
    return percepcao;
}

// Move o Minotauro para o próximo vértice informado
void Minotauro::mover(int prxVertice) {
    std::clog << "[DEBUG] Minotauro::mover: de " << pos << " para " << prxVertice << std::endl;
    pos = prxVertice;
}