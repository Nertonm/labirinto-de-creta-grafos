/**
 * @file Logger.h
 * @author Thiago Nerton 
 * @version 2.0
 * @brief Sistema de log centralizado para a simulação.
 * @details Este logger é uma classe estática (singleton) que permite o registro
 * de eventos em diferentes níveis de severidade (DEBUG, INFO, WARNING, ERROR),
 * com formatação automática, cores e timestamps.
 */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <map>
#include <thread>
#include <chrono>
#include "labirinto/Grafo.h" // Necessário para imprimir os vizinhos

/**
 * @enum LogLevel
 * @brief Define os diferentes níveis de severidade para as mensagens de log.
 */
enum class LogLevel {
    DEBUG,   // Informações detalhadas para depuração
    INFO,    // Eventos importantes da simulação (movimentos, encontros)
    WARNING, // Situações inesperadas que não quebram a simulação
    ERROR    // Erros críticos que podem comprometer a simulação
};

/**
 * @class Logger
 * @brief Classe estática para gerenciar o registro de eventos da simulação.
 * @details Por ser estática, seus métodos podem ser chamados de qualquer lugar
 * do código sem a necessidade de criar uma instância. Ex: Logger::info(...);
 */
class Logger {
public:
    // Proíbe a criação de instâncias desta classe
    Logger() = delete;

    // Cores por agente

    enum class LogSource {
        PRISIONEIRO,
        MINOTAURO,
        OUTRO
    };

    inline static const char* PRISIONEIRO_COLOR = "\033[1;32m"; // Verde
    inline static const char* MINOTAURO_COLOR = "\033[38;5;94m"; // Azul escuro
    inline static const char* DEFAULT_AGENT_COLOR = "";

    /**
     * @struct EventoMovimento
     * @brief Armazena os detalhes de um único movimento de um agente para o log animado.
     */
    struct EventoMovimento {
        double tempoInicio;
        double tempoFim;
        std::string agente;
        int origem;
        int destino;
        int peso;
    };

    /**
     * @struct SimulacaoInfo
     * @brief Agrupa todos os dados necessários para imprimir o cabeçalho inicial da simulação.
     */
    struct SimulacaoInfo {
        int vEntrada;
        int vSaida;
        int posMinotauro;
        int kitsComida;
        int numVertices;
        int numArestas;
        const Grafo* labirinto; // Ponteiro para o labirinto para listar as conexões
    };    

    /**
     * @brief Define o nível mínimo de log a ser exibido.
     * @details Mensagens com nível de severidade menor que o definido serão ignoradas.
     * Ex: se o nível for INFO, mensagens de DEBUG não serão mostradas.
     * @param level O nível mínimo de log a ser exibido.
     */
    static void setLevel(LogLevel level) {
        currentLevel = level;
    }

    /**
     * @brief Registra uma mensagem de log de depuração.
     * @tparam Args Tipos dos argumentos a serem formatados na mensagem.
     * @param simulationTime O tempo atual da simulação.
     * @param message A mensagem de log, com placeholders {} para formatação.
     * @param args Os valores que substituirão os placeholders na mensagem.
     */

    template<typename... Args>
    static void debug(double simulationTime, const std::string& message, LogSource source = LogSource::OUTRO, Args... args) {
        log(LogLevel::DEBUG, simulationTime, message, source, args...);
    }

    /**
     * @brief Registra uma mensagem de log de informação.
     */

    template<typename... Args>
    static void info(double simulationTime, const std::string& message, LogSource source = LogSource::OUTRO, Args... args) {
        log(LogLevel::INFO, simulationTime, message, source, args...);
    }

    /**
     * @brief Registra uma mensagem de log de aviso.
     */

    template<typename... Args>
    static void warning(double simulationTime, const std::string& message, LogSource source = LogSource::OUTRO, Args... args) {
        log(LogLevel::WARNING, simulationTime, message, source, args...);
    }

    /**
     * @brief Registra uma mensagem de log de erro.
     */

    template<typename... Args>
    static void error(double simulationTime, const std::string& message, LogSource source = LogSource::OUTRO, Args... args) {
        log(LogLevel::ERROR, simulationTime, message, source, args...);
    }


    /**
     * @brief Imprime um cabeçalho estilizado e o estado inicial da simulação.
     * @param info Uma estrutura contendo todos os dados necessários para o relatório.
     */
    static void imprimirInicioSimulacao(const SimulacaoInfo& info);

