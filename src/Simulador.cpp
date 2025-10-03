#include "labirinto/Prisioneiro.h"
#include "estruturas/MinhaListaAdj.h"
#include "estruturas/MeuPair.h"
#include "labirinto/Simulador.h"
#include <string>
#include <sstream> // Essencial para o std::stringstream
#include <iostream>
#include <random> // Para geração de números aleatórios

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


void Simulador::imprimirInicioSimulacao() {
    // --- Paleta de Cores Temática ---
    const char* BRONZE = "\033[38;5;172m";
    const char* STONE_DARK = "\033[38;5;238m";
    const char* MINOTAUR_COLOR = "\033[38;5;94m";
    const char* RED_HIGHLIGHT = "\033[1;31m";
    const char* GREEN_INFO = "\033[1;32m";
    const char* GREEN = "\033[1;32m";
    const char* STONE = "\033[38;5;242m";
    const char* RESET = "\033[0m";
    const char* BOLD = "\033[1m";

    // --- Título Principal ---
    std::cout << BRONZE << BOLD << "\n     ╔═════════════════════════════════════════════╗" << RESET << std::endl;
    std::cout << BRONZE << BOLD << "     ║" << RESET << "       " << BRONZE << BOLD << "LABIRINTO DE CRETA - SIMULAÇÃO" << RESET << "        " << BRONZE << BOLD << "║" << RESET << std::endl;
    std::cout << BRONZE << BOLD << "     ╚═════════════════════════════════════════════╝" << RESET << std::endl;

    // --- Arte ASCII do Minotauro (Fornecida pelo usuário e colorizada) ---
    // A arte é impressa linha por linha para permitir a inserção de cores
    std::cout << STONE_DARK << "      _____________ " << BRONZE << "nnnnnnnn nnnnnnnn" << STONE_DARK << " ___________" << RESET << std::endl;
    std::cout << STONE_DARK << "     |             " << BRONZE << "nnnnnnnn    nn" << STONE_DARK << "               |" << RESET << std::endl;
    std::cout << STONE_DARK << "     | -\"\"\\        " << BRONZE << "HHnnnnHP    HH" << STONE_DARK << "               |" << RESET << std::endl;
    std::cout << STONE_DARK << "    .-\"  .`)     (" << MINOTAUR_COLOR << "                              " << STONE_DARK << "|" << RESET << std::endl;
    std::cout << STONE_DARK << "   j   .'_+     :" << MINOTAUR_COLOR << "[                )      " << STONE_DARK << ".^--.. |" << RESET << std::endl;
    std::cout << STONE_DARK << "  i    -\"       " << MINOTAUR_COLOR << "|l                ].    /      i" << STONE_DARK << "|" << RESET << std::endl;
    std::cout << STONE_DARK << " ,\" .:j         " << MINOTAUR_COLOR << "`8o  _,,+.,.--,   d" << STONE_DARK << "|   `:::;    b" << RESET << std::endl;
    std::cout << STONE_DARK << " i  :'|          " << MINOTAUR_COLOR << "\"88p;.  (" << RED_HIGHLIGHT << "-.\"_\"-" << MINOTAUR_COLOR << ".oP" << MINOTAUR_COLOR << "        \\.   :" << RESET << std::endl;
    std::cout << STONE_DARK << " ; .  (            " << MINOTAUR_COLOR << ">,%%%   " << RED_HIGHLIGHT << "f" << MINOTAUR_COLOR << "),):8\"" << MINOTAUR_COLOR << "          \\:'  i" << RESET << std::endl;
    std::cout << STONE_DARK << "i  :: j          " << MINOTAUR_COLOR << ",;%%%:; ; ; i:%%%.," << MINOTAUR_COLOR << "        i.   `." << RESET << std::endl;
    std::cout << STONE_DARK << "i  `: ( ____  " << MINOTAUR_COLOR << ",-::::::' ::j  [:```" << MINOTAUR_COLOR << "          [8:   )" << RESET << std::endl;
    std::cout << STONE_DARK << "<  ..``'::::" << MINOTAUR_COLOR << "8888oooooo.  :(jj(,;,,," << MINOTAUR_COLOR << "         [8::  <" << RESET << std::endl;
    std::cout << STONE_DARK << "`. ``:.      " << MINOTAUR_COLOR << "oo.8888888888:;%%%8o.::.+888+o.:`:'" << STONE_DARK << "  |" << RESET << std::endl;
    std::cout << STONE_DARK << " `.   `        " << MINOTAUR_COLOR << "`o`88888888b`%%%%%88< Y888P\"\"'-" << STONE_DARK << "    ;" << RESET << std::endl;
    std::cout << STONE_DARK << "   \"`---`.       " << MINOTAUR_COLOR << "Y`888888888;;.,\"888b.\"\"..::::'" << STONE_DARK << "-'" << RESET << std::endl;
    std::cout << STONE_DARK << "     |    \"-....  " << MINOTAUR_COLOR << "b`8888888:::::.`8888._::-" << STONE_DARK << "     |" << RESET << std::endl;
    std::cout << STONE_DARK << "     |       `:::. " << MINOTAUR_COLOR << "`:::::O:::::::.`%%'" << STONE_DARK << "|         |" << RESET << std::endl;
    std::cout << STONE_DARK << "     |        `.      " << MINOTAUR_COLOR << "\"``::::::''" << STONE_DARK << "    .'         |" << RESET << std::endl;
    std::cout << STONE_DARK << "     |          `.                   <          |" << RESET << std::endl;
    std::cout << STONE_DARK << "     |            +:         `:   -';           |" << RESET << std::endl;
    std::cout << STONE_DARK << "     |  ____       `:         : .::/            |" << RESET << std::endl;
    std::cout << STONE_DARK << "     | `-|--        ;+_  :::. :..;;;            |" << RESET << std::endl;
    std::cout << STONE_DARK << "     |______________;;;;,;;;;;;;;,;;____________|" << RESET << std::endl;


    // --- Seção de Informações ---
    std::cout << BRONZE << BOLD << "\n            ⚜ " << BOLD << "O DESTINO DO PRISIONEIRO" << RESET << BRONZE << BOLD << " ⚜" << RESET << std::endl;
    std::cout << STONE_DARK << "  ╔════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << STONE_DARK << "  ║ " << RESET << "Ponto de Partida (Entrada):      Sala " << BOLD << vEntr << RESET << std::endl;
    std::cout << STONE_DARK << "  ║ " << GREEN_INFO << "Vislumbre de Liberdade (Saída):  Sala " << BOLD << labirinto.get_saida() << RESET << std::endl;
    std::cout << STONE_DARK << "  ║────────────────────────────────────────────────────────║" << std::endl;
    std::cout << STONE_DARK << "  ║ " << RED_HIGHLIGHT << "Covil da Fera (Minotauro):       Sala " << BOLD << posIniM << RESET << std::endl;
    std::cout << STONE_DARK << "  ║ " << RESET << "Provisões para a Jornada:      " << BOLD << kitsDeComida << " kits" << RESET << std::endl;
    std::cout << STONE_DARK << "  ╚════════════════════════════════════════════════════════╝" << RESET << std::endl;
    
    // --- Seção de Geração: "Os Corredores de Pedra" ---
    std::cout << BRONZE << "\n            🏛  " << BOLD << "Os Corredores de Pedra" << RESET << BRONZE << " 🏛" << RESET << std::endl;
    std::cout << GREEN << "  Gerando uma estrutura com " << BOLD << nV << RESET << GREEN << " salas e " << BOLD << nA << RESET << GREEN << " caminhos..." << RESET << std::endl;
    std::cout << STONE <<  "  ╔════════════════════════════════════════════════════════╗" << std::endl;

    for (int v = 0; v < labirinto.getNumVertices(); ++v) {
        const auto& vizinhos = labirinto.get_vizinhos(v);
        std::cout << STONE << "  ║ " << BRONZE << "• Sala " << BOLD << v << RESET << STONE << " se conecta com:" << RESET << std::endl;
        
        auto no = vizinhos.get_cabeca();
        if (no == nullptr) {
            std::cout << STONE << "  ║    └── Vazio e Silêncio..." << std::endl;
        } else {
            while (no != nullptr) {
                // Determina o conector do galho (└── para o último, ├── para os outros)
                const char* connector = (no->prox == nullptr) ? "└──" : "├──";
                std::cout << STONE << "  ║    " << connector << " Sala " << BOLD << no->dado.primeiro << RESET 
                          << STONE << " (distância: " << no->dado.segundo << ")" << RESET << std::endl;
                no = no->prox;
            }
        }
        if (v < labirinto.getNumVertices() - 1) {
             std::cout << STONE << "  ║╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌║" << std::endl;
        }
    }
    std::cout << STONE <<  "  ╚════════════════════════════════════════════════════════╝" << RESET << std::endl;
    std::cout << "\n" << BRONZE << BOLD << "Que os deuses guiem seus passos. A simulação começou!" << RESET << std::endl;
}

