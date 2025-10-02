#include <fstream>
#include "Grafo.h"
#include "Prisioneiro.h"
#include "Minotauro.h"
#include <random>

class Simulador {
public:
    // O construtor recebe uma referência para o fluxo do arquivo já aberto
    Simulador(std::ifstream& arquivo_entrada);

    void run(unsigned int seed, int chance_de_sobrevivencia);
    bool prisioneiro_morreu_ou_viveu(unsigned int seed, int chance_de_sobrevivencia, std::mt19937& gerador);

private:
    Grafo labirinto;
    // Minotauro cadelao;

public:
    int vEntr;
    int vSaid;
    int posIniM;
    int percepcao_minotauro;

    int kitsDeComida;

    double tempo_global;
    double prxMovP;
    double prxMovM;
};