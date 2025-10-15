/**
 * @file Simulador.cpp
 * @author Thiago Nerton
 * @brief Implementação da classe Simulador.
 * @details Este arquivo contém a lógica central que gerencia o loop da simulação,
 * o carregamento de dados, o processamento de turnos dos agentes (Prisioneiro e
 * Minotauro) e a verificação das condições de fim de jogo.
 */

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
#include <limits>
#include "utils/Logger.h"

/**
 * @brief Construtor da classe Simulador.
 * @details Inicializa as variáveis de estado da simulação com seus valores padrão.
 */
Simulador::Simulador() : fimDeJogo(false), tempoGlobal(0.0), prxMovP(0.0), prxMovM(0.0), ultimaPosP(-1), ultimaPosM(-1) {}

/**
 * @brief Carrega a configuração do labirinto e da simulação a partir de um arquivo de texto.
 * @details Lê o número de vértices, arestas, as conexões do grafo e os parâmetros
 * iniciais da simulação (posições, percepção, comida).
 * @param nomeArquivo O caminho para o arquivo de configuração.
 * @return `true` se o arquivo foi carregado com sucesso, `false` caso contrário.
 */
bool Simulador::carregarArquivo(const std::string& nomeArquivo) {
    std::ifstream arquivoEntrada(nomeArquivo);
    Logger::info(0.0, "Iniciando carregamento do arquivo: {}", Logger::LogSource::OUTRO, nomeArquivo);

    if (!arquivoEntrada.is_open()) {
    Logger::error(0.0, "Erro ao abrir arquivo: {}", Logger::LogSource::OUTRO, nomeArquivo);
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
        Logger::error(0.0, "Erro ao ler dimensões do labirinto no arquivo: {}", Logger::LogSource::OUTRO, nomeArquivo);
        return false;
    }

    labirinto.setNumVertices(nV);
    labirinto.setNumArestas(nA);

    for (int i = 0; i < nA; ++i) {
        if (std::getline(arquivoEntrada, linha)) {
            std::stringstream ss(linha);
            int u, v, peso;
            ss >> u >> v >> peso;
            labirinto.adicionar_aresta(u, v, peso);
        } else {
            Logger::error(0.0, "Erro ao ler as arestas do arquivo: {}", Logger::LogSource::OUTRO, nomeArquivo);
            return false;
        }
    }

    int vSaid;
    if (!lerValor(vEntr) || !lerValor(vSaid) || !lerValor(posIniM) || !lerValor(percepcaoMinotauro) || !lerValor(kitsDeComida)) {
        Logger::error(0.0, "Erro ao ler parâmetros da simulação no arquivo: {}", Logger::LogSource::OUTRO, nomeArquivo);
        return false;
    }

    labirinto.set_saida(vSaid);

    Logger::info(0.0, "Arquivo carregado com sucesso: {}", Logger::LogSource::OUTRO, nomeArquivo);
    return true;
}

/**
 * @brief Simula uma batalha entre o prisioneiro e o Minotauro.
 * @details Gera um número aleatório de 1 a 100 e verifica se é menor ou igual à
 * chance de batalha do prisioneiro.
 * @param seed A semente para o gerador de números aleatórios (não utilizada diretamente, mas garante consistência).
 * @param chanceBatalha A chance percentual (1-100) de o prisioneiro vencer.
 * @param gerador A instância do gerador de números aleatórios a ser usada.
 * @return `true` se o prisioneiro venceu a batalha, `false` caso contrário.
 */
bool Simulador::prisioneiroBatalha(unsigned int seed, int chanceBatalha, std::mt19937& gerador) {
    std::uniform_int_distribution<int> dist(1, 100);
    int sorte = dist(gerador);
    Logger::info(tempoGlobal, "Batalha! Número sorteado: {}. Chance de vitória do prisioneiro: {}.", Logger::LogSource::PRISIONEIRO, sorte, chanceBatalha);
    return sorte <= chanceBatalha;
}

/**
 * @brief Executa o loop principal da simulação.
 * @details A simulação opera em um sistema de eventos discretos. O loop principal
 * avança o tempo para o próximo evento agendado (chegada de um agente a uma sala
 * ou um encontro em uma aresta). Ele processa o evento, agenda o próximo movimento
 * do agente envolvido e verifica as condições de fim de jogo a cada passo.
 * @param seed A semente para inicializar o gerador de números aleatórios, garantindo a reprodutibilidade.
 * @param chanceBatalha A chance percentual de o prisioneiro vencer um encontro.
 * @return Uma struct `ResultadoSimulacao` contendo todos os dados do desfecho da simulação.
 */
