#include <fstream>
#include "Grafo.h"
#include "Prisioneiro.h"
#include "Minotauro.h"

class Simulador {
public:
    // O construtor recebe uma referência para o fluxo do arquivo já aberto
    Simulador(std::ifstream& arquivo_entrada);

   // void run();

private:
    Grafo labirinto;
    // Prisioneiro coitado;
    // Minotauro cadelao;

    // Atributos para guardar os parâmetros lidos
    int vertice_entrada;
    int vertice_saida;
    int pos_inicial_minotauro;
    int percepcao_minotauro;
    int tempo_maximo_comida;
};