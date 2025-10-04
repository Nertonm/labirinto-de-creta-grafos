
#include "labirinto/Prisioneiro.h"
#include "labirinto/Prisioneiro.h"
#include <iostream>
#include "estruturas/Novelo.h"
#include <iomanip>

bool Prisioneiro::foiVisitado(int vertice) const {
    return vertice >= 0 && vertice < (int)visitados.size() ? visitados[vertice] : false;
}

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

void Prisioneiro::registrarAcontecimento(const std::string& tipo, const std::string& descricao, int posicao, int tempo) {
    HistoricoPrisioneiro h{tipo, descricao, posicao, tempo};
    historico.push_back(h);
}

void Prisioneiro::imprimirHistorico() const {
    std::cout << "\n--- Histórico do Prisioneiro ---\n";
    for (const auto& h : historico) {
        std::cout << "Tipo: " << h.tipo << " | Descrição: " << h.descricao
                  << " | Posição: " << h.posicao << " | Tempo: " << h.tempo << std::endl;
    }
    std::cout << "-------------------------------\n";
}

const std::vector<int>& Prisioneiro::getCaminho() const {
    return caminho;
}

int Prisioneiro::mover(const listaAdj<MeuPair<int, int>>& vizinhos) {
    auto no_vizinho = vizinhos.get_cabeca();

    if (kitsDeComida <= 0) {
        std::cout << "[DEBUG] Prisioneiro não pode se mover, kits de comida esgotados!\n";
        registrarAcontecimento("FIM", "Kits de comida esgotados", pos, 0);
        return 0;
    }

    // Procura um caminho (vizinho) que ainda não foi visitado
    while (no_vizinho != nullptr)
    {
        int proximo_vertice = no_vizinho->dado.primeiro;
        int peso_aresta = no_vizinho->dado.segundo;

        if (!foiVisitado(proximo_vertice)) {
            if (kitsDeComida < peso_aresta) {
                std::clog << "  - Não pode mover para " << proximo_vertice << " (peso: " << peso_aresta << ", kits restantes: " << kitsDeComida << ") - Motivo: kits insuficientes." << std::endl;
                registrarAcontecimento("FALHA", "Kits insuficientes para mover", proximo_vertice, peso_aresta);
                no_vizinho = no_vizinho->prox;
                continue;
            }
            novelo.criarRastro({pos, peso_aresta});
            pos = proximo_vertice;
            visitados[pos] = true;
            caminho.push_back(pos);
            kitsDeComida -= peso_aresta; // Consome kits de comida
            std::clog << "  - Escolha: mover para " << pos << " (peso: " << peso_aresta << ", kits restantes: " << kitsDeComida << ") - Motivo: sala não visitada e kits suficientes." << std::endl;
            registrarAcontecimento("MOVIMENTO", "Moveu para nova sala", pos, peso_aresta);
            return peso_aresta;
        }
        no_vizinho = no_vizinho->prox;
    }
    int peso_volta = voltarAtras();
    return peso_volta;
}


int Prisioneiro::getKitsDeComida() const {
    return kitsDeComida;
}

Prisioneiro::~Prisioneiro() {
}

int Prisioneiro::voltarAtras(){
    // Se todos os vizinhos já foram visitados, faz backtracking
    if (!novelo.nenhumRastro()) {
        MeuPair<int, int> rastroAnterior = novelo.topo();
        if (kitsDeComida < rastroAnterior.segundo) {
            std::clog << "  - Não pode fazer backtracking para " << rastroAnterior.primeiro << " (peso: " << rastroAnterior.segundo << ", kits restantes: " << kitsDeComida << ") - Motivo: kits insuficientes." << std::endl;
            return 0;
        }
    novelo.puxarRastro();
    int vertice_volta = rastroAnterior.primeiro;
    pos = vertice_volta;
    caminho.push_back(pos);
    kitsDeComida -= rastroAnterior.segundo; // Consome kits de comida
    std::clog << "  - Escolha: backtracking para " << pos << " (peso: " << rastroAnterior.segundo << ", kits restantes: " << kitsDeComida << ") - Motivo: todos vizinhos visitados, rastro disponível e kits suficientes." << std::endl;
    registrarAcontecimento("BACKTRACK", "Voltou para sala anterior", pos, rastroAnterior.segundo);
    return rastroAnterior.segundo;
    }
    std::clog << "  - Prisioneiro está engasgado, sem vizinhos e sem rastro!" << std::endl;
    return 0;
}