Simulador::ResultadoSimulacao Simulador::run(unsigned int seed, int chanceBatalha) {
    tempoGlobal = 0.0;
    // Reset estado/resultado da simulação
    fimDeJogo = false;
    encontroEdgePendente = false;
    tempoEncontroEdge = -1.0;
    resultado = ResultadoSimulacao{};
    resultado.minotauroVivo = true;
    resultado.motivoFim.clear();
    resultado.tipoEncontro.clear();
    resultado.tempoEncontro = -1.0;
    resultado.eventos.clear();
    // Inicializa o gerador de números aleatórios com a seed fornecida
    std::mt19937 gerador(seed);

    // Inicializa os agentes
    Prisioneiro p(vEntr, kitsDeComida);
    Minotauro m(posIniM, percepcaoMinotauro, labirinto, labirinto.getNumVertices());

    // Minotauro lembra os caminhos mínimos entre todos os pares de vértices
    m.lembrarCaminhos();

    // Inicializa os tempos dos próximos movimentos
    prxMovP = 0.0; 
    prxMovM = 0.0;
    bool minotauroVivo = true;

    // inicializa posições de referência para checagem de encontro
    ultimaPosP = p.getPos();
    ultimaPosM = m.getPos();

    while (true){
        double tP = prxMovP;
        double tM = minotauroVivo ? prxMovM : std::numeric_limits<double>::infinity();
        double tE = encontroEdgePendente ? tempoEncontroEdge : std::numeric_limits<double>::infinity();

        double tNext = std::min(tP, std::min(tM, tE));

        if (tNext == tE) {
            // Evento: encontro em trânsito no meio da aresta
            tempoGlobal = tempoEncontroEdge;
            encontroEdgePendente = false; // consome evento
            Logger::info(tempoGlobal, "Prisioneiro e Minotauro se cruzam no corredor entre {} e {}!", Logger::LogSource::OUTRO, ultimaPosP, destAtualP);
            // Resolve batalha
            resultado.tempoEncontro = tempoGlobal;
            resultado.tipoEncontro = "aresta";
            if (prisioneiroBatalha(seed, chanceBatalha, gerador)) {
                Logger::info(tempoGlobal, "Prisioneiro venceu a batalha contra o Minotauro!", Logger::LogSource::PRISIONEIRO);
                resultado.prisioneiroSobreviveu = true;
                resultado.minotauroVivo = false;
                this->resultado.motivoFim = "Prisioneiro derrotou o Minotauro.";
                fimDeJogo = true;
            } else {
                std::string motivoFim = "Prisioneiro foi pego e devorado pelo Minotauro.";
                Logger::info(tempoGlobal, motivoFim, Logger::LogSource::MINOTAURO);
                resultado.prisioneiroSobreviveu = false;
                resultado.minotauroVivo = true;
                this->resultado.motivoFim = motivoFim;
                fimDeJogo = true;
            }
        } else if (tNext == tP || !minotauroVivo) {
            tempoGlobal = prxMovP;
            // ao iniciar um novo deslocamento do prisioneiro, fixa a última sala
            ultimaPosP = p.getPos();
            turnoPrisioneiro(p);
            // após agendar movimento, verificar cruzamento em aresta
            agendarEncontroEmArestaSeNecessario();
        } else {
            tempoGlobal = prxMovM;
            bool temCheiroDePrisioneiro = cheiroDePrisioneiro(m.getPos(), p.getPos(), m.getPercepcao(), m);
            // fixa a última sala do minotauro antes de iniciar o deslocamento
            ultimaPosM = m.getPos();
            turnoMinotauro(m, ultimaPosP, gerador, temCheiroDePrisioneiro);
            // após agendar movimento, verificar cruzamento em aresta
            agendarEncontroEmArestaSeNecessario();
        }
        if (fimDeJogo)
            break;
        verificaEstados(p,m,fimDeJogo,minotauroVivo,resultado.motivoFim, seed, chanceBatalha, gerador);
        if (fimDeJogo)
            break;
    }

    resultado.caminhoP = p.getCaminho();
    resultado.kitsRestantes = p.getKitsDeComida();
    resultado.posFinalP = p.getPos();
    resultado.posFinalM = m.getPos();
    resultado.diasSobrevividos = static_cast<int>(tempoGlobal);
    resultado.tempoReal = tempoGlobal;

    return resultado;
}

