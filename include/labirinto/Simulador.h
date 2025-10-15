/**
 * @file Simulador.h
 * @brief Definição da classe Simulador, o motor central da simulação.
 * @details Este arquivo declara a classe Simulador, que é responsável por
 * carregar o cenário, gerenciar o loop de eventos, processar os turnos dos
 * agentes e determinar o resultado final da simulação.
 */

#include <fstream>
#include "Grafo.h"
#include "Prisioneiro.h"
#include "Minotauro.h"
#include "utils/Logger.h"
#include <random>

/**
 * @class Simulador
 * @brief Orquestra a simulação do labirinto, gerenciando o tempo e as interações.
 * @details A classe utiliza um modelo de simulação de eventos discretos, onde o tempo
 * avança para o próximo evento agendado, garantindo uma progressão precisa e eficiente.
 */
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

    /**
     * @brief Construtor da classe Simulador.
     */
    Simulador();

    /**
     * @brief Carrega a configuração do labirinto e da simulação a partir de um arquivo.
     * @param nome_arquivo O caminho para o arquivo de texto do cenário.
     * @return `true` se o arquivo foi carregado com sucesso, `false` caso contrário.
     */
    bool carregarArquivo(const std::string& nome_arquivo);

    
    /**
     * @struct ResultadoSimulacao
     * @brief Encapsula todos os dados relevantes do desfecho de uma simulação.
     */
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

    /**
     * @brief Executa o loop principal da simulação.
     * @param seed A semente para o gerador de números aleatórios, para reprodutibilidade.
     * @param chanceBatalha A chance percentual (1-100) de o prisioneiro vencer um encontro.
     * @return Uma struct `ResultadoSimulacao` com todos os dados do final da simulação.
     */
    ResultadoSimulacao run(unsigned int seed, int chanceBatalha);
    
    /**
     * @brief Coleta as informações iniciais da simulação para exibição.
     * @return Uma struct `Logger::SimulacaoInfo` com os dados da configuração inicial.
     */
    Logger::SimulacaoInfo getSimulacaoInfo() const;

private:
    /**
     * @brief Verifica se o Minotauro pode detectar o prisioneiro.
     * @return `true` se a distância entre eles for menor ou igual à percepção.
     */
    bool cheiroDePrisioneiro(int posMinotauro, int posPrisioneiro, int percepcao, Minotauro& m);

    /**
     * @brief Simula uma batalha entre o prisioneiro e o Minotauro.
     * @return `true` se o prisioneiro vencer.
     */
    bool prisioneiroBatalha(unsigned int seed, int chanceBatalha, std::mt19937& gerador);

    /**
     * @brief Processa um turno de movimento para o prisioneiro.
     */
    void turnoPrisioneiro(Prisioneiro& p);

    /**
     * @brief Processa um turno de movimento para o Minotauro.
     * @return Um inteiro indicando o status do turno.
     */
    int turnoMinotauro(Minotauro& m, int posPrisioneiro, std::mt19937& gerador,  bool cheiroDePrisioneiro);

    /**
     * @brief Verifica as condições de fim de jogo após cada evento.
     */
    void verificaEstados(Prisioneiro& p, Minotauro& m, bool& fimDeJogo, bool& minotauroVivo, std::string& motivoFim, unsigned int seed, int chanceBatalha, std::mt19937& gerador);

    /**
     * @brief Detecta se os agentes estão se movendo um em direção ao outro na mesma aresta.
     * @param[out] tEncontroOut Parâmetro de saída que receberá o tempo exato do encontro.
     * @return `true` se um encontro for detectado.
     */
    bool detectarEncontroEmAresta(double& tEncontroOut);

    /**
     * @brief Agenda um evento de encontro em aresta se um for detectado.
     */
    void agendarEncontroEmArestaSeNecessario();


    Grafo labirinto;  // O grafo que representa o labirinto.    
    int nA; // número de arestas
    int nV; // número de vértices
    int vEntr; // vértice de entrada
    int vSaid; // vértice de saída
    int posIniM; // posição inicial do Minotauro
    int percepcaoMinotauro; // percepção do Minotauro

    bool fimDeJogo; // flag para indicar se a simulação terminou
    std::vector<EventoMovimento> eventos; // registro de eventos para o logger
    ResultadoSimulacao resultado; // resultado final da simulação

    int kitsDeComida; // kits de comida iniciais do prisioneiro

    // Estados da simulação
    double tempoGlobal; // tempo contínuo da simulação
    double prxMovP; // tempo do próximo movimento do prisioneiro
    double prxMovM; // tempo do próximo movimento do Minotauro

    // Posições efetivas para checagem durante deslocamentos
    int ultimaPosP; // última sala do prisioneiro antes do deslocamento atual
    int ultimaPosM; // última sala do minotauro antes do deslocamento atual

    // Rastreio do deslocamento atual (para encontros em aresta)
    int destAtualP = -1; // destino atual do prisioneiro (-1 se parado)
    int destAtualM = -1; // destino atual do minotauro (-1 se parado)
    double inicioMovP = 0.0; // tempo de início do movimento do prisioneiro
    double inicioMovM = 0.0; // tempo de início do movimento do minotauro

    // Evento de encontro em trânsito
    bool encontroEdgePendente = false; // flag indicando se há um encontro em aresta pendente
    double tempoEncontroEdge = -1.0; // O tempo agendado para o evento de encontro em aresta.
};