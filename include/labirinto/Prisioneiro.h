#pragma once

#include <vector>
#include "estruturas/MinhaListaAdj.h"
#include "estruturas/MeuPair.h"
#include "estruturas/Novelo.h"

// Declaração antecipada da classe Grafo para evitar inclusão circular
class Grafo;

class Prisioneiro {
public:
    struct HistoricoPrisioneiro {
        std::string tipo;
        std::string descricao;
        int posicao;
        int tempo;
    };

    struct TamanhoDoNovelo {
    static const int tamanhoNovelo = 1000;
    };

    // Construtor que inicializa o prisioneiro com sua posição inicial e o tempo máximo sem comida
    Prisioneiro(int posInicial, int supDias);

    // Adiciona um acontecimento ao histórico
    void registrarAcontecimento(const std::string& tipo, const std::string& descricao, int posicao, int tempo);

    // Imprime todos os acontecimentos do histórico
    void imprimirHistorico() const;
    
    // Destrutor
    ~Prisioneiro();

    // Método para mover o prisioneiro para um novo vértice
    int mover(const listaAdj<MeuPair<int, int>>& vizinhos);

    int getPos() const;
    const std::vector<int>& getCaminho() const;
    int getKitsDeComida() const;
    bool foiVisitado(int vertice) const; // Método público adicionado
    int voltarAtras();

private:
    int pos; // Vértice atual do prisioneiro
    int kitsDeComida; // Número de kits de comida disponíveis
    Novelo<MeuPair<int, int>> novelo; // Simula o fio de lã para o backtracking
    std::vector<int> caminho; // Histórico dos vértices visitados
    std::vector<HistoricoPrisioneiro> historico; // Histórico detalhado dos acontecimentos
    std::vector<bool> visitados; // Marca os vértices já visitados
};