/**
 * @brief Verifica o estado atual da simulação para determinar se o jogo terminou.
 * @details Checa as três condições de término: prisioneiro sem comida, prisioneiro na saída,
 * ou um encontro entre prisioneiro e Minotauro na mesma sala.
 * @param p Referência ao objeto Prisioneiro.
 * @param m Referência ao objeto Minotauro.
 * @param fimDeJogo Referência ao booleano que controla o loop principal.
 * @param minotauroVivo Referência ao booleano que indica se o Minotauro está vivo.
 * @param motivoFim Referência à string que armazenará a razão do fim da simulação.
 * @param seed Semente para a batalha.
 * @param chanceBatalha Chance de vitória do prisioneiro na batalha.
 * @param gerador Gerador de números aleatórios para a batalha.
 */
void Simulador::verificaEstados(Prisioneiro& p, Minotauro& m, bool& fimDeJogo, bool& minotauroVivo, std::string& motivoFim, unsigned int seed, int chanceBatalha, std::mt19937& gerador) {
    if (!p.getKitsDeComida()) {
        motivoFim = "O prisioneiro morreu de fome no dia " + std::to_string(static_cast<int>(tempoGlobal)) + ".";
        Logger::info(tempoGlobal, motivoFim, Logger::LogSource::PRISIONEIRO);
        resultado.prisioneiroSobreviveu = false;
        fimDeJogo = true;
    } else if (p.getPos() == labirinto.get_saida()) {
        motivoFim = "O prisioneiro escapou com sucesso!";
        Logger::info(tempoGlobal, motivoFim, Logger::LogSource::PRISIONEIRO);
        resultado.prisioneiroSobreviveu = true;
        fimDeJogo = true;
    } else {
        // Encontro apenas quando ambos não estão em trânsito e ocupam a mesma sala
        bool pEmTransito = prxMovP > tempoGlobal;
        bool mEmTransito = prxMovM > tempoGlobal;
        if (!pEmTransito && !mEmTransito && p.getPos() == m.getPos() && minotauroVivo) {
            Logger::info(tempoGlobal, "Prisioneiro encontrou o Minotauro!", Logger::LogSource::PRISIONEIRO);
            resultado.tempoEncontro = tempoGlobal;
            resultado.tipoEncontro = "sala";
            if (prisioneiroBatalha(seed, chanceBatalha, gerador)) {
                minotauroVivo = false;
                resultado.minotauroVivo = false;
                Logger::info(tempoGlobal, "Prisioneiro venceu a batalha contra o Minotauro!", Logger::LogSource::PRISIONEIRO);
            } else {
                motivoFim = "Prisioneiro foi pego e devorado pelo Minotauro.";
                Logger::info(tempoGlobal, motivoFim, Logger::LogSource::MINOTAURO);
                resultado.prisioneiroSobreviveu = false;
                resultado.minotauroVivo = true;
                fimDeJogo = true;
            }
        }
    }
}


/**
 * @brief Processa um turno de movimento para o prisioneiro.
 * @details Invoca o método `mover` do prisioneiro para determinar a próxima sala.
 * Com base no custo do movimento, agenda o próximo evento de chegada do prisioneiro (`prxMovP`).
 * @param p Referência ao objeto Prisioneiro.
 */
void Simulador::turnoPrisioneiro(Prisioneiro& p){
    p.setTempoPrisioneiro(tempoGlobal);

    int pos_antiga = p.getPos();
    const auto& vizinhos = labirinto.get_vizinhos(p.getPos());
    int custoMovimento = p.mover(vizinhos);
    Logger::info(tempoGlobal, "Prisioneiro começando a se mover da sala {} para {}. Custo: {} kits de comida.", Logger::LogSource::PRISIONEIRO, pos_antiga, p.getPos(), custoMovimento);
    if (custoMovimento > 0){
        prxMovP = tempoGlobal + custoMovimento;
    inicioMovP = tempoGlobal;
    destAtualP = p.getPos();
    resultado.eventos.push_back(Logger::EventoMovimento{tempoGlobal, prxMovP, "Prisioneiro", pos_antiga, destAtualP, custoMovimento});
    } else {
        Logger::warning(tempoGlobal, "Prisioneiro está preso na sala {} e não conseguiu se mover.", Logger::LogSource::PRISIONEIRO, pos_antiga);
        prxMovP = tempoGlobal + 1.0;
    inicioMovP = tempoGlobal;
    destAtualP = p.getPos();
    resultado.eventos.push_back(Logger::EventoMovimento{tempoGlobal, prxMovP, "Prisioneiro", pos_antiga, destAtualP, 1});
    }
}


