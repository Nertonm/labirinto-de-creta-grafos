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
#include "utils/Logger.h"

Simulador::Simulador() : fimDeJogo(false), tempoGlobal(0.0), prxMovP(0.0), prxMovM(0.0) {}

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
    Logger::SimulacaoInfo info = {vEntr, vSaid, posIniM, kitsDeComida, nV, nA, &labirinto};
    Logger::imprimirInicioSimulacao(info);
    return true;
}

bool Simulador::prisioneiroBatalha(unsigned int seed, int chanceBatalha, std::mt19937& gerador) {
    std::uniform_int_distribution<int> dist(1, 100);
    int sorte = dist(gerador);
    Logger::info(tempoGlobal, "Batalha! Número sorteado: {}. Chance de vitória do prisioneiro: {}.", Logger::LogSource::PRISIONEIRO, sorte, chanceBatalha);
    return sorte <= chanceBatalha;
}

/**
 * @brief Executa o loop principal da simulação.
 * @details A lógica de tempo foi refatorada para um sistema de eventos discreto.
 * O loop avança o tempo para o próximo evento agendado (movimento de um
 * agente), garantindo uma progressão de tempo consistente e correta.
 */
Simulador::ResultadoSimulacao Simulador::run(unsigned int seed, int chanceBatalha) {
    tempoGlobal = 0.0;
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

    int ultimaPosP = p.getPos();

    while (true){
        bool turnoDoPrisioneiro = (prxMovP <= prxMovM || !minotauroVivo);

        if (turnoDoPrisioneiro) {
            tempoGlobal = prxMovP;
            ultimaPosP = p.getPos();
            turnoPrisioneiro(p);
        }
        else if (minotauroVivo){
            tempoGlobal = prxMovM;
            turnoMinotauro(m, ultimaPosP, gerador);
        }
        verificaEstados(p,m,fimDeJogo,minotauroVivo,resultado.motivoFim, seed, chanceBatalha, gerador);
        if (fimDeJogo)
            break;
    }

    resultado.caminhoP = p.getCaminho();
    resultado.kitsRestantes = p.getKitsDeComida();
    resultado.posFinalP = p.getPos();
    resultado.posFinalM = m.getPos();
    resultado.diasSobrevividos = static_cast<int>(tempoGlobal);

    return resultado;
}

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
    } else if (p.getPos() == m.getPos() && minotauroVivo) {
        Logger::info(tempoGlobal, "Prisioneiro encontrou o Minotauro!", Logger::LogSource::PRISIONEIRO);
        if (prisioneiroBatalha(seed, chanceBatalha, gerador)) {
            minotauroVivo = false;
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



void Simulador::turnoPrisioneiro(Prisioneiro& p){
    p.setTempoPrisioneiro(tempoGlobal);

    int pos_antiga = p.getPos();
    const auto& vizinhos = labirinto.get_vizinhos(p.getPos());
    int custoMovimento = p.mover(vizinhos);
    Logger::info(tempoGlobal, "Prisioneiro começando a se mover da sala {} para {}. Custo: {} kits de comida.", Logger::LogSource::PRISIONEIRO, pos_antiga, p.getPos(), custoMovimento);
    if (custoMovimento > 0){
        prxMovP = tempoGlobal + custoMovimento;
    } else {
        Logger::warning(tempoGlobal, "Prisioneiro está preso na sala {} e não conseguiu se mover.", Logger::LogSource::PRISIONEIRO, pos_antiga);
        prxMovP = tempoGlobal + 1.0;
    }
}


int Simulador::turnoMinotauro(Minotauro& m, int posPrisioneiro, std::mt19937& gerador) {
    m.setTempoMinotauro((tempoGlobal));
    int posAntiga = m.getPos();
    int proximoPasso = posAntiga;
    int distancia = m.lembrarDist(posAntiga, posPrisioneiro);

    if (distancia != -1 && distancia <= m.getPercepcao()) {
    Logger::info(tempoGlobal, "Minotauro sente que o Prisioneiro está perto e começa a persegui-lo. Distância: {}", Logger::LogSource::MINOTAURO, distancia);
        proximoPasso = m.lembrarProxPasso(posAntiga, posPrisioneiro);
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
        prxMovM = tempoGlobal + labirinto.getPesoAresta(posAntiga, proximoPasso);
    }
    return 1;
}
