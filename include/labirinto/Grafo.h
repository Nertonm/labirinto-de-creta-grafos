// include/labirinto/Grafo.h

#pragma once // Diretiva padrão para evitar que o header seja incluído múltiplas vezes

#include <vector>
#include <unordered_map>

#include "estruturas/MinhaListaAdj.h"
#include "estruturas/MeuPair.h"
#include <utility> // Para std::pair
#include <fstream> // Para std::ifstream

class Grafo {
public:
    int getNumVertices() const { return nV; }
    // Construtor que inicializa o grafo a partir de um arquivo
    Grafo();
    // Destrutor 
    ~Grafo();

    // Método para adicionar uma aresta ao grafo
    void adicionar_aresta(int u, int v, int peso);
    void set_saida(int vSaida);
    int get_saida() const;

    const listaAdj<MeuPair<int, int>>& get_vizinhos(int vertice) const;
    std::vector<int> minCaminhosCalc(int origem, int destino) const;

private:
    std::unordered_map<int, listaAdj<MeuPair<int, int>>> adjacencias;
    int vSaida;
    int nV;
    int nA;
};
