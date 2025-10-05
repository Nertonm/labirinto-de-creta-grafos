
#include "labirinto/Prisioneiro.h"
#include "utils/Logger.h"
#include "estruturas/Novelo.h"
#include <iostream>
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


const std::vector<int>& Prisioneiro::getCaminho() const {
    return caminho;
}

int Prisioneiro::mover(const listaAdj<MeuPair<int, int>>& vizinhos) {
    auto no_vizinho = vizinhos.get_cabeca();

    if (kitsDeComida <= 0) {
        return 0;
    }

    while (no_vizinho != nullptr)
    {
        int proximo_vertice = no_vizinho->dado.primeiro;
        int peso_aresta = no_vizinho->dado.segundo;

        if (!foiVisitado(proximo_vertice)) {
            if (kitsDeComida < peso_aresta) {
                no_vizinho = no_vizinho->prox;
                continue;
            }
            novelo.criarRastro({pos, peso_aresta});
            pos = proximo_vertice;
            visitados[pos] = true;
            caminho.push_back(pos);
            kitsDeComida -= peso_aresta; 
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
            Logger::info(tempoPrisioneiro, "Não pode fazer backtracking para {} (peso: {}, kits restantes: {}) - Motivo: kits insuficientes.", Logger::LogSource::PRISIONEIRO, rastroAnterior.primeiro, rastroAnterior.segundo, kitsDeComida);
            return 0;
        }
    novelo.puxarRastro();
    int vertice_volta = rastroAnterior.primeiro;
    pos = vertice_volta;
    caminho.push_back(pos);
    kitsDeComida -= rastroAnterior.segundo; // Consome kits de comida
    Logger::info(tempoPrisioneiro, "Escolha: backtracking para {} (peso: {}, kits restantes: {}) - Motivo: todos vizinhos visitados, rastro disponível e kits suficientes.", Logger::LogSource::PRISIONEIRO, pos, rastroAnterior.segundo, kitsDeComida);
    registrarAcontecimento("BACKTRACK", "Voltou para sala anterior", pos, rastroAnterior.segundo);
    return rastroAnterior.segundo;
    }
    Logger::warning(tempoPrisioneiro, "Prisioneiro está engasgado, sem vizinhos e sem rastro!", Logger::LogSource::PRISIONEIRO);
    return 0;
}