/**
 * @brief Processa um turno de movimento para o Minotauro.
 * @details Determina o próximo movimento do Minotauro. Se ele "sente o cheiro" do prisioneiro,
 * ele usa sua memória para se mover pelo caminho mais rápido. Caso contrário, move-se
 * aleatoriamente. Agenda o próximo evento de chegada do Minotauro (`prxMovM`).
 * @param m Referência ao objeto Minotauro.
 * @param posPrisioneiro A posição atual do prisioneiro.
 * @param gerador Gerador de números aleatórios para o movimento errante.
 * @param cheiroDePrisioneiro `true` se o Minotauro detectou o prisioneiro.
 * @return Retorna 1 indicando que o turno foi processado.
 */
int Simulador::turnoMinotauro(Minotauro& m, int posPrisioneiro, std::mt19937& gerador, bool cheiroDePrisioneiro) {
    m.setTempoMinotauro((tempoGlobal));
    int posAntiga = m.getPos();
    int proximoPasso = posAntiga;

    if (cheiroDePrisioneiro) {
        Logger::info(tempoGlobal, "Minotauro sente que o Prisioneiro está perto e começa a persegui-lo duas vezes mais rapido.", Logger::LogSource::MINOTAURO);
        // Valida índices antes de consultar a memória do Minotauro
        if (posAntiga >= 0 && posPrisioneiro >= 0) {
            int memProx = m.lembrarProxPasso(posAntiga, posPrisioneiro);
            if (memProx >= 0) {
                proximoPasso = memProx;
            } else {
                // fallback para comportamento aleatório semelhante ao caso sem cheiro
                const auto& vizinhos = labirinto.get_vizinhos(posAntiga);
                int numVizinhos = 0;
                for (auto no = vizinhos.get_cabeca(); no != nullptr; no = no->prox) {
                    numVizinhos++;
                }
                if (numVizinhos > 0) {
                    std::uniform_int_distribution<int> dist(0, numVizinhos - 1);
                    int alvo = dist(gerador);
                    int i = 0;
                    for (auto no = vizinhos.get_cabeca(); no != nullptr; no = no->prox) {
                        if (i == alvo) {
                            proximoPasso = no->dado.primeiro;
                            break;
                        }
                        i++;
                    }
                }
            }
        }
    } else {
    Logger::info(tempoGlobal, "Minotauro vaga atrás de alimento.", Logger::LogSource::MINOTAURO);
        const auto& vizinhos = labirinto.get_vizinhos(posAntiga);
        int numVizinhos = 0;
        for (auto no = vizinhos.get_cabeca(); no != nullptr; no = no->prox) {
            numVizinhos++;
        }
        if (numVizinhos > 0) {
            std::uniform_int_distribution<int> dist(0, numVizinhos - 1);
            int alvo = dist(gerador);
            int i = 0;
            for (auto no = vizinhos.get_cabeca(); no != nullptr; no = no->prox) {
                if (i == alvo) {
                    proximoPasso = no->dado.primeiro;
                    break;
                }
                i++;
            }
        }
    }
    m.mover(proximoPasso);
    Logger::info(tempoGlobal, "Minotauro movendo da sala {} para {}.", Logger::LogSource::MINOTAURO, posAntiga, proximoPasso);
    if (posAntiga != proximoPasso) {
        resultado.caminhoM.push_back(proximoPasso);
        double pesoAresta = static_cast<double>(labirinto.getPesoAresta(posAntiga, proximoPasso));
        if (cheiroDePrisioneiro) {
            prxMovM = tempoGlobal + (pesoAresta / 2.0);
            if (prxMovM <= tempoGlobal) prxMovM = tempoGlobal + 0.0001; // avanço mínimo para evitar repetir o evento no mesmo instante
        } else {
            prxMovM = tempoGlobal + pesoAresta;
        }
        inicioMovM = tempoGlobal;
        destAtualM = proximoPasso;
    resultado.eventos.push_back(Logger::EventoMovimento{tempoGlobal, prxMovM, "Minotauro", posAntiga, destAtualM, static_cast<int>(pesoAresta)});
    }
    return 1;
}

