#pragma once

#include <vector>
#include <unordered_set>

// Declaração antecipada da classe Grafo para evitar inclusão circular
class Grafo;

class Prisioneiro {
public:
    // Construtor que inicializa o prisioneiro com sua posição inicial e o tempo máximo sem comida
    Prisioneiro(int posicao_inicial, int tempo_maximo_comida);
    
    // Destrutor
    ~Prisioneiro();

    // Método para mover o prisioneiro para um novo vértice
    void mover(int novo_vertice);
    
}