    /**
     * @brief Exibe um log animado de todos os eventos de movimento da simulação.
     * @details Esta função exibe o progresso em tempo real, usando pausas para
     * criar um efeito de animação. Deve ser chamada no final da simulação.
     * @param eventos Um vetor contendo todos os movimentos que ocorreram.
     */
    static void printarLogsComProgresso(const std::vector<EventoMovimento>& eventos);

private:
    /**
     * @brief Formata e imprime a mensagem de log se o nível for apropriado.
     * @details Esta é a função central que todas as outras chamam. Ela formata
     * a string, adiciona cores e prefixos, e a imprime no console.
     * @param agente Nome do agente ("Prisioneiro", "Minotauro" ou vazio)
     */
    template<typename... Args>
    static void log(LogLevel level, double simulationTime, const std::string& message, LogSource source, Args... args) {
        if (level < currentLevel) {
            return; // Ignora a mensagem se o nível for muito baixo
        }

        constexpr int timeWidth = 7;
        constexpr int levelWidth = 5;
        const char* agentColor = DEFAULT_AGENT_COLOR;
        switch (source) {
            case LogSource::PRISIONEIRO:
                agentColor = PRISIONEIRO_COLOR;
                break;
            case LogSource::MINOTAURO:
                agentColor = MINOTAURO_COLOR;
                break;
            default:
                agentColor = DEFAULT_AGENT_COLOR;
        }

        std::cout << agentColor
                  << std::left << std::setw(timeWidth+3) << ("[" + format_time(simulationTime) + "]")
                  << "[" << std::left << std::setw(levelWidth) << levelStrings.at(level) << "] "
                  << format(message, args...)
                  << RESET_COLOR << std::endl;
    }

    // Formata o tempo para sempre mostrar 2 casas decimais, alinhado à esquerda
    static std::string format_time(double t) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << t;
        return oss.str();
    }

    /**
     * @brief Substitui os placeholders {} na mensagem pelos argumentos fornecidos.
     * @details Uma função auxiliar para permitir uma formatação similar a de outras linguagens.
     */
    template<typename... Args>
    static std::string format(const std::string& fmt, Args... args) {
        std::stringstream ss;
        format_internal(ss, fmt, args...);
        return ss.str();
    }

    // Função base para a recursão do format
    static void format_internal(std::stringstream& ss, const std::string& fmt) {
        ss << fmt;
    }

    // Função recursiva para processar os argumentos
    template<typename T, typename... Args>
    static void format_internal(std::stringstream& ss, std::string fmt, T value, Args... args) {
        size_t pos = fmt.find("{}");
        if (pos != std::string::npos) {
            ss << fmt.substr(0, pos) << value;
            fmt = fmt.substr(pos + 2);
        }
        format_internal(ss, fmt, args...);
    }

    // --- Atributos Estáticos ---
    inline static LogLevel currentLevel = LogLevel::DEBUG; // Nível padrão

    // Cores para o console
    inline static const char* RESET_COLOR = "\033[0m";
    inline static const std::map<LogLevel, const char*> levelColors = {
        {LogLevel::DEBUG,   "\033[38;5;242m"}, // Cinza
        {LogLevel::INFO,    "\033[1;37m"},    // Branco
        {LogLevel::WARNING, "\033[1;33m"},    // Amarelo
        {LogLevel::ERROR,   "\033[1;31m"}     // Vermelho
    };

    // Strings para os níveis
    inline static const std::map<LogLevel, std::string> levelStrings = {
        {LogLevel::DEBUG,   "DEBUG"},
        {LogLevel::INFO,    "INFO"},
        {LogLevel::WARNING, "WARN"},
        {LogLevel::ERROR,   "ERROR"}
    };


    inline static const char* BRONZE = "\033[38;5;172m";
    inline static const char* STONE_DARK = "\033[38;5;238m";
    inline static const char* MINOTAUR_COLOR = "\033[38;5;94m";
    inline static const char* RED_HIGHLIGHT = "\033[1;31m";
    inline static const char* GREEN_INFO = "\033[1;32m";
    inline static const char* GREEN = "\033[1;32m";
    inline static const char* STONE = "\033[38;5;242m";
    inline static const char* BOLD = "\033[1m";



};

