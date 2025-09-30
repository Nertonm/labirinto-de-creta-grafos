#include "labirinto/Prisioneiro.h"
#include "estruturas/MinhaListaAdj.h"
#include "estruturas/MeuPair.h"
#include "labirinto/Simulador.h"
#include <string>
#include <sstream> // Essencial para o std::stringstream
#include <iostream>
#include <random> // Para geração de números aleatórios


Simulador::Simulador(std::ifstream& arquivo_entrada) {
    std::string linha;
    int nA, nV = 0;
    
    // Lê a primeira linha: número de vértices
    if (std::getline(arquivo_entrada, linha)) {
        std::stringstream ss(linha);
        ss >> nV;
    } else {
        throw std::runtime_error("Erro ao ler o número de vértices.");
    }

    // Lê a segunda linha: número de arestas
    if (std::getline(arquivo_entrada, linha)) {
        std::stringstream ss(linha);
        ss >> nA;
    } else {
        throw std::runtime_error("Erro ao ler o número de arestas.");
    }

    std::cout << "Configurando labirinto com " << nV << " vertices e " << nA << " arestas." << std::endl;

    // Lê as próximas 'nA' linhas: arestas do grafo
    for (int i = 0; i < nA; ++i) {
        if (std::getline(arquivo_entrada, linha)) {
            std::stringstream ss(linha);
            int u, v, peso;
            ss >> u >> v >> peso;
            labirinto.adicionar_aresta(u, v, peso);
        } else {
            throw std::runtime_error("Erro ao ler as arestas do grafo.");
        }
    }

    // Lê Vértice de entrada
    std::getline(arquivo_entrada, linha);
    std::stringstream(linha) >> vEntr;
    std::cout << "Vértice de entrada: " << vEntr << std::endl;

    // Lê Vértice de saída
    std::getline(arquivo_entrada, linha);
    std::stringstream(linha) >> vSaid;
    labirinto.set_saida(vSaid); 

    // Lê Posição inicial do Minotauro
    std::getline(arquivo_entrada, linha);
    std::stringstream(linha) >> posIniM;

    // Lê Parâmetro de percepção
    std::getline(arquivo_entrada, linha);
    std::stringstream(linha) >> percepcao_minotauro;

    // Lê Tempo máximo
    std::getline(arquivo_entrada, linha);
    std::stringstream(linha) >> tmpMax;

    std::cout << "Simulador iniciado com sucesso!" << std::endl;
}


void Simulador::run(unsigned int seed, int chance_de_sobrevivencia) {
    tempo_global = 0;
    prxMovP = 0.0;
    prxMovM = 0.0;

    std::mt19937 gerador(seed);
    
    std::string motivo_fim;
    bool fim_de_jogo = false;
    bool minotauro_vivo = false;
    // bool minotauro_vivo = ture;


    Prisioneiro p(vEntr, tmpMax);
    // Minotauro minotauro(pos_inicial_minotauro, percepcao_minotauro, labirinto);

    while (!fim_de_jogo){
        std::cout << "[Dia: " << tempo_global << "] Prisioneiro está na sala " << p.getPosAtual() 
        << std::endl;

        if (prxMovP <= prxMovM || !minotauro_vivo) {
            int pos_antiga = p.getPosAtual();
            const auto& vizinhos = labirinto.get_vizinhos(p.getPosAtual());

            std::cout << "Vizinhos: ";
            listaAdj<MeuPair<int, int>>::No* no_vizinho = vizinhos.get_cabeca();
            while (no_vizinho) {
                const MeuPair<int, int>& vizinho = no_vizinho->dado;
                std::cout << "(Vertice: " << vizinho.primeiro << ", Peso: " << vizinho.segundo << ") ";
                no_vizinho = no_vizinho->prox;
            }
            std::cout << std::endl;

            p.mover(vizinhos, tmpMax);
            int pos_nova = p.getPosAtual();
            std::cout << "Prisioneiro moveu de " << pos_antiga << " para " << pos_nova << std::endl;

            int peso_aresta = 0;

            const auto& vizinhos_antiga = labirinto.get_vizinhos(pos_antiga);
            listaAdj<MeuPair<int, int>>::No* no_vizinho_antigo = vizinhos_antiga.get_cabeca();
            while (no_vizinho_antigo) {
                const MeuPair<int, int>& vizinho = no_vizinho_antigo->dado;
                if (vizinho.primeiro == pos_nova) {
                    peso_aresta = vizinho.segundo;
                    break;
                }
                no_vizinho_antigo = no_vizinho_antigo->prox;
            }
            tempo_global += peso_aresta;
            prxMovP = tempo_global + peso_aresta;

            std::cout << "[" << tempo_global << "s] Prisioneiro moveu-se para " << pos_nova << 
            ". Próximo movimento em " << prxMovP << "s." << std::endl;
        } else {
            // Movimento do minotauro
        }
        if (tempo_global > tmpMax) {
            motivo_fim += "O prisioneiro morreu de fome no dia " + std::to_string(tempo_global) + 
            " tentando alcançar a sala " + std::to_string(prxMovP) + ".";
            fim_de_jogo = true;
        }
        if (p.getPosAtual() == vSaid) {
            motivo_fim += "O prisioneiro escapou com sucesso!";
            fim_de_jogo = true;
        }
        //if (prisioneiro.getPosAtual() == minotauro.getPosAtual() && minotauro_vivo) {
        if (p.getPosAtual() == 1000){
            motivo_fim = "Prisioneiro foi pego pelo Minotauro.";
            if (prisioneiro_morreu_ou_viveu(seed, chance_de_sobrevivencia, gerador)) {
                motivo_fim += " Mas incrivelmente, o prisioneiro derrotou o Minotauro!";
                minotauro_vivo = false;
            } else {
                motivo_fim += " O prisioneiro virou lanche de Minotauro.";
                fim_de_jogo = true;
            }
            continue;
        }

    }
    std::cout << "Fim de jogo: " << motivo_fim << std::endl;
    std::cout << "Caminho percorrido pelo prisioneiro: ";
    const std::vector<int>& caminho = p.getCaminho();
    for (size_t i = 0; i < caminho.size(); ++i) {
        std::cout << caminho[i];
        if (i < caminho.size() - 1) {
            std::cout << " -> ";
        }
    }
}

bool Simulador::prisioneiro_morreu_ou_viveu(unsigned int seed, int chance_de_sobrevivencia, std::mt19937& gerador) {
    std::uniform_real_distribution<double> distribuidor(1, 100);
    int numero_sorteado = distribuidor(gerador);
    return numero_sorteado <= chance_de_sobrevivencia;
}