#include "labirinto/Simulador.h"
#include <string>
#include <sstream> // Essencial para o std::stringstream
#include <iostream>

Simulador::Simulador(std::ifstream& arquivo_entrada) {
    std::string linha;
    int num_arestas, num_vertices;
    
    // Lê a primeira linha: número de vértices
    if (std::getline(arquivo_entrada, linha)) {
        std::stringstream ss(linha);
        ss >> num_vertices;
    } else {
        throw std::runtime_error("Erro ao ler o número de vértices.");
    }

    // Lê a segunda linha: número de arestas
    if (std::getline(arquivo_entrada, linha)) {
        std::stringstream ss(linha);
        ss >> num_arestas;
    } else {
        throw std::runtime_error("Erro ao ler o número de arestas.");
    }

    std::cout << "Configurando labirinto com " << num_vertices << " vertices e " << num_arestas << " arestas." << std::endl;

    // Lê as próximas 'num_arestas' linhas: arestas do grafo
    for (int i = 0; i < num_arestas; ++i) {
        if (std::getline(arquivo_entrada, linha)) {
            std::stringstream ss(linha);
            int u, v, peso;
            ss >> u >> v >> peso;
            labirinto.adicionar_aresta(u, v, peso);
        } else {
            throw std::runtime_error("Erro ao ler as arestas do grafo.");
        }
    }

    // Lê Vértice de entrada
    std::getline(arquivo_entrada, linha);
    std::stringstream(linha) >> vertice_entrada;
    std::cout << "Vértice de entrada: " << vertice_entrada << std::endl;

    // Lê Vértice de saída
    std::getline(arquivo_entrada, linha);
    std::stringstream(linha) >> vertice_saida;
    labirinto.set_saida(vertice_saida); 

    // Lê Posição inicial do Minotauro
    std::getline(arquivo_entrada, linha);
    std::stringstream(linha) >> pos_inicial_minotauro;

    // Lê Parâmetro de percepção
    std::getline(arquivo_entrada, linha);
    std::stringstream(linha) >> percepcao_minotauro;

    // Lê Tempo máximo
    std::getline(arquivo_entrada, linha);
    std::stringstream(linha) >> tempo_maximo_comida;

    std::cout << "Simulador iniciado com sucesso!" << std::endl;
}