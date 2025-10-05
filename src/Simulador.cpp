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

struct EventoMovimento {
    double tempoInicio;
    double tempoFim;
    std::string agente;
    int origem;
    int destino;
    int peso;
};



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

/**
 * @brief Executa o loop principal da simulação.
 * @details A lógica de tempo foi refatorada para um sistema de eventos discreto.
 * O loop avança o tempo para o próximo evento agendado (movimento de um
 * agente), garantindo uma progressão de tempo consistente e correta.
 */
Simulador::ResultadoSimulacao Simulador::run(unsigned int seed, int chanceBatalha) {
    // Inicializa o relogio global    
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

    // Estrutura para armazenar o resultado da simulação
    Simulador::ResultadoSimulacao resultado;
    bool fimDeJogo = false;
    bool minotauroVivo = true;
    std::string motivoFim;
    std::vector<EventoMovimento> eventos;

    while (!fimDeJogo){
        // Decide quem se move primeiro com base no próximo tempo de movimento
        if (prxMovP <= prxMovM && p.getKitsDeComida() > 0) {
            turnoPrisioneiro(p);
        }
        else if (minotauroVivo)
        {
            // Turno do Minotauro
            tempoGlobal = prxMovM;

            int custoMovimento = turnoMinotauro(m, p.getPos(), gerador);
            std::cout << "[DEBUG] Turno do Minotauro: movendo da sala " << m.getPos() << " para "<< m.getPos() 
            << " Custo do movimento: " << custoMovimento << " unidades de tempo." << std::endl;
            prxMovM = tempoGlobal + custoMovimento;
        } else {
            tempoGlobal = prxMovP;
            const auto& vizinhos = labirinto.get_vizinhos(p.getPos());
            int custoMovimento = p.mover(vizinhos);
            if (custoMovimento > 0){
                prxMovP = tempoGlobal + custoMovimento;
            } else {
                resultado.motivoFim = "O prisioneiro ficou preso sem poder se mover.";
                fimDeJogo = true;
            }
        }
        verificaEstados(p,m,fimDeJogo,minotauroVivo,motivoFim, seed, chanceBatalha, gerador);
    }
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
    // Turno do Prisioneiro
    tempoGlobal = prxMovP;

    int pos_antiga = p.getPos();

    // Get vizinhos da posição atual
    const auto& vizinhos = labirinto.get_vizinhos(p.getPos());

    // Move o prisioneiro e atualiza o tempo do próximo movimento
    int custoMovimento = p.mover(vizinhos);

    std::cout << "[DEBUG] Turno do Prisioneiro: movendo da sala " << pos_antiga << " para "<< p.getPos() 
    << " Custo do movimento: " << custoMovimento << " kits de comida." << std::endl;
    
    if (custoMovimento > 0){
        prxMovP = tempoGlobal + custoMovimento;
    }
    else{
        resultado.motivoFim = "O prisioneiro ficou preso sem poder se mover.";
        fimDeJogo = true;
    }
}


int Simulador::turnoMinotauro(Minotauro& m, int posPrisioneiro, std::mt19937& gerador) {
    int posAntiga = m.getPos();
    int proximoPasso = posAntiga;

    int distancia = m.lembrarDist(posAntiga, posPrisioneiro);

    if (distancia != -1 && distancia <= m.getPercepcao()) {
        // MODO PERSEGUIÇÃO
        proximoPasso = m.lembrarProxPasso(posAntiga, posPrisioneiro);
    } else {
        // MODO PATRULHA (Lógica otimizada para evitar alocação de std::vector)
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

    if (posAntiga != proximoPasso) {
        return labirinto.getPesoAresta(posAntiga, proximoPasso);
    }
    
    return 1;
}