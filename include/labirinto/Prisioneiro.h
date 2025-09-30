#pragma once

#include <vector>
#include "estruturas/MinhaListaAdj.h"
#include "estruturas/MeuPair.h"
#include "estruturas/Novelo.h"

// Declaração antecipada da classe Grafo para evitar inclusão circular
class Grafo;

class Prisioneiro {
public:
    // Construtor que inicializa o prisioneiro com sua posição inicial e o tempo máximo sem comida
    Prisioneiro(int posInicial, int supDias);
    
    // Destrutor
    ~Prisioneiro();

    // Método para mover o prisioneiro para um novo vértice
    void mover(const listaAdj<MeuPair<int, int>>& vizinhos,  int& tempo_restante);

    int getPosAtual() const;
    const std::vector<int>& getCaminho() const;

private:
    int posAtual; // Vértice atual do prisioneiro
    Novelo<MeuPair<int, int>> novelo; // Simula o fio de lã para o backtracking
    std::vector<int> caminho; // Histórico dos vértices visitados
    std::vector<bool> visitados; // Marca os vértices já visitados
};