/**
 * @file main.cpp
 * @author Thiago Nerton
 * @brief Ponto de entrada principal da aplicação de simulação.
 * @details Este arquivo é responsável por processar os argumentos da linha de comando,
 * inicializar o simulador, executar a simulação e imprimir o resultado final
 * no formato solicitado (relatório para humanos ou JSON).
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include "labirinto/Simulador.h"
#include "utils/Logger.h"

// Cores ANSI locais para relatórios humanos (mantém compatível com Logger)
// Evita depender de símbolos internos de Logger.
static constexpr const char* RESET_COLOR   = "\033[0m";
static constexpr const char* BOLD          = "\033[1m";
static constexpr const char* BRONZE        = "\033[38;5;172m";
static constexpr const char* STONE         = "\033[38;5;242m";
static constexpr const char* STONE_DARK    = "\033[38;5;238m";
static constexpr const char* GREEN_INFO    = "\033[1;32m";
static constexpr const char* RED_HIGHLIGHT = "\033[1;31m";

/**
 * @struct ConfiguracaoSimulacao
 * @brief Estrutura para manter as configurações da simulação.
 * @note Atualmente não utilizada, mas preparada para futuras expansões.
 */
struct ConfiguracaoSimulacao {
    unsigned int seed = 1; 
    int chanceDeSobrevivencia = 1; 
};

