#include <fstream>
#include "Grafo.h"
#include "Prisioneiro.h"
#include "Minotauro.h"
#include <random>

class Simulador {
public:
    Simulador();

    bool carregarArquivo(const std::string& nome_arquivo);

    // Estrutura para encapsular os resultados da simulação
    struct ResultadoSimulacao{
        bool prisioneiroSobreviveu;
        int diasSobrevividos;
        std::vector<int> caminhoP;
        std::vector<int> caminhoM;
        std::string motivoFim;
        int kitsRestantes;
        int posFinalP;
        int posFinalM;
        bool minotauroVivo;
    };

    ResultadoSimulacao run(unsigned int seed, int chanceBatalha);
    void imprimirInicioSimulacao();


private:
    bool prisioneiroBatalha(unsigned int seed, int chanceBatalha, std::mt19937& gerador);
    void turnoPrisioneiro(Prisioneiro& p);
    void turnoMinotauro(Minotauro& m, int posPrisioneiro, std::mt19937& gerador);
    void verificaEstados(Prisioneiro& p, Minotauro& m, bool& fimDeJogo, bool& minotauroVivo, std::string& motivoFim, unsigned int seed, int chanceBatalha, std::mt19937& gerador);

    void imprimirEstadoAtual(int tempoGlobal, const Prisioneiro& p, const Minotauro& m);
    void imprimirFimSimulacao(const std::string& motivoFim, const Prisioneiro& p, const Minotauro& m);

    Grafo labirinto;
    
    int nA;
    int nV;
    int vEntr;
    int vSaid;
    int posIniM;
    int percepcaoMinotauro;

    int kitsDeComida;

    // Estados da simulação
    double tempoGlobal;
    double prxMovP;
    double prxMovM;
};