/**
 * @brief Verifica se o Minotauro pode detectar o prisioneiro.
 * @param posMinotauro Posição atual do Minotauro.
 * @param posPrisioneiro Posição atual do prisioneiro.
 * @param percepcao O alcance da percepção do Minotauro.
 * @param m Referência ao objeto Minotauro para consultar a distância.
 * @return `true` se a distância entre eles for menor ou igual à percepção, `false` caso contrário.
 */
bool Simulador::cheiroDePrisioneiro(int posMinotauro, int posPrisioneiro, int percepcao, Minotauro& m) {
    // Valida índices antes de consultar a memória do Minotauro
    if (posMinotauro < 0 || posPrisioneiro < 0)
        return false;
    int dist = m.lembrarDist(posMinotauro, posPrisioneiro);
    if (dist < 0) // -1 indica erro ou caminho desconhecido
        return false;
    return dist <= percepcao;
}

/**
 * @brief Se um encontro em uma aresta for detectado, agenda um novo evento de encontro.
 * @details Chama `detectarEncontroEmAresta` e, se um encontro for iminente,
 * define `encontroEdgePendente` como `true` e armazena o tempo do evento em `tempoEncontroEdge`.
 */
bool Simulador::detectarEncontroEmAresta(double& tEncontroOut) {
    // Ambos precisam estar em trânsito
    if (!(prxMovP > tempoGlobal && prxMovM > tempoGlobal)) return false;

    // Devem estar na mesma aresta e em sentidos opostos
    if (!(ultimaPosP == destAtualM && destAtualP == ultimaPosM)) return false;

    // Comprimento da aresta
    int Lw = labirinto.getPesoAresta(ultimaPosP, destAtualP);
    if (Lw <= 0) return false;
    double L = static_cast<double>(Lw);

    // Velocidades (constantes ao longo da aresta)
    double dP = prxMovP - inicioMovP;
    double dM = prxMovM - inicioMovM;
    if (dP <= 0.0 || dM <= 0.0) return false;
    double vP = L / dP;
    double vM = L / dM;

    // Tempo de encontro te após ambos terem iniciado
    double te = (L + vP*inicioMovP + vM*inicioMovM) / (vP + vM);

    // Valida que te ocorre enquanto ambos ainda estão viajando
    double s = std::max(inicioMovP, inicioMovM);
    double e = std::min(prxMovP, prxMovM);
    // Verifica se o tempo de encontro está dentro do intervalo
    if (te + 1e-9 < s || te - 1e-9 > e) return false;

    // Tudo ok, retorna o tempo de encontro
    tEncontroOut = te;
    return true;
}

/**
 * @brief Se um encontro em uma aresta for detectado, agenda um novo evento de encontro.
 * @details Chama `detectarEncontroEmAresta` e, se um encontro for iminente,
 * define `encontroEdgePendente` como `true` e armazena o tempo do evento em `tempoEncontroEdge`.
 */
void Simulador::agendarEncontroEmArestaSeNecessario() {
    double tCand = -1.0;
    if (detectarEncontroEmAresta(tCand)) {
        if (!encontroEdgePendente || tCand < tempoEncontroEdge - 1e-9) {
            encontroEdgePendente = true;
            tempoEncontroEdge = tCand;
        }
    }
}

/**
 * @brief Coleta e retorna as informações iniciais da simulação.
 * @details Usado para imprimir o cabeçalho no modo de relatório humano.
 * @return Uma struct `Logger::SimulacaoInfo` com os dados da configuração inicial.
 */
Logger::SimulacaoInfo Simulador::getSimulacaoInfo() const {
    Logger::SimulacaoInfo info;
    info.vEntrada = vEntr;
    info.vSaida = labirinto.get_saida();
    info.posMinotauro = posIniM;
    info.kitsComida = kitsDeComida;
    info.numVertices = nV;
    info.numArestas = nA;
    info.labirinto = &labirinto;
    return info;
}