// void Simulador::run(unsigned int seed, int chance_de_sobrevivencia) {
//     tempo_global = 0;

//     // Inicializa os tempos dos próximos movimentos
//     prxMovP = 0.0;
//     prxMovM = 0.0;

//     // Inicializa o gerador de números aleatórios com a seed fornecida
//     std::mt19937 gerador(seed);
    
//     std::string motivo_fim;

//     bool fim_de_jogo = false;
//     bool minotauro_vivo = true;
    

//     Prisioneiro p(vEntr, kitsDeComida);
//     Minotauro m(posIniM, percepcao_minotauro, labirinto, labirinto.getNumVertices());
//     m.lembrarCaminhos();


//     while (!fim_de_jogo){
//         std::cout << "[Dia: " << tempo_global << "] Prisioneiro está na sala " << p.getPos() 
//         << std::endl;

//         if (prxMovP <= prxMovM || !minotauro_vivo) {
//             int pos_antiga = p.getPos();
//             const auto& vizinhos = labirinto.get_vizinhos(p.getPos());

//             std::cout << "Vizinhos: ";
//             listaAdj<MeuPair<int, int>>::No* no_vizinho = vizinhos.get_cabeca();
//             while (no_vizinho) {
//                 const MeuPair<int, int>& vizinho = no_vizinho->dado;
//                 std::cout << "(Vertice: " << vizinho.primeiro << ", Peso: " << vizinho.segundo << ") ";
//                 no_vizinho = no_vizinho->prox;
//             }
//             std::cout << std::endl;