// Implementação das funções fora da classe
inline void Logger::imprimirInicioSimulacao(const Logger::SimulacaoInfo& info) {
    // Título Principal
    std::cout << BRONZE << BOLD << "\n     ╔═════════════════════════════════════════════╗" << RESET_COLOR << std::endl;
    std::cout << BRONZE << BOLD << "     ║" << RESET_COLOR << "       " << BRONZE << BOLD << "LABIRINTO DE CRETA - SIMULAÇÃO" << RESET_COLOR << "        " << BRONZE << BOLD << "║" << RESET_COLOR << std::endl;
    std::cout << BRONZE << BOLD << "     ╚═════════════════════════════════════════════╝" << RESET_COLOR << std::endl;

    // --- Arte ASCII do Minotauro (Fornecida pelo usuário e colorizada) ---
    std::cout << STONE_DARK << "      _____________ " << BRONZE << "nnnnnnnn nnnnnnnn" << STONE_DARK << " ___________" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "     |             " << BRONZE << "nnnnnnnn    nn" << STONE_DARK << "               |" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "     | -\"\"\\        " << BRONZE << "HHnnnnHP    HH" << STONE_DARK << "               |" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "    .-\"  .`)     (" << MINOTAUR_COLOR << "                              " << STONE_DARK << "|" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "   j   .'_+     :" << MINOTAUR_COLOR << "[                )      " << STONE_DARK << ".^--.. |" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "  i    -\"       " << MINOTAUR_COLOR << "|l                ].    /      i" << STONE_DARK << "|" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << " ,\" .:j         " << MINOTAUR_COLOR << "`8o  _,,+.,.--,   d" << STONE_DARK << "|   `:::;    b" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << " i  :'|          " << MINOTAUR_COLOR << "\"88p;.  (" << RED_HIGHLIGHT << "-.\"_\"-" << MINOTAUR_COLOR << ".oP" << MINOTAUR_COLOR << "        \\.   :" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << " ; .  (            " << MINOTAUR_COLOR << ">,%%%   " << RED_HIGHLIGHT << "f" << MINOTAUR_COLOR << "),):8\"" << MINOTAUR_COLOR << "          \\:'  i" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "i  :: j          " << MINOTAUR_COLOR << ",;%%%:; ; ; i:%%%.," << MINOTAUR_COLOR << "        i.   `." << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "i  `: ( ____  " << MINOTAUR_COLOR << ",-::::::' ::j  [:```" << MINOTAUR_COLOR << "          [8:   )" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "<  ..``'::::" << MINOTAUR_COLOR << "8888oooooo.  :(jj(,;,,," << MINOTAUR_COLOR << "         [8::  <" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "`. ``:.      " << MINOTAUR_COLOR << "oo.8888888888:;%%%8o.::.+888+o.:`:'" << STONE_DARK << "  |" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << " `.   `        " << MINOTAUR_COLOR << "`o`88888888b`%%%%%88< Y888P\"\"'-" << STONE_DARK << "    ;" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "   \"`---`.       " << MINOTAUR_COLOR << "Y`888888888;;.,\"888b.\"\"..::::'" << STONE_DARK << "-'" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "     |    \"-....  " << MINOTAUR_COLOR << "b`8888888:::::.`8888._::-" << STONE_DARK << "     |" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "     |       `:::. " << MINOTAUR_COLOR << "`:::::O:::::::.`%%'" << STONE_DARK << "|         |" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "     |        `.      " << MINOTAUR_COLOR << "\"``::::::''" << STONE_DARK << "    .'         |" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "     |          `.                   <          |" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "     |            +:         `:   -';           |" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "     |  ____       `:         : .::/            |" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "     | `-|--        ;+_  :::. :..;;;            |" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "     |______________;;;;,;;;;;;;;,;;____________|" << RESET_COLOR << std::endl;

    // --- Seção de Informações ---
    std::cout << BRONZE << BOLD << "\n            ⚜ " << BOLD << "O DESTINO DO PRISIONEIRO" << RESET_COLOR << BRONZE << BOLD << " ⚜" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "  ╔════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << STONE_DARK << "  ║ " << RESET_COLOR << "Ponto de Partida (Entrada):      Sala " << BOLD << info.vEntrada << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "  ║ " << GREEN_INFO << "Vislumbre de Liberdade (Saída):  Sala " << BOLD << info.vSaida << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "  ║────────────────────────────────────────────────────────║" << std::endl;
    std::cout << STONE_DARK << "  ║ " << RED_HIGHLIGHT << "Covil da Fera (Minotauro):       Sala " << BOLD << info.posMinotauro << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "  ║ " << RESET_COLOR << "Provisões para a Jornada:      " << BOLD << info.kitsComida << " kits" << RESET_COLOR << std::endl;
    std::cout << STONE_DARK << "  ╚════════════════════════════════════════════════════════╝" << RESET_COLOR << std::endl;

    // --- Seção de Geração: "Os Corredores de Pedra" ---
    std::cout << BRONZE << "\n            🏛  " << BOLD << "Os Corredores de Pedra" << RESET_COLOR << BRONZE << " 🏛" << RESET_COLOR << std::endl;
    std::cout << GREEN << "  Gerando uma estrutura com " << BOLD << info.numVertices << RESET_COLOR << GREEN << " salas e " << BOLD << info.numArestas << RESET_COLOR << GREEN << " caminhos..." << RESET_COLOR << std::endl;
    std::cout << STONE <<  "  ╔════════════════════════════════════════════════════════╗" << std::endl;

    for (int v = 0; v < info.labirinto->getNumVertices(); ++v) {
        const auto& vizinhos = info.labirinto->get_vizinhos(v);
        std::cout << STONE << "  ║ " << BRONZE << "• Sala " << BOLD << v << RESET_COLOR << STONE << " se conecta com:" << RESET_COLOR << std::endl;
        auto no = vizinhos.get_cabeca();
        if (no == nullptr) {
            std::cout << STONE << "  ║    └── Vazio e Silêncio..." << std::endl;
        } else {
            while (no != nullptr) {
                const char* connector = (no->prox == nullptr) ? "└──" : "├──";
                std::cout << STONE << "  ║    " << connector << " Sala " << BOLD << no->dado.primeiro << RESET_COLOR 
                          << STONE << " (distância: " << no->dado.segundo << ")" << RESET_COLOR << std::endl;
                no = no->prox;
            }
        }
        if (v < info.labirinto->getNumVertices() - 1) {
             std::cout << STONE << "  ║╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌║" << std::endl;
        }
    }
    std::cout << STONE <<  "  ╚════════════════════════════════════════════════════════╝" << RESET_COLOR << std::endl;
    std::cout << "\n" << BRONZE << BOLD << "Que os deuses guiem seus passos. A simulação começou!" << RESET_COLOR << std::endl;
}

