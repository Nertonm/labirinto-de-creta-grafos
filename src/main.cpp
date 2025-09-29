// src/main.cpp

#include <iostream>
#include <fstream>
#include <string>
#include "labirinto/Simulador.h"

int main(int argc, char* argv[]) {
    // Para rodar: ./simulador data/labirinto_01
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo>" << std::endl;
        return 1;
    }
    // Nome do arquivo passado como argumento
    std::string nome_arquivo = argv[1];

    // Abre o arquivo para leitura
    std::ifstream file(nome_arquivo);

    // Verifica se o arquivo foi aberto com sucesso
    if (!file.is_open()) {
        std::cerr << "Erro: Não foi possível abrir o arquivo " << nome_arquivo << std::endl;
        return 1;
    }

    try {
        Simulador simulation(file);
        simulation.testa_movimento_prisioneiro();
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
        return 1;
    }

    file.close();
    return 0;
}