//             p.mover(vizinhos);
//             int pos_nova = p.getPos();
//             std::cout << "Prisioneiro moveu de " << pos_antiga << " para " << pos_nova << std::endl;

//             int peso_aresta = 0;

//             const auto& vizinhos_antiga = labirinto.get_vizinhos(pos_antiga);
//             listaAdj<MeuPair<int, int>>::No* no_vizinho_antigo = vizinhos_antiga.get_cabeca();
//             while (no_vizinho_antigo) {
//                 const MeuPair<int, int>& vizinho = no_vizinho_antigo->dado;
//                 if (vizinho.primeiro == pos_nova) {
//                     peso_aresta = vizinho.segundo;
//                     break;
//                 }
//                 no_vizinho_antigo = no_vizinho_antigo->prox;
//             }
//             tempo_global += peso_aresta;
//             prxMovP = tempo_global + peso_aresta;

//             std::cout << "[" << tempo_global << "] Prisioneiro moveu-se para " << pos_nova << 
//             ". Próximo movimento em " << prxMovP << "s." << std::endl;
//         } else {
//             static std::vector<int> historicoMinotauro;
//             int pos_antiga = m.getPos();
//             int proximoVertice;
//             int distancia = -1;
//             if (pos_antiga >= 0 && pos_antiga < labirinto.getNumVertices() && p.getPos() >= 0 && p.getPos() < labirinto.getNumVertices()) {
//                 distancia = m.lembrarDist(pos_antiga, p.getPos());
//             }
//             if (historicoMinotauro.empty() || historicoMinotauro.back() != pos_antiga) {
//                 historicoMinotauro.push_back(pos_antiga);
//             }
//             if (distancia != -1 && distancia <= percepcao_minotauro) {
//                 // Persegue o prisioneiro pelo menor caminho
//                 int prox = -1;
//                 if (pos_antiga >= 0 && pos_antiga < labirinto.getNumVertices() && p.getPos() >= 0 && p.getPos() < labirinto.getNumVertices()) {
//                     prox = m.lembrarProxPasso(pos_antiga, p.getPos());
//                 }
//                 proximoVertice = (prox != -1) ? prox : pos_antiga;
//             } else {
//                 // Move aleatoriamente para um vizinho
//                 const auto& vizinhos = labirinto.get_vizinhos(pos_antiga);
//                 std::vector<int> opcoes;
//                 for (auto no = vizinhos.get_cabeca(); no != nullptr; no = no->prox) {
//                     opcoes.push_back(no->dado.primeiro);
//                 }
//                 if (!opcoes.empty()) {
//                     std::uniform_int_distribution<size_t> dist(0, opcoes.size() - 1);
//                     proximoVertice = opcoes[dist(gerador)];
//                 } else if (historicoMinotauro.size() > 1) {
//                     // Backtracking: volta ao vértice anterior
//                     historicoMinotauro.pop_back();
//                     if (!historicoMinotauro.empty()) {
//                         proximoVertice = historicoMinotauro.back();
//                     } else {
//                         proximoVertice = pos_antiga;
//                     }
//                 } else {
//                     proximoVertice = pos_antiga; // Não tem para onde voltar
//                 }
//             }
//             m.mover(proximoVertice);
//             int pos_nova = m.getPos();
//             int peso_aresta = 0;
//             const auto& vizinhos_minotauro = labirinto.get_vizinhos(pos_antiga);
//             listaAdj<MeuPair<int, int>>::No* no_vizinho_minotauro = vizinhos_minotauro.get_cabeca();
//             while (no_vizinho_minotauro) {
//                 const MeuPair<int, int>& vizinho = no_vizinho_minotauro->dado;
//                 if (vizinho.primeiro == pos_nova) {
//                     peso_aresta = vizinho.segundo;
//                     break;
//                 }
//                 no_vizinho_minotauro = no_vizinho_minotauro->prox;
//             }
//             prxMovM = tempo_global + peso_aresta;
//             std::cout << "[" << tempo_global << "] Minotauro moveu-se para " << pos_nova << 
//             ". Próximo movimento em " << prxMovM << "s." << std::endl;
//         }
//         if (kitsDeComida < 0) {
//             int pos_nova = p.getPos();
//             motivo_fim += "O prisioneiro morreu de fome no dia " + std::to_string(tempo_global) +
//                 " tentando alcançar a sala " + std::to_string(pos_nova) + ".";
//             fim_de_jogo = true;
//             continue;
//         }
//         if (p.getPos() == vSaid) {
//             motivo_fim += "O prisioneiro escapou com sucesso!";
//             fim_de_jogo = true;
//             continue;
//         }
//         if (p.getPos() == m.getPos() && minotauro_vivo) {
//             motivo_fim = "Prisioneiro foi pego pelo Minotauro.";
//             if (prisioneiro_morreu_ou_viveu(seed, chance_de_sobrevivencia, gerador)) {
//                 motivo_fim += " Mas incrivelmente, o prisioneiro derrotou o Minotauro!";
//                 minotauro_vivo = false;
//             } else {
//                 motivo_fim += " O prisioneiro virou lanche de Minotauro.";
//                 fim_de_jogo = true;
//             }
//             continue;
//         }

//     }
//     std::cout << "Fim de jogo: " << motivo_fim << std::endl;
//     std::cout << "Caminho percorrido pelo prisioneiro: ";
//     const std::vector<int>& caminho = p.getCaminho();
//     for (size_t i = 0; i < caminho.size(); ++i) {
//         std::cout << caminho[i];
//         if (i < caminho.size() - 1) {
//             std::cout << " -> ";
//         }
//     }
// }

// bool Simulador::prisioneiro_morreu_ou_viveu(unsigned int seed, int chance_de_sobrevivencia, std::mt19937& gerador) {
//     std::uniform_int_distribution<int> distribuidor(1, 100);
//     int numero_sorteado = distribuidor(gerador);
//     return numero_sorteado <= chance_de_sobrevivencia;
// }