inline void Logger::printarLogsComProgresso(const std::vector<EventoMovimento>& eventos) {
    std::map<double, std::vector<const EventoMovimento*>> eventosPorTempo;
    for (const auto& ev : eventos) {
        eventosPorTempo[ev.tempoInicio].push_back(&ev);
    }
    double tempoGlobal = 0.0;
    for (const auto& [tempo, grupo] : eventosPorTempo) {
        tempoGlobal = tempo;
        std::cout << std::fixed << std::setprecision(2);
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        for (const auto* ev : grupo) {
            std::string cor = (ev->agente == "Minotauro") ? "\033[38;5;94m" : "\033[1;32m";
            std::cout << cor << "[TEMPO " << tempoGlobal << "] " << ev->agente << " começou a ir de " << ev->origem << " para " << ev->destino << " (peso: " << ev->peso << ")" << "\033[0m" << std::endl;
            std::cout << "    Detalhes: origem=" << ev->origem << ", destino=" << ev->destino << ", peso=" << ev->peso << std::endl;
        }
        int barraLen = 20;
        for (int p = 0; p <= barraLen; ++p) {
            double t = tempoGlobal + (grupo[0]->tempoFim-grupo[0]->tempoInicio) * (double(p)/barraLen);
            int porcento = int(100.0 * p / barraLen);
            for (const auto* ev : grupo) {
                std::string cor = (ev->agente == "Minotauro") ? "\033[38;5;94m" : "\033[1;32m";
                std::cout << cor << "[TEMPO " << t << "] " << ev->agente << " progresso: [";
                for (int b = 0; b < barraLen; ++b) std::cout << (b < p ? '#' : '-');
                std::cout << "] " << porcento << "%\033[0m" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        for (const auto* ev : grupo) {
            std::string cor = (ev->agente == "Minotauro") ? "\033[38;5;94m" : "\033[1;32m";
            std::cout << cor << "[TEMPO " << ev->tempoFim << "] " << ev->agente << " chegou em " << ev->destino << "\033[0m" << std::endl;
            for (const auto* outro : grupo) {
                if (ev->agente != outro->agente && ev->destino == outro->destino) {
                    std::cout << "\n\033[1;31m";
                    std::cout << "════════════════════════════════════════════════════════════\n";
                    std::cout << "*** ENCONTRO! Minotauro encontrou o Prisioneiro na sala " << ev->destino << "! ***\n";
                    std::cout << "Motivo: Ambos chegaram na mesma sala ao mesmo tempo.\n";
                    std::cout << "Estado no momento do encontro:\n";
                    std::cout << "  - Minotauro: posição=" << ev->destino << ", tempo=" << ev->tempoFim << "\n";
                    std::cout << "  - Prisioneiro: posição=" << outro->destino << ", tempo=" << outro->tempoFim << "\n";
                    std::cout << "════════════════════════════════════════════════════════════\n";
                    std::cout << "\033[0m\n";
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
    }
}