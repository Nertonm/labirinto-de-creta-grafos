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
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo> [--json-only | --human]" << std::endl;
        return 1;
    }
    // Nome do arquivo passado como argumento
    std::string nomeArquivo = argv[1];
    bool jsonOnly = false;
    bool humanReport = true; // padrão
    if (argc >= 3) {
        std::string flag = argv[2];
        if (flag == "--json-only") { jsonOnly = true; humanReport = false; }
        else if (flag == "--human") { humanReport = true; jsonOnly = false; }
    }

    // Definir nível de log conforme modo selecionado antes de qualquer log
    if (jsonOnly) {
        Logger::setLevel(LogLevel::ERROR); // suprime INFO/WARN/DEBUG em json-only
    } else {
        Logger::setLevel(LogLevel::DEBUG);
        Logger::info(0.0, "[TESTE] Logger está funcionando!", Logger::LogSource::OUTRO);
    }

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
        // Imprimir cabeçalho estilizado apenas em modo humano
        if (humanReport && !jsonOnly) {
            Logger::imprimirInicioSimulacao(simulation.getSimulacaoInfo());
        }

        Simulador::ResultadoSimulacao resultado = simulation.run(1, 1); // Seed 1, Chance 1%

    if (humanReport && !jsonOnly) {
            // RELATÓRIO HUMANO COM DESTAQUE
            std::cout << "\n\n";
            std::cout << "======================================================\n";
            std::cout << "             RELATÓRIO FINAL DA SIMULAÇÃO             \n";
            std::cout << "======================================================\n";
            std::cout << "  Resultado: " << (resultado.prisioneiroSobreviveu ? "Sim" : "Não")
                      << "  Motivo: " << (resultado.motivoFim.empty() ? (resultado.prisioneiroSobreviveu ? "O prisioneiro escapou com sucesso!" : "") : resultado.motivoFim) << "\n";
            std::cout << "------------------------------------------------------\n";
            std::cout << "  Tempo total de sobrevivência: " << resultado.diasSobrevividos << " unidades de tempo\n";
            std::cout << "  Kits de comida restantes: " << resultado.kitsRestantes << "\n";
            std::cout << "  Minotauro sobreviveu: " << (resultado.minotauroVivo ? "Sim" : "Não") << "\n";
            if (resultado.tempoEncontro >= 0) {
                std::cout << "  Encontro: sim (tipo: " << resultado.tipoEncontro << ", t=" << std::fixed << std::setprecision(2) << resultado.tempoEncontro << ")\n";
            } else {
                std::cout << "  Encontro: não\n";
            }
            std::cout << "------------------------------------------------------\n";
            // Caminhos
            std::cout << "  Caminho do Prisioneiro: ";
            for (size_t i = 0; i < resultado.caminhoP.size(); ++i) {
                std::cout << resultado.caminhoP[i];
                if (i + 1 < resultado.caminhoP.size()) std::cout << " -> ";
            }
            std::cout << "\n";
            std::cout << "  Caminho do Minotauro: ";
            for (size_t i = 0; i < resultado.caminhoM.size(); ++i) {
                std::cout << resultado.caminhoM[i];
                if (i + 1 < resultado.caminhoM.size()) std::cout << " -> ";
            }
            std::cout << "\n";
            std::cout << "------------------------------------------------------\n";
            // Linha do tempo com barras e destaque de encontro
            if (!resultado.eventos.empty()) {
                std::string local;
                if (resultado.tempoEncontro >= 0) {
                    if (resultado.tipoEncontro == "sala") {
                        local = "Sala " + std::to_string(resultado.posFinalP);
                    } else if (resultado.tipoEncontro == "aresta" && !resultado.eventos.empty()) {
                        // usar último par de movimentos em conflito como contexto simples
                        // nota: para precisão, o simulador poderia preencher explicitamente origem/destino do encontro
                        int u = resultado.eventos.back().origem;
                        int v = resultado.eventos.back().destino;
                        local = "Corredor " + std::to_string(u) + "–" + std::to_string(v);
                    }
                }
                Logger::printarLogsComProgresso(resultado.eventos, resultado.tempoEncontro, resultado.tipoEncontro, local);
            }
            std::cout << "======================================================\n";
        }

        // JSON resumido (somente se solicitado explicitamente ou padrão se jsonOnly)
    if (jsonOnly) {
        std::cout << "{\n" 
                  << "  \"sobreviveu\": " << (resultado.prisioneiroSobreviveu ? "true" : "false") << ",\n"
            << "  \"tempo\": " << resultado.diasSobrevividos << ",\n"
            << "  \"tempoReal\": " << std::fixed << std::setprecision(6) << resultado.tempoReal << ",\n"
                  << "  \"kits\": " << resultado.kitsRestantes << ",\n"
                  << "  \"posP\": " << resultado.posFinalP << ",\n"
                  << "  \"posM\": " << resultado.posFinalM << ",\n"
                  << "  \"minotauroVivo\": " << (resultado.minotauroVivo ? "true" : "false") << ",\n"
                  << "  \"encontro\": { \"ok\": " << (resultado.tempoEncontro >= 0 ? "true" : "false")
                  << ", \"tipo\": \"" << (resultado.tempoEncontro >= 0 ? resultado.tipoEncontro : "") << "\", \"t\": "
                  << (resultado.tempoEncontro >= 0 ? resultado.tempoEncontro : -1) << " }\n";
            std::cout << "}\n";
        }
    } catch (const std::exception& e) {
    Logger::error(0.0, "Uma exceção crítica ocorreu: {}", Logger::LogSource::OUTRO, e.what());
        return 1;
    }

    file.close();
    return 0;
}