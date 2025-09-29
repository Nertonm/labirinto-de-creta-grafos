#pragma once

#include <vector>
#include <unordered_set>
#include "estrutras/MinhaListaAdj.h"
#include "estrutras/MeuPair.h"
#include "estrutras/MinhaPilha.h"

// Declaração antecipada da classe Grafo para evitar inclusão circular
class Grafo;

class Prisioneiro {
public:
    // Construtor que inicializa o prisioneiro com sua posição inicial e o tempo máximo sem comida
    Prisioneiro(int posicao_inicial, int tempo_maximo_comida);
    
    // Destrutor
    ~Prisioneiro();

    // Método para mover o prisioneiro para um novo vértice
    void mover(const MinhaListaAdj<MeuPair<int, int>>& vizinhos,  int& tempo_restante);
    
    int get_posicao_atual() const;
    const std::vector<int>& get_caminho_percorrido() const;

private:
    int posicao_atual; // Vértice atual do prisioneiro
    MinhaPilha<MeuPair<int, int>> novelo_de_la; // Simula o fio de lã para o backtracking
    std::vector<int> caminho_percorrido; // Histórico dos vértices visitados
    std::vector<bool> visitados; // Marca os vértices já visitados
};