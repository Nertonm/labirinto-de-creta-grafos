// src/main.cpp

#include <iostream>
#include <fstream>
#include <string>
#include "labirinto/Simulador.h"

struct ConfiguracaoSimulacao {
    unsigned int seed = 1; 
    int chanceDeSobrevivencia = 1; 
};

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo>" << std::endl;
        return 1;
    }
    // Nome do arquivo passado como argumento
    std::string nomeArquivo = argv[1];

    // Abre o arquivo para leitura
    std::ifstream file(nomeArquivo);

    // Verifica se o arquivo foi aberto com sucesso
    if (!file.is_open()) {
        std::cerr << "Erro: Não foi possível acessar o arquivo " << nomeArquivo << std::endl;
        return 1;
    }

    try {
        Simulador simulation;
        simulation.carregarArquivo(nomeArquivo);
        simulation.imprimirInicioSimulacao();
        simulation.run(1, 1);
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
        return 1;
    }

    file.close();
    return 0;
}