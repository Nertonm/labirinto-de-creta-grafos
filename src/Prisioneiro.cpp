/**
 * @file Prisioneiro.cpp
 * @author Thiago Nerton
 * @brief Implementação da classe Prisioneiro.
 * @details Este arquivo contém a lógica de programação para os métodos da classe
 * Prisioneiro, incluindo sua movimentação, gerenciamento de suprimentos e a
 * estratégia de backtracking usando o novelo de lã.
 */

#include "labirinto/Prisioneiro.h"
#include "utils/Logger.h"
#include "estruturas/Novelo.h"
#include <iostream>
#include <iomanip>

/**
 * @brief Verifica se um vértice já foi visitado pelo prisioneiro.
 * @param vertice O ID do vértice a ser verificado.
 * @return `true` se o vértice já foi visitado, `false` caso contrário.
 */
bool Prisioneiro::foiVisitado(int vertice) const {
    return vertice >= 0 && vertice < (int)visitados.size() ? visitados[vertice] : false;
}

/**
 * @brief Construtor da classe Prisioneiro.
 * @details Inicializa o estado do prisioneiro, definindo sua posição inicial,
 * quantidade de comida, marcando a primeira sala como visitada e iniciando o
 * rastro do novelo de lã.
 * @param salaInicial O vértice onde o prisioneiro inicia.
 * @param kitsDeComida A quantidade inicial de kits de comida.
 */
Prisioneiro::Prisioneiro(int salaInicial, int kitsDeComida) {
    pos = salaInicial;
    this->kitsDeComida = kitsDeComida;
    visitados.resize(TamanhoDoNovelo::tamanhoNovelo, false);
    visitados[pos] = true;

    caminho.push_back(pos);
    novelo.criarRastro({pos, 0});
}

/**
 * @brief Obtém a posição atual do prisioneiro.
 * @return O ID do vértice onde o prisioneiro está localizado.
 */
int Prisioneiro::getPos() const {
    return pos;
}

/**
 * @brief Adiciona um evento ao histórico de acontecimentos do prisioneiro.
 * @param tipo Categoria do evento (ex: "BACKTRACK").
 * @param descricao Uma breve descrição do que aconteceu.
 * @param posicao A sala onde o evento ocorreu.
 * @param tempo O custo em tempo/comida associado ao evento.
 */
void Prisioneiro::registrarAcontecimento(const std::string& tipo, const std::string& descricao, int posicao, int tempo) {
    HistoricoPrisioneiro h{tipo, descricao, posicao, tempo};
    historico.push_back(h);
}

/**
 * @brief Obtém o caminho percorrido pelo prisioneiro até o momento.
 * @return Uma referência constante para o vetor de inteiros representando a sequência de vértices visitados.
 */
const std::vector<int>& Prisioneiro::getCaminho() const {
    return caminho;
}

/**
 * @brief Executa a lógica de movimento do prisioneiro para o próximo turno.
 * @details Prioriza mover-se para uma sala adjacente não visitada. Se todas as
 * salas vizinhas já foram visitadas, tenta realizar o backtracking com `voltarAtras`.
 * O movimento consome kits de comida equivalentes ao peso da aresta.
 * @param vizinhos A lista de adjacências do vértice atual do prisioneiro.
 * @return O custo (peso da aresta) do movimento realizado. Retorna 0 se nenhum movimento for possível.
 */
int Prisioneiro::mover(const listaAdj<MeuPair<int, int>>& vizinhos) {
    auto no_vizinho = vizinhos.get_cabeca();

    if (kitsDeComida <= 0) {
        return 0;
    }

    while (no_vizinho != nullptr)
    {
        int proximo_vertice = no_vizinho->dado.primeiro;
        int peso_aresta = no_vizinho->dado.segundo;

        if (!foiVisitado(proximo_vertice)) {
            if (kitsDeComida < peso_aresta) {
                no_vizinho = no_vizinho->prox;
                continue;
            }
            novelo.criarRastro({pos, peso_aresta});
            pos = proximo_vertice;
            visitados[pos] = true;
            caminho.push_back(pos);
            kitsDeComida -= peso_aresta; 
            return peso_aresta;
        }
        no_vizinho = no_vizinho->prox;
    }

    int peso_volta = voltarAtras();
    return peso_volta;
}

/**
 * @brief Obtém a quantidade de kits de comida restantes.
 * @return A quantidade atual de kits de comida.
 */
int Prisioneiro::getKitsDeComida() const {
    return kitsDeComida;
}

/**
 * @brief Destrutor da classe Prisioneiro.
 */
Prisioneiro::~Prisioneiro() {
}

/**
 * @brief Realiza o movimento de backtracking.
 * @details Usa o "novelo de lã" para retornar à sala anterior de onde veio.
 * Este movimento também consome a mesma quantidade de comida que o caminho de ida.
 * @return O custo do movimento de retorno (peso da aresta). Retorna 0 se o backtracking não for possível.
 */
int Prisioneiro::voltarAtras(){
    // Se todos os vizinhos já foram visitados, faz backtracking
    if (!novelo.nenhumRastro()) {
        MeuPair<int, int> rastroAnterior = novelo.topo();
        // Verifica se há comida para voltar
        if (kitsDeComida < rastroAnterior.segundo) {
            Logger::info(tempoPrisioneiro, "Não pode fazer backtracking para {} (peso: {}, kits restantes: {}) - Motivo: kits insuficientes.", Logger::LogSource::PRISIONEIRO, rastroAnterior.primeiro, rastroAnterior.segundo, kitsDeComida);
            return 0;
        }
        // Executa o backtracking
        novelo.puxarRastro();
        int vertice_volta = rastroAnterior.primeiro;
        pos = vertice_volta;
        caminho.push_back(pos);
        kitsDeComida -= rastroAnterior.segundo; // Consome kits de comida
        Logger::info(tempoPrisioneiro, "Escolha: backtracking para {} (peso: {}, kits restantes: {}) - Motivo: todos vizinhos visitados, rastro disponível e kits suficientes.", Logger::LogSource::PRISIONEIRO, pos, rastroAnterior.segundo, kitsDeComida);
        registrarAcontecimento("BACKTRACK", "Voltou para sala anterior", pos, rastroAnterior.segundo);
        return rastroAnterior.segundo;
    }
    // Caso extremo: sem vizinhos e sem rastro
    Logger::warning(tempoPrisioneiro, "Prisioneiro está engasgado, sem vizinhos e sem rastro!", Logger::LogSource::PRISIONEIRO);
    return 0;
}