# Title of project

* Alejandro Urcera Martins 1 - a21703818
* Andre Duarte Cosme  2 - a21702744
* Frederico Placido 3 - a21702188

## Our solution

Jogo de zombies onde utilizámos a linguagem C para criar uma grelha toroidal, na qual em cada uma das suas coordenadas existe a posibilidade de conter um agente, podendo ser zombie ou humano. Estes agentes têm a possibilidade de se mover de forma automática (IA) ou através dos comandos do teclado. 
Para a visualização usámos a livraria Raylib, no SO Windows.
O ficheiro Makefile está configurado para utilizar a livraria Raylib em Windows, não tendo as especificações para Linux, que é uma instrução de uma linha no LDLIBS. 


### Architecture

O jogo está estructurado de maneira a que a visualização esteja separada do resto do código, e a livraria também está implementada no showworld_simple.c, mas queremos pôr outra livraria a funcionar perfeitamente sem realizar nenhuma mudança, apenas as do makefile.

As "calses" do projeto estão estruturadas em três ficheiros, o jogo, que é onde se encontra o main do programa, que realiza as funções básicas e as chamadas das funções das outras clases; o ficheiro agent que retorna os dados dos agentes; e a clase world, que retorna os dados guardados no mundo, os agentes que lá estão e realiza algumas funcões básicas para que os agentes que estão nele realizem bem os movimentos. (Todos estes ficheiros .c têm a sua correspondente .h para realizar as chamadas entre clases.

Depois o ficheiro ini.c realiza a  leitura no ficheiro config para chamar as variáveis pedidas no exercicio e assim poder mudar as regras do jogo, tipo numero de agentes, tamanho da grelha etc.

[Architecture do codigo](JOGO.svg)

### Data structures

O jogo tem as estructuras dos agentes, do world, do simpleshowworld etc, que basicamente guardam os dados dos diferentes tipos, de forma a que quando se quer a informaçao de cada clase so temos que chamar as funçoes caracteristicas da clase e obtemos os dados dessa clase. 
As clases todas têm malloc ou calloc para iniciar a memoria do objeto que queremos criar e no final liberta esta memoria com as funcoes destruct. A maiores os agentes estao definidos como items para que sejao mais genericos, e temos implementada tambem as clases world_get world_put e woorld_destroy_full que realizam as funçoes que o mesmo nome indica, o world_full destroi primeiro os agentes da grelha e despois a grelha do mundo.

Para criar o jogo precisamos de ter a mais da grelha de agentes do world, uma struct que guarde a informaçao dos agentes para poder mexer com eles antes de enviar os dados ao world e a visualizaçao com update_showworld. Tambem realizamos o shuffle dos agentes nesta struct para mudar as posiçoes deles em cada turno.

### Algorithms

O jogo tem dois algoritmos principais, que sao a pesquisa da vizinhança de moore e o movimento toroidal dos agentes e tambem da pesquisa da vizinhança. Depois ha os movimentos dos agentes com a IA, os zombies perseguem os humanos e quando estao a uma  coordenada de distancia infectam e mudam os humanos para zombies. Os humanos fogem dos zombies, se os agentes encontram um agente nas coordenadas onde se vao mexer, em vez disso, ficam parados.
Tambem metemos movimento com as setas para os agentes controlaveis

A vizinhança de moore pesquisa em caracol para nao repetir as mesmas coordenadas ja pesquisadas, e tambem pesquisa atraves da grelha em toroidal, aqui esta o exemplo no código:

```c
/**  Se o agente nao e controlavel fazemos a IA*/
                /**Realizamos a funcao de pesquisar em caracol sem passar de 
                 * novo pelas posicoes ja pesquisadas*/
                for (int i = 1; i <= ((config.xdim + config.ydim) / 4); i++) {
                    for (int j = 1; j <= 2; j++) {
                        /**Com a variavel move pesquisara o raio do caracol para 
                         * aumentar ou diminuir a pesquisa no x e y*/
                        if (move == 4) {
                            move = 1;
                        } else move += 1;
                        /**Agora  so temos que mudar as posicoes para que 
                         * realize bem a pesquisaF*/
                        for (int block = 1; block <= i; block++) {
                            switch (move) {
                                case 1:
                                    // Mexe para a direita i vezes
                                    xNovo += 1;
                                    break;
                                case 2:
                                    // Mexe para baixo i vezes
                                    yNovo += 1;
                                    break;
                                case 3:
                                    xNovo -= 1;
                                    break;
                                case 4:
                                    // mexe para cima i vezes
                                    yNovo -= 1;
                                    break;
                            }
                            /**chamamos a funcao toroidal para pesquisar 
                             * tambem no outro lado da grelha se o agente
                             * estiver perto dos bordes*/
                            toroidal(&xNovo, &yNovo, &toro, &na);
                            /**Se a posicao pesquisada e distinta de null e que
                             *  ha um agente nessa posicao*/
                            if (w->grid[yNovo * w->xdim + xNovo] != NULL) {

```

A funçao toroidal basicamente troca o agente ou a pesquisa do agente quando esta num border ate o border contrario. Aqui esta o codigo principal
```c

void toroidal(int *x, int *y, int *toro, int *na) {
    int ax = *x;
    int ay = *y;

    if (ax > 19) {
        ax = 0;
        if (na1 != *na) {
            if (*(toro) == 0) {
                *(toro) = 1;
            } else {
                *(toro) = 0;
            }
        }
        na1 = *na;
    } else if (ax < 0) {
        ax = 19;
        if (na1 != *na) {
            if (*(toro) == 0) {
                *(toro) = 1;
            } else {
                *(toro) = 0;
            }
        }
        na1 = *na;
    }
    if (ay < 0) {
        ay = 19;
        if (na1 != *na) {
            if (*(toro) == 0) {
                *(toro) = 1;
            } else {
                *(toro) = 0;
            }
        }
        na1 = *na;
    } else if (ay > 19) {
        ay = 0;
        if (na1 != *na) {
            if (*(toro) == 0) {
                *(toro) = 1;
            } else {
                *(toro) = 0;
            }
        }
        na1 = *na;
    }
    if (ay < 0 && ay > 19 && ax < 0 && ax > 19) {
        *(toro) = 0;
        na1 -= 1;
    }

    *x = ax;
    *y = ay;
}

```

## User guide

O jogo e muito simples, cada turno os agentes movem uma posiçao na grelha, ha dois tipos de agentes, zombies e humanos, os zombies têm que perseguir os humanos e infeta-los, e os humanos fogem, se os zombies apanharem todos os humanos os humanos perdem, se nao os apanharem os humanos ganham.

Ha tambem dois tipos de movimentos, os movimentos da IA, e os movimentos que mexeremos com as setas, podemos controlar tanto humanos como zombies.

## Conclusions

O jogo engloba toda a materia dada na aula, ademais de ter que pensar na logica dos movimentos de IA que em algumas ocasioes resulta um reto para a programaçao. Tambem aprendemos a trabalhar em grupo com os companheiros, e a trabalhar no github, a usar uma livraria grafica e tambem a criar makefile etc... 

## References

Conversaçoes formais com o grupo de colegas de Joao Duarte, Ines Gonçalves, Ines Nunes e tambem com o grupo de Leandro Bras, Hugo Martins e Diana Noia.

* <a name="ref1">\[1\]</a> Pereira, A. (2017). C e Algoritmos, 2ª edição. Sílabo.
* <a name="ref2">\[2\]</a> Reagan, P. (2014). [Game Programming in C with the
Ncurses Library](https://www.viget.com/articles/game-programming-in-c-with-the-ncurses-library),
Viget Labs.
* <a name="ref3">\[3\]</a> [2D game development basics (2010)](https://stackoverflow.com/questions/3144399/2d-game-development-basics),
stackoverflow.com

