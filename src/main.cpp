// src/main.cpp

#include <iostream>
#include <fstream>
#include <string>
#include "labirinto/Simulador.h"
#include "utils/Logger.h"

struct ConfiguracaoSimulacao {
    unsigned int seed = 1; 
    int chanceDeSobrevivencia = 1; 
};

int main(int argc, char* argv[]) {
    Logger::info(0.0, "[TESTE] Logger está funcionando!", Logger::LogSource::OUTRO);

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
    Logger::error(0.0, "Erro: Não foi possível acessar o arquivo {}", Logger::LogSource::OUTRO, nomeArquivo);
        return 1;
    }

    try {
        Simulador simulation;
        if (!simulation.carregarArquivo(nomeArquivo)) {
            return 1; // Encerra se o arquivo não puder ser carregado
        }

        Simulador::ResultadoSimulacao resultado = simulation.run(1, 1); // Seed 1, Chance 1%

        // MELHORIA: Relatório final detalhado
        std::cout << "\n\n";
        std::cout << "======================================================\n";
        std::cout << "             RELATÓRIO FINAL DA SIMULAÇÃO             \n";
        std::cout << "======================================================\n";
        std::cout << "  Resultado: " << (resultado.prisioneiroSobreviveu ? "Sim" : "Não");
        std::cout << "  Motivo: " << resultado.motivoFim << "\n";
        std::cout << "------------------------------------------------------\n";
        std::cout << "  Tempo total de sobrevivência: " << resultado.diasSobrevividos << " unidades de tempo\n";
        std::cout << "  Kits de comida restantes: " << resultado.kitsRestantes << "\n";
        std::cout << "  Posição final do Prisioneiro: Sala " << resultado.posFinalP << "\n";
        std::cout << "  Posição final do Minotauro: Sala " << resultado.posFinalM << "\n";
        std::cout << "  Minotauro sobreviveu: " << (resultado.minotauroVivo ? "Sim" : "Não") << "\n";
        std::cout << "------------------------------------------------------\n";
        std::cout << "======================================================\n";
    } catch (const std::exception& e) {
    Logger::error(0.0, "Uma exceção crítica ocorreu: {}", Logger::LogSource::OUTRO, e.what());
        return 1;
    }

    file.close();
    return 0;
}