/**
 * @brief Função principal que executa o programa.
 * @details
 * 1.  Analisa os argumentos da linha de comando para obter o nome do arquivo do cenário
 * e o modo de saída (`--human` ou `--json-only`).
 * 2.  Configura o nível do Logger com base no modo de saída.
 * 3.  Cria uma instância do `Simulador`.
 * 4.  Carrega o arquivo de cenário.
 * 5.  Executa a simulação com uma seed e chance de batalha fixas.
 * 6.  Imprime o relatório final no formato apropriado.
 * @param argc O número de argumentos da linha de comando.
 * @param argv Um array de strings contendo os argumentos.
 * @return `0` em caso de sucesso, `1` em caso de erro.
 */
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo> [--json-only|--human] [--no-progress]" << std::endl;
        return 1;
    }
    // Nome do arquivo passado como argumento
    std::string nomeArquivo = argv[1];
    bool jsonOnly = false;
    bool humanReport = true; // padrão
    bool showProgress = true; // novo: controlar logs de progresso
    // Parseia múltiplas flags opcionais a partir do 2º argumento
    for (int i = 2; i < argc; ++i) {
        std::string flag = argv[i];
        if (flag == "--json-only") { jsonOnly = true; humanReport = false; }
        else if (flag == "--human") { humanReport = true; jsonOnly = false; }
        else if (flag == "--no-progress") { showProgress = false; }
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
    // --- LINHA DO TEMPO DETALHADA (ANTES DO RELATÓRIO FINAL) ---
    if (showProgress && !resultado.eventos.empty()) {
        std::cout << BRONZE << "\n  ⏳ " << BOLD << "Linha do Tempo dos Eventos" << RESET_COLOR << std::endl;
        std::string local;
        if (resultado.tempoEncontro >= 0) {
            if (resultado.tipoEncontro == "sala") {
                local = "Sala " + std::to_string(resultado.posFinalP);
            } else if (resultado.tipoEncontro == "aresta" && !resultado.eventos.empty()) {
                int u = -1, v = -1;
                for(const auto& ev : resultado.eventos) {
                    if (ev.agente == "Prisioneiro" && resultado.tempoEncontro >= ev.tempoInicio && resultado.tempoEncontro <= ev.tempoFim) {
                        u = ev.origem;
                        v = ev.destino;
                        break;
                    }
                }
                if (u != -1) local = "Corredor " + std::to_string(u) + "–" + std::to_string(v);
            }
        }
        // Passa tempoCorte = resultado.tempoReal para não imprimir progresso após o fim
        Logger::printarLogsComProgresso(resultado.eventos, resultado.tempoReal, resultado.tempoEncontro, resultado.tipoEncontro, local);
        std::cout << std::endl;
    }
    // --- CABEÇALHO DO RELATÓRIO ---
    std::cout << "\n\n";
    std::cout << "          " << BOLD << BRONZE << "╔═════════════════════════════════════════════╗" << RESET_COLOR << std::endl;
    std::cout << "          " << BOLD << BRONZE << "║" << RESET_COLOR << "       " << BOLD << "O FIM DA JORNADA NO LABIRINTO" << RESET_COLOR << "        " << BOLD << BRONZE << "║" << RESET_COLOR << std::endl;
    std::cout << "          " << BOLD << BRONZE << "╚═════════════════════════════════════════════╝" << RESET_COLOR << std::endl;
    std::cout << "\n";

    // --- DESFECHO ---
    std::cout << STONE << "  " << BOLD << "Desfecho: " << RESET_COLOR;
    if (resultado.prisioneiroSobreviveu) {
        std::cout << GREEN_INFO << "O Prisioneiro Escapou!" << RESET_COLOR << std::endl;
    } else {
        std::cout << RED_HIGHLIGHT << "O Prisioneiro Pereceu." << RESET_COLOR << std::endl;
    }
    std::cout << STONE << "  " << BOLD << "Motivo:   " << RESET_COLOR << resultado.motivoFim << std::endl;
    std::cout << "\n";
    
    // --- ESTATÍSTICAS DA JORNADA ---
    std::cout << BRONZE << "  ⚜ " << BOLD << "Estatísticas da Jornada" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "  ----------------------------------------------------" << RESET_COLOR << std::endl;
    std::cout << "  " << BOLD << "Tempo de Sobrevivência: " << RESET_COLOR << resultado.diasSobrevividos << " unidades de tempo" << std::endl;
    std::cout << "  " << BOLD << "Provisões Restantes:    " << RESET_COLOR << resultado.kitsRestantes << " kits de comida" << std::endl;
    std::cout << "  " << BOLD << "Posição Final (P):      " << RESET_COLOR << "Sala " << resultado.posFinalP << std::endl;
    std::cout << "  " << BOLD << "Posição Final (M):      " << RESET_COLOR << "Sala " << resultado.posFinalM << std::endl;
    std::cout << STONE_DARK << "  ----------------------------------------------------" << RESET_COLOR << std::endl;

    // --- DESTAQUES ---
    std::cout << BRONZE << "\n  ⚔ " << BOLD << "Destaques da Jornada" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "  ----------------------------------------------------" << RESET_COLOR << std::endl;
    if (resultado.tempoEncontro >= 0) {
        std::cout << "  " << BOLD << "Confronto Decisivo: " << RED_HIGHLIGHT << "Sim" << RESET_COLOR << std::endl;
        std::cout << "    " << BOLD << "Momento do Encontro: " << RESET_COLOR << std::fixed << std::setprecision(2) << resultado.tempoEncontro << " unidades de tempo" << std::endl;
        std::cout << "    " << BOLD << "Tipo de Encontro:    " << RESET_COLOR << "Em uma " << resultado.tipoEncontro << std::endl;
    } else {
        std::cout << "  " << BOLD << "Confronto Decisivo: " << GREEN_INFO << "Não" << RESET_COLOR << ". O Minotauro nunca encontrou sua presa." << std::endl;
    }
    std::cout << "  " << BOLD << "Destino do Minotauro: ";
    if (resultado.minotauroVivo) {
        std::cout << "Sobreviveu";
    } else {
        std::cout << RED_HIGHLIGHT << "Derrotado em combate" << RESET_COLOR;
    }
    std::cout << std::endl;
    std::cout << STONE_DARK << "  ----------------------------------------------------" << RESET_COLOR << std::endl;


    // --- CAMINHOS PERCORRIDOS ---
    auto print_caminho = [](const std::string& nome, const std::vector<int>& caminho) {
        std::cout << BRONZE << "\n  📜 " << BOLD << "O Rastro de " << nome << RESET_COLOR << std::endl;
        if (caminho.empty()) {
            std::cout << STONE << "     Nenhum passo significativo foi dado." << RESET_COLOR << std::endl;
            return;
        }
        std::cout << "     ";
        for (size_t i = 0; i < caminho.size(); ++i) {
            std::cout << BOLD << "Sala " << caminho[i] << RESET_COLOR;
            if (i < caminho.size() - 1) {
                std::cout << STONE << " → " << RESET_COLOR;
            }
        }
        std::cout << std::endl;
    };
    
    print_caminho("Prisioneiro", resultado.caminhoP);
    print_caminho("Minotauro", resultado.caminhoM);

    std::cout << "\n" << BOLD << BRONZE << "========================================================" << RESET_COLOR << std::endl;
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