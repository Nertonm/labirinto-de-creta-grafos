
### Introdução
Na antiguidade, após uma guerra entre Creta e Atenas, Atenas foi obrigada a enviar periodicamente jovens como
sacrifício para um labirinto, em sinal de submissão. Dentro dele habita o terrível Minotauro, uma criatura faminta, dotada de força descomunal e sentidos aguçados. O labirinto possui uma única entrada e uma saída única. 
A missão é simular a travessia de um entrante no labirinto e o comportamento do Minotauro, que caça o intruso.
Cada um que entra no labirinto pode levar consigo uma espada e alguns kits de alimentos que demoram a estragar
e são leves, como forma de dar falsa esperança aqueles que entram. Neste caso, se há o encontro com o minotauro, então há uma chance pequena de apenas 1% de chance de vitória do intruso. Mesmo em caso de morte do minotauro, a comida levada dura apenas um tempo limitado, onde é necessário escapar do labirinto antes de um certo número de movimentos nele.
Considere o labirinto representado como um grafo G = (V, E), onde vértices representam posições no labirinto e
arestas representam conexões entre duas posições. Dois pontos podem ser acessados por diferentes rotas diretamente, ou seja, através de diferentes arestas. Cada aresta possui um peso associado representando a dificuldade e gasto de tempo para percorrê-la.
Por seu tamanho, o minotauro não consegue utilizar a entrada e saída do labirinto para escapar, mas possui uma
força enorme e é mantido alimentado, a menos quando está próximo da época do sacrifício, passando este a ficar faminto e com raiva. As posições do minotauro e prisioneiro no labirinto são atualizadas a cada passo, onde os movimentos se dão de um vértice a outro adjacente. Se o minotauro encontra alguém, ele o ataca com fúria.
Considere também que o minotauro possui ótima audição e olfato, podendo perceber a posição do entrante caso
este esteja a uma distância menor ou igual a um certo parâmetro d passado como entrada. Neste caso o minotauro
passa a perseguir o entrante com uma velocidade maior, ou seja, passa a se mover dois vértices por vez em direção ao intruso, enquanto o entrante apenas um por vez.
O minotauro conhece todo o labirinto, incluindo as distâncias entre cada ponto, enquanto o entrante desconhece
totalmente o labirinto. Porém, considere que o prisioneiro possui uma ótima memória e um novelo de lã, que pode
utilizar como guia, caso queira retornar a algum ponto do labirinto e consegue identificar por onde já passou. Considere que as arestas podem ser percorridas em qualquer sentido.

### Representação do Grafo
O labirinto é representado como um grafo ponderado, G = (V, E), onde o input é fornecido em um arquivo de texto com o seguinte formato:
- A primeira linha contém um inteiro, n representando o número de vértices.
- A segunda linha contém um inteiro, m representando o número de arestas.
- As linhas seguintes contêm três inteiros, u, v e w, representando uma aresta entre os vértices u e v com peso w.
- A próxima linha contém um inteiro, representando o vértice inicial do prisioneiro.
- A próxima linha contém um inteiro, representando o vértice de saída do labirinto, tendo que ser diferente da entrada.
- A próxima linha contém um inteiro, representando o vértice inicial do minotauro, tendo que ser diferente da entrada e saída.
- A próxima linha contém um inteiro, representando a distância máxima para o minotauro perceber o prisioneiro.
- A próxima linha contém um inteiro, representando a duração do alimento levado pelo entrante.

### Movimentação 
• Entrante:
    – Move-se um vértice por vez;
    – Não conhece o labirinto, mas pode utilizar uma estratégia de exploração, utilizando um novelo de lã para
    retornar por caminhos já percorridos;
    – Se encontrar a saída em qualquer momento, o programa deve indicar que o prisioneiro foi salvo;
    – Se o Minotauro encontrar o prisioneiro em qualquer vértice, então há uma batalha com 1% de chance de
    sobrevivência.
• Minotauro:
    – Move-se normalmente um vértice por vez ao longo do labirinto;
    – Caso detecte o entrante a distância em soma de pesos de arestas menor ou igual ao parâmetro de percepção
    (p(G)), passa a persegui-lo;
    – Durante a perseguição, move-se dois vértices por rodada pelo caminho mínimo em direção ao prisioneiro;
    – Se alcançar o mesmo vértice do entrante antes deste encontrar a saída, o elimina imediatamente com chance
    de 99%.


### Saída
• Relatório contendo:
– A informação de que o prisioneiro escapou ou como ele morreu;
– Tempo restante até acabar a comida (considere apenas números inteiros);
– Sequência de vértices visitados pelo prisioneiro;
– Se ocorreu, o momento da detecção do prisioneiro pelo minotauro e o momento em que o alcançou;
– Caminho percorrido pelo Minotauro durante a perseguição, se ocorreu;
