#include <iomanip>
#include <queue>
#include <thread>
#include <chrono>
#include "labirinto/Prisioneiro.h"
#include <map>
#include "estruturas/MinhaListaAdj.h"
#include "estruturas/MeuPair.h"
#include "labirinto/Simulador.h"
#include <string>
#include <sstream> // Essencial para o std::stringstream
#include <iostream>
#include <random> // Para geração de números aleatórios

struct EventoMovimento {
    double tempoInicio;
    double tempoFim;
    std::string agente;
    int origem;
    int destino;
    int peso;
};

void printarLogsComProgresso(const std::vector<EventoMovimento>& eventos) {
    // Agrupa eventos por tempoInicio
    std::map<double, std::vector<const EventoMovimento*>> eventosPorTempo;
    for (const auto& ev : eventos) {
        eventosPorTempo[ev.tempoInicio].push_back(&ev);
    }
    double tempoGlobal = 0.0;
    for (const auto& [tempo, grupo] : eventosPorTempo) {
        tempoGlobal = tempo;
        std::cout << std::fixed << std::setprecision(2);
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        // Exibe todos os eventos que começam neste tempo
        for (const auto* ev : grupo) {
            std::string cor = (ev->agente == "Minotauro") ? "\033[38;5;94m" : "\033[1;32m";
            std::cout << cor << "[TEMPO " << tempoGlobal << "] " << ev->agente << " começou a ir de " << ev->origem << " para " << ev->destino << " (peso: " << ev->peso << ")" << "\033[0m" << std::endl;
            // Detalhes extras
            std::cout << "    Detalhes: origem=" << ev->origem << ", destino=" << ev->destino << ", peso=" << ev->peso << std::endl;
        }
        // Simula progresso para cada evento
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
        // Exibe chegada e destaca encontro
        for (const auto* ev : grupo) {
            std::string cor = (ev->agente == "Minotauro") ? "\033[38;5;94m" : "\033[1;32m";
            std::cout << cor << "[TEMPO " << ev->tempoFim << "] " << ev->agente << " chegou em " << ev->destino << "\033[0m" << std::endl;
            // Encontro especial
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
                    // ASCII art opcional
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
    }
}

Simulador::Simulador() = default;

bool Simulador::carregarArquivo(const std::string& nomeArquivo) {
    std::ifstream arquivoEntrada(nomeArquivo);

    if (!arquivoEntrada.is_open()) {
        std::cerr << "Erro: Não foi possível abrir o arquivo " << nomeArquivo << std::endl;
        return false;
    }
    std::string linha;

    auto lerValor = [&](int& valor) {
        if (std::getline(arquivoEntrada, linha)) {
            std::stringstream(linha) >> valor;
            return true;
        } 
        return false;
    };

    if (!lerValor(nV) || !lerValor(nA)) {
        std::cerr << "Erro ao ler dimensões do labirinto." << std::endl;
        return false;
    }

    // Inicializa nV e nA no objeto Grafo
    labirinto.setNumVertices(nV);
    labirinto.setNumArestas(nA);

    for (int i = 0; i < nA; ++i) {
        if (std::getline(arquivoEntrada, linha)) {
            std::stringstream ss(linha);
            int u, v, peso;
            ss >> u >> v >> peso;
            labirinto.adicionar_aresta(u, v, peso); // Adiciona apenas uma vez, pois o método já adiciona nos dois sentidos
        } else {
            std::cerr << "Erro ao ler as arestas." << std::endl;
            return false;
        }
    }

    int vSaid;
    if (!lerValor(vEntr) || !lerValor(vSaid) || !lerValor(posIniM) || !lerValor(percepcaoMinotauro) || !lerValor(kitsDeComida)) {
        std::cerr << "Erro ao ler parâmetros da simulação." << std::endl;
        return false;
    }


    labirinto.set_saida(vSaid);
    return true;
}

bool Simulador::prisioneiroBatalha(unsigned int seed, int chanceBatalha, std::mt19937& gerador) {
    std::uniform_int_distribution<int> dist(1, 100);
    int sorte = dist(gerador);
    return sorte <= chanceBatalha;
}


void Simulador::imprimirInicioSimulacao() {
    // --- Paleta de Cores Temática ---
    const char* BRONZE = "\033[38;5;172m";
    const char* STONE_DARK = "\033[38;5;238m";
    const char* MINOTAUR_COLOR = "\033[38;5;94m";
    const char* RED_HIGHLIGHT = "\033[1;31m";
    const char* GREEN_INFO = "\033[1;32m";
    const char* GREEN = "\033[1;32m";
    const char* STONE = "\033[38;5;242m";
    const char* RESET = "\033[0m";
    const char* BOLD = "\033[1m";

    // --- Título Principal ---
    std::cout << BRONZE << BOLD << "\n     ╔═════════════════════════════════════════════╗" << RESET << std::endl;
    std::cout << BRONZE << BOLD << "     ║" << RESET << "       " << BRONZE << BOLD << "LABIRINTO DE CRETA - SIMULAÇÃO" << RESET << "        " << BRONZE << BOLD << "║" << RESET << std::endl;
    std::cout << BRONZE << BOLD << "     ╚═════════════════════════════════════════════╝" << RESET << std::endl;

    // --- Arte ASCII do Minotauro (Fornecida pelo usuário e colorizada) ---
    // A arte é impressa linha por linha para permitir a inserção de cores
    std::cout << STONE_DARK << "      _____________ " << BRONZE << "nnnnnnnn nnnnnnnn" << STONE_DARK << " ___________" << RESET << std::endl;
    std::cout << STONE_DARK << "     |             " << BRONZE << "nnnnnnnn    nn" << STONE_DARK << "               |" << RESET << std::endl;
    std::cout << STONE_DARK << "     | -\"\"\\        " << BRONZE << "HHnnnnHP    HH" << STONE_DARK << "               |" << RESET << std::endl;
    std::cout << STONE_DARK << "    .-\"  .`)     (" << MINOTAUR_COLOR << "                              " << STONE_DARK << "|" << RESET << std::endl;
    std::cout << STONE_DARK << "   j   .'_+     :" << MINOTAUR_COLOR << "[                )      " << STONE_DARK << ".^--.. |" << RESET << std::endl;
    std::cout << STONE_DARK << "  i    -\"       " << MINOTAUR_COLOR << "|l                ].    /      i" << STONE_DARK << "|" << RESET << std::endl;
    std::cout << STONE_DARK << " ,\" .:j         " << MINOTAUR_COLOR << "`8o  _,,+.,.--,   d" << STONE_DARK << "|   `:::;    b" << RESET << std::endl;
    std::cout << STONE_DARK << " i  :'|          " << MINOTAUR_COLOR << "\"88p;.  (" << RED_HIGHLIGHT << "-.\"_\"-" << MINOTAUR_COLOR << ".oP" << MINOTAUR_COLOR << "        \\.   :" << RESET << std::endl;
    std::cout << STONE_DARK << " ; .  (            " << MINOTAUR_COLOR << ">,%%%   " << RED_HIGHLIGHT << "f" << MINOTAUR_COLOR << "),):8\"" << MINOTAUR_COLOR << "          \\:'  i" << RESET << std::endl;
    std::cout << STONE_DARK << "i  :: j          " << MINOTAUR_COLOR << ",;%%%:; ; ; i:%%%.," << MINOTAUR_COLOR << "        i.   `." << RESET << std::endl;
    std::cout << STONE_DARK << "i  `: ( ____  " << MINOTAUR_COLOR << ",-::::::' ::j  [:```" << MINOTAUR_COLOR << "          [8:   )" << RESET << std::endl;
    std::cout << STONE_DARK << "<  ..``'::::" << MINOTAUR_COLOR << "8888oooooo.  :(jj(,;,,," << MINOTAUR_COLOR << "         [8::  <" << RESET << std::endl;
    std::cout << STONE_DARK << "`. ``:.      " << MINOTAUR_COLOR << "oo.8888888888:;%%%8o.::.+888+o.:`:'" << STONE_DARK << "  |" << RESET << std::endl;
    std::cout << STONE_DARK << " `.   `        " << MINOTAUR_COLOR << "`o`88888888b`%%%%%88< Y888P\"\"'-" << STONE_DARK << "    ;" << RESET << std::endl;
    std::cout << STONE_DARK << "   \"`---`.       " << MINOTAUR_COLOR << "Y`888888888;;.,\"888b.\"\"..::::'" << STONE_DARK << "-'" << RESET << std::endl;
    std::cout << STONE_DARK << "     |    \"-....  " << MINOTAUR_COLOR << "b`8888888:::::.`8888._::-" << STONE_DARK << "     |" << RESET << std::endl;
    std::cout << STONE_DARK << "     |       `:::. " << MINOTAUR_COLOR << "`:::::O:::::::.`%%'" << STONE_DARK << "|         |" << RESET << std::endl;
    std::cout << STONE_DARK << "     |        `.      " << MINOTAUR_COLOR << "\"``::::::''" << STONE_DARK << "    .'         |" << RESET << std::endl;
    std::cout << STONE_DARK << "     |          `.                   <          |" << RESET << std::endl;
    std::cout << STONE_DARK << "     |            +:         `:   -';           |" << RESET << std::endl;
    std::cout << STONE_DARK << "     |  ____       `:         : .::/            |" << RESET << std::endl;
    std::cout << STONE_DARK << "     | `-|--        ;+_  :::. :..;;;            |" << RESET << std::endl;
    std::cout << STONE_DARK << "     |______________;;;;,;;;;;;;;,;;____________|" << RESET << std::endl;


    // --- Seção de Informações ---
    std::cout << BRONZE << BOLD << "\n            ⚜ " << BOLD << "O DESTINO DO PRISIONEIRO" << RESET << BRONZE << BOLD << " ⚜" << RESET << std::endl;
    std::cout << STONE_DARK << "  ╔════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << STONE_DARK << "  ║ " << RESET << "Ponto de Partida (Entrada):      Sala " << BOLD << vEntr << RESET << std::endl;
    std::cout << STONE_DARK << "  ║ " << GREEN_INFO << "Vislumbre de Liberdade (Saída):  Sala " << BOLD << labirinto.get_saida() << RESET << std::endl;
    std::cout << STONE_DARK << "  ║────────────────────────────────────────────────────────║" << std::endl;
    std::cout << STONE_DARK << "  ║ " << RED_HIGHLIGHT << "Covil da Fera (Minotauro):       Sala " << BOLD << posIniM << RESET << std::endl;
    std::cout << STONE_DARK << "  ║ " << RESET << "Provisões para a Jornada:      " << BOLD << kitsDeComida << " kits" << RESET << std::endl;
    std::cout << STONE_DARK << "  ╚════════════════════════════════════════════════════════╝" << RESET << std::endl;
    
    // --- Seção de Geração: "Os Corredores de Pedra" ---
    std::cout << BRONZE << "\n            🏛  " << BOLD << "Os Corredores de Pedra" << RESET << BRONZE << " 🏛" << RESET << std::endl;
    std::cout << GREEN << "  Gerando uma estrutura com " << BOLD << nV << RESET << GREEN << " salas e " << BOLD << nA << RESET << GREEN << " caminhos..." << RESET << std::endl;
    std::cout << STONE <<  "  ╔════════════════════════════════════════════════════════╗" << std::endl;

    for (int v = 0; v < labirinto.getNumVertices(); ++v) {
        const auto& vizinhos = labirinto.get_vizinhos(v);
        std::cout << STONE << "  ║ " << BRONZE << "• Sala " << BOLD << v << RESET << STONE << " se conecta com:" << RESET << std::endl;
        
        auto no = vizinhos.get_cabeca();
        if (no == nullptr) {
            std::cout << STONE << "  ║    └── Vazio e Silêncio..." << std::endl;
        } else {
            while (no != nullptr) {
                // Determina o conector do galho (└── para o último, ├── para os outros)
                const char* connector = (no->prox == nullptr) ? "└──" : "├──";
                std::cout << STONE << "  ║    " << connector << " Sala " << BOLD << no->dado.primeiro << RESET 
                          << STONE << " (distância: " << no->dado.segundo << ")" << RESET << std::endl;
                no = no->prox;
            }
        }
        if (v < labirinto.getNumVertices() - 1) {
             std::cout << STONE << "  ║╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌║" << std::endl;
        }
    }
    std::cout << STONE <<  "  ╚════════════════════════════════════════════════════════╝" << RESET << std::endl;
    std::cout << "\n" << BRONZE << BOLD << "Que os deuses guiem seus passos. A simulação começou!" << RESET << std::endl;
}

Simulador::ResultadoSimulacao Simulador::run(unsigned int seed, int chanceBatalha) {
    // Inicializa o relogio global    
    tempoGlobal = 0.0;

    // Inicializa os tempos dos próximos movimentos
    prxMovP = 0.0;
    prxMovM = 0.0;

    // Inicializa o gerador de números aleatórios com a seed fornecida
    std::mt19937 gerador(seed);

    // Cria o prisioneiro e o minotauro
    Prisioneiro p(vEntr, kitsDeComida);
    Minotauro m(posIniM, percepcaoMinotauro, labirinto, labirinto.getNumVertices());

    // Minotauro lembra os caminhos mínimos entre todos os pares de vértices
    m.lembrarCaminhos();

    // Estrutura para armazenar o resultado da simulação

    Simulador::ResultadoSimulacao resultado;
    bool fimDeJogo = false;
    bool minotauroVivo = true;
    std::string motivoFim;

    // Silence unused parameter warning
    (void)chanceBatalha;

    std::vector<EventoMovimento> eventos;
    while (!fimDeJogo){
        if (prxMovP <= prxMovM && p.getKitsDeComida() > 0) {
            int pos_antiga = p.getPos();
            const auto& vizinhos = labirinto.get_vizinhos(pos_antiga);
            tempoGlobal = prxMovP;
            // Descobre próximo movimento
            int destino = -1;
            int peso = 0;
            auto no_vizinho = vizinhos.get_cabeca();
            while (no_vizinho != nullptr) {
                int proximo_vertice = no_vizinho->dado.primeiro;
                int peso_aresta = no_vizinho->dado.segundo;
                if (!p.foiVisitado(proximo_vertice) && p.getKitsDeComida() >= peso_aresta) {
                    destino = proximo_vertice;
                    peso = peso_aresta;
                    break;
                }
                no_vizinho = no_vizinho->prox;
            }
            if (destino == -1) {
                fimDeJogo = true;
                continue;
            }
            EventoMovimento ev;
            ev.tempoInicio = tempoGlobal;
            ev.tempoFim = tempoGlobal + peso;
            ev.agente = "Prisioneiro";
            ev.origem = pos_antiga;
            ev.destino = destino;
            ev.peso = peso;
            eventos.push_back(ev);
            tempoGlobal += peso;
            prxMovP = tempoGlobal;
            p.mover(vizinhos); // Atualiza posição, kits, etc
        }
        else if (minotauroVivo)
        {
            // Chama o turno do Minotauro, que já faz o debug detalhado
            int pos_antiga = m.getPos();
            turnoMinotauro(m, p.getPos(), gerador);
            int proximoPasso = m.getPos();
            int peso_aresta = 0;
            if (pos_antiga != proximoPasso) {
                peso_aresta = labirinto.getPesoAresta(pos_antiga, proximoPasso);
            }
            EventoMovimento ev;
            ev.tempoInicio = tempoGlobal;
            ev.tempoFim = tempoGlobal + peso_aresta;
            ev.agente = "Minotauro";
            ev.origem = pos_antiga;
            ev.destino = proximoPasso;
            ev.peso = peso_aresta;
            eventos.push_back(ev);
            prxMovM = tempoGlobal + peso_aresta;
        } else {
            tempoGlobal = prxMovP;
            continue;
        }
        verificaEstados(p,m,fimDeJogo,minotauroVivo,motivoFim, seed, chanceBatalha, gerador);
    }
        std::cout << "[DEBUG] Loop principal da simulação finalizado. Nenhum movimento restante.\n" << std::endl;
        printarLogsComProgresso(eventos);
        return resultado;
}

void Simulador::verificaEstados(Prisioneiro& p, Minotauro& m, bool& fimDeJogo, bool& minotauroVivo, std::string& motivoFim, unsigned int seed, int chanceBatalha, std::mt19937& gerador) {
    if (!p.getKitsDeComida()) {
        motivoFim = "O prisioneiro morreu de fome no dia " + std::to_string(static_cast<int>(tempoGlobal)) + ".";
        fimDeJogo = true;
    } else if (p.getPos() == labirinto.get_saida()) {
        motivoFim = "O prisioneiro escapou com sucesso!";
        fimDeJogo = true;
    } else if (p.getPos() == m.getPos() && minotauroVivo) {
        if (prisioneiroBatalha(seed, chanceBatalha, gerador)) {
            minotauroVivo = false;
            // O jogo continua, mas o Minotauro está fora de ação.
        } else {
            motivoFim = "Prisioneiro foi pego e devorado pelo Minotauro.";
            fimDeJogo = true;
        }
    }
}



void Simulador::turnoPrisioneiro(Prisioneiro& p){
    // int pos_antiga = p.getPos(); // unused variable
    const auto& vizinhos = labirinto.get_vizinhos(p.getPos());

    tempoGlobal = prxMovP;

    int gastoKits = p.mover(vizinhos);
    prxMovP = tempoGlobal + gastoKits;
}


void Simulador::turnoMinotauro(Minotauro& m, int pos_prisioneiro, std::mt19937& gerador) {
    // Avança o tempo global para o momento da ação do Minotauro
    std::clog << "\n[DEBUG] ===== INÍCIO DO TURNO DO MINOTAURO =====" << std::endl;
    tempoGlobal = prxMovM;
    int pos_antiga = m.getPos();
    int proximoPasso = pos_antiga; // Valor padrão: ficar parado se não houver para onde ir

    int distancia = m.lembrarDist(pos_antiga, pos_prisioneiro);
    std::clog << "\n[DEBUG] TURNO DO MINOTAURO" << std::endl;
    std::clog << "  - Posição atual: " << pos_antiga << std::endl;
    std::clog << "  - Posição do prisioneiro: " << pos_prisioneiro << std::endl;
    std::clog << "  - Distância até prisioneiro: " << distancia << std::endl;
    std::clog << "  - Percepção: " << m.getPercepcao() << std::endl;
    std::clog << "  - Salas no labirinto: " << labirinto.getNumVertices() << std::endl;

    if (distancia == -1) {
        std::clog << "  - Motivo da ação: Não foi possível calcular a distância. Minotauro ficará parado." << std::endl;

    } else if (distancia <= m.getPercepcao()) {

        proximoPasso = m.lembrarProxPasso(pos_antiga, pos_prisioneiro);
        std::clog << "  - AÇÃO: PERSEGUIÇÃO" << std::endl;
        std::clog << "    - Motivo: Prisioneiro está a " << distancia << " de distância, dentro do alcance de percepção (" << m.getPercepcao() << ")" << std::endl;
        std::clog << "    - Próximo passo calculado: " << proximoPasso << std::endl;
    } else {
        const auto& vizinhos = labirinto.get_vizinhos(pos_antiga);
        std::vector<int> opcoes;
        std::clog << "  - AÇÃO: PATRULHA" << std::endl;
        std::clog << "    - Motivo: Prisioneiro está a " << distancia << " de distância, fora do alcance de percepção (" << m.getPercepcao() << ")" << std::endl;
        std::clog << "    - Vizinhos disponíveis: ";
        for (auto no = vizinhos.get_cabeca(); no != nullptr; no = no->prox) {
            opcoes.push_back(no->dado.primeiro);
            std::clog << no->dado.primeiro << "(peso=" << no->dado.segundo << ") ";
        }
        std::clog << std::endl;
        if (!opcoes.empty()) {
            std::uniform_int_distribution<size_t> dist(0, opcoes.size() - 1);
            proximoPasso = opcoes[dist(gerador)];
            int pesoEscolhido = labirinto.getPesoAresta(pos_antiga, proximoPasso);
            std::clog << "    - Próximo passo escolhido aleatoriamente: " << proximoPasso << " (peso=" << pesoEscolhido << ")" << std::endl;
        } else {
            std::clog << "    - Minotauro está em um beco sem saída e ficará parado." << std::endl;
        }
    }

    int pos_antes = m.getPos();
    m.mover(proximoPasso);
    int pos_depois = m.getPos();

    int peso_aresta = 0;
    if (pos_antiga != proximoPasso) {
        peso_aresta = labirinto.getPesoAresta(pos_antiga, proximoPasso);
        std::clog << "  - Peso da aresta percorrida: " << peso_aresta << std::endl;
    } else {
        std::clog << "  - Minotauro permaneceu na mesma sala." << std::endl;
    }
    // Agenda o próximo movimento do Minotauro
    prxMovM += peso_aresta;
    std::clog << "  - Tempo global atualizado: " << tempoGlobal << " | Próximo movimento em: " << prxMovM << std::endl;
    std::clog << "[DEBUG] ===== FIM DO TURNO DO MINOTAURO =====" << std::endl;
    std::clog << "  - Posição antes: " << pos_antes << " | Posição depois: " << pos_depois << std::endl;
}