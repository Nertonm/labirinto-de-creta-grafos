#include <fstream>
#include "Grafo.h"
#include "Prisioneiro.h"
#include "Minotauro.h"
#include "utils/Logger.h"
#include <random>

class Simulador {
public:

    // Estrutura para registrar eventos de movimento na simulação
    struct EventoMovimento {
        double tempoInicio;
        double tempoFim;
        std::string agente;
        int origem;
        int destino;
        int peso;
    };
    Simulador();

    bool carregarArquivo(const std::string& nome_arquivo);

    // Estrutura para encapsular os resultados da simulação
    struct ResultadoSimulacao{
        bool prisioneiroSobreviveu;
        int diasSobrevividos;
    double tempoReal; // tempo contínuo da simulação
        std::vector<int> caminhoP;
        std::vector<int> caminhoM;
        std::string motivoFim;
        int kitsRestantes;
        int posFinalP;
        int posFinalM;
        bool minotauroVivo;
    // Novo: Linha do tempo e encontro
    std::vector<Logger::EventoMovimento> eventos;
    double tempoEncontro = -1.0;
    std::string tipoEncontro; // "sala" ou "aresta"
    };

    ResultadoSimulacao run(unsigned int seed, int chanceBatalha);

    // Informações para cabeçalho humano
    Logger::SimulacaoInfo getSimulacaoInfo() const;

private:
    bool cheiroDePrisioneiro(int posMinotauro, int posPrisioneiro, int percepcao, Minotauro& m);
    bool prisioneiroBatalha(unsigned int seed, int chanceBatalha, std::mt19937& gerador);
    void turnoPrisioneiro(Prisioneiro& p);
    int turnoMinotauro(Minotauro& m, int posPrisioneiro, std::mt19937& gerador,  bool cheiroDePrisioneiro);
    void verificaEstados(Prisioneiro& p, Minotauro& m, bool& fimDeJogo, bool& minotauroVivo, std::string& motivoFim, unsigned int seed, int chanceBatalha, std::mt19937& gerador);

    // Detecção/agendamento de encontro em trânsito na mesma aresta (sentidos opostos)
    bool detectarEncontroEmAresta(double& tEncontroOut);
    void agendarEncontroEmArestaSeNecessario();


    Grafo labirinto;
    
    int nA;
    int nV;
    int vEntr;
    int vSaid;
    int posIniM;
    int percepcaoMinotauro;

    bool fimDeJogo;
    std::vector<EventoMovimento> eventos;
    ResultadoSimulacao resultado;

    int kitsDeComida;

    // Estados da simulação
    double tempoGlobal;
    double prxMovP;
    double prxMovM;

    // Posições efetivas para checagem durante deslocamentos
    int ultimaPosP; // última sala do prisioneiro antes do deslocamento atual
    int ultimaPosM; // última sala do minotauro antes do deslocamento atual

    // Rastreio do deslocamento atual (para encontros em aresta)
    int destAtualP = -1;
    int destAtualM = -1;
    double inicioMovP = 0.0;
    double inicioMovM = 0.0;

    // Evento de encontro em trânsito
    bool encontroEdgePendente = false;
    double tempoEncontroEdge = -1.0;
};