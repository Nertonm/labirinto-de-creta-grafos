#include "labirinto/Prisioneiro.h"
#include "labirinto/Prisioneiro.h"

bool Prisioneiro::foiVisitado(int vertice) const {
    return vertice >= 0 && vertice < (int)visitados.size() ? visitados[vertice] : false;
}
#include <iostream>
#include "estruturas/Novelo.h"


Prisioneiro::Prisioneiro(int salaInicial, int kitsDeComida) {
    pos = salaInicial;
    this->kitsDeComida = kitsDeComida;
    visitados.resize(TamanhoDoNovelo::tamanhoNovelo, false);
    visitados[pos] = true;

    caminho.push_back(pos);
    novelo.criarRastro({pos, 0});
}

int Prisioneiro::getPos() const {
    return pos;
}

const std::vector<int>& Prisioneiro::getCaminho() const {
    return caminho;
}

int Prisioneiro::mover(const listaAdj<MeuPair<int, int>>& vizinhos) {
    auto no_vizinho = vizinhos.get_cabeca();

    if (kitsDeComida <= 0) {
        std::cout << "[DEBUG] Prisioneiro não pode se mover, kits de comida esgotados!\n";
        return 0;
    }

    // Debug: mostrar vizinhos e visitados
    std::clog << "\n[DEBUG] TURNO DO PRISIONEIRO" << std::endl;
    std::clog << "  - Sala atual: " << pos << std::endl;
    std::clog << "  - Kits de comida restantes: " << kitsDeComida << std::endl;
    std::clog << "  - Caminho até agora: ";
    for (int v : caminho) std::clog << v << " ";
    std::clog << std::endl;
    std::clog << "  - Vizinhos da sala " << pos << ": ";
    if (&vizinhos == nullptr) {
        std::cout << "[ERRO] Ponteiro de vizinhos é nulo para sala " << pos << std::endl;
        return 0;
    }
    std::cout << "[DEBUG] Vizinhos da sala " << pos << ": ";
    auto no_vizinho_dbg = vizinhos.get_cabeca();
    if (no_vizinho_dbg == nullptr) {
        std::clog << "Nenhum vizinho encontrado." << std::endl;
    }
    while (no_vizinho_dbg != nullptr) {
        int v = no_vizinho_dbg->dado.primeiro;
        std::clog << v << "(peso: " << no_vizinho_dbg->dado.segundo << ", visitado: " << (foiVisitado(v) ? "sim" : "não") << ") ";
        no_vizinho_dbg = no_vizinho_dbg->prox;
    }
    std::clog << std::endl;

    // Procura um caminho (vizinho) que ainda não foi visitado
    while (no_vizinho != nullptr)
    {
        int proximo_vertice = no_vizinho->dado.primeiro;
        int peso_aresta = no_vizinho->dado.segundo;

        if (!foiVisitado(proximo_vertice)) {
            if (kitsDeComida < peso_aresta) {
                std::clog << "  - Não pode mover para " << proximo_vertice << " (peso: " << peso_aresta << ", kits restantes: " << kitsDeComida << ") - Motivo: kits insuficientes." << std::endl;
                no_vizinho = no_vizinho->prox;
                continue;
            }
            novelo.criarRastro({pos, peso_aresta});
            pos = proximo_vertice;
            visitados[pos] = true;
            caminho.push_back(pos);
            kitsDeComida -= peso_aresta; // Consome kits de comida
            std::clog << "  - Escolha: mover para " << pos << " (peso: " << peso_aresta << ", kits restantes: " << kitsDeComida << ") - Motivo: sala não visitada e kits suficientes." << std::endl;
            return peso_aresta;
        }
        no_vizinho = no_vizinho->prox;
    }
    // Se todos os vizinhos já foram visitados, faz backtracking
    if (!novelo.nenhumRastro()) {
        MeuPair<int, int> par = novelo.topo();
        if (kitsDeComida < par.segundo) {
            std::clog << "  - Não pode fazer backtracking para " << par.primeiro << " (peso: " << par.segundo << ", kits restantes: " << kitsDeComida << ") - Motivo: kits insuficientes." << std::endl;
            return 0;
        }
        novelo.puxarRastro();
        int vertice_volta = par.primeiro;
        pos = vertice_volta;
        caminho.push_back(pos);
        kitsDeComida -= par.segundo; // Consome kits de comida
        std::clog << "  - Escolha: backtracking para " << pos << " (peso: " << par.segundo << ", kits restantes: " << kitsDeComida << ") - Motivo: todos vizinhos visitados, rastro disponível e kits suficientes." << std::endl;
        return par.segundo; 
    }
    std::clog << "  - Prisioneiro está engasgado, sem vizinhos e sem rastro!" << std::endl;
    return 0;
}


int Prisioneiro::getKitsDeComida() const {
    return kitsDeComida;
}

Prisioneiro::~Prisioneiro() {
}