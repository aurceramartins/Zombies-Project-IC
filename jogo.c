/*
 * This file is part of "2º Projeto de Introdução à Computação 2017/2018"
 * (2oPIC1718).
 *
 * 2oPIC1718 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * 2oPIC1718 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 2oPIC1718. If not, see <http://www.gnu.org/licenses/>.
 * */

/**
 * @file
 * This file is an example of: a) how to use the API defined in the
 * showworld.h header (mandatory for the project); and, b) how to use the
 * concrete simple implementation of the API (provided by the
 * showworld_simple.c file).
 *
 * @author Alejandro Urcera, Andre Cosme, Frederico Placido.
 * @date 2018
 * @copyright [GNU General Public License version 3(GPLv3)](http://www.gnu.org/licenses/gpl.html)
 * */
#include "showworld.h"
#include "agent.h"
#include "world.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "ini.h"

/**Struct de agents onde imos a mexer os agentes que van a estar depois na 
 * grelha do jogo.
 */
struct agentID {
    int x;
    int y;
    unsigned int id;
    AGENT_TYPE type;
    unsigned int ply;
};

/**
 * Struct para recolher as variabeis do ficheiro ini.h 
 */
typedef struct {
    int xdim;
    int ydim;
    int nzombies;
    int nhumans;
    int nzplayers;
    int nhplayers;
    int maxturns;
} configuration;



/**
 * Funcao que crea o movemento toroidal da grelha, se um agente esta num lado
 *  da grelha pasa para o lado oposto.
 * @param x
 * @param y
 * @param toro
 * @param na
 */
void toroidal(int *x, int *y, int *toro, int *na);

/**
 * Funcao intermedia entre distancia e toroidal que recolhe a posicao do agente
 * na grelha para indicar as outras funcoes o tipo de movemento que deven fazer. 
 * @param xN
 * @param yN
 * @param xNovo
 * @param yNovo
 * @param movein
 * @param movedir
 */
void MoveToroidal(int xN, int yN, int xNovo, int yNovo, int *movein, int *movedir);


/**
 * funcao distancia verifica a distancia entre dois agentes e realiza o 
 * movemento do humano ou zombie con as suas caracteristicas
 * @param x
 * @param y
 * @param xNovo
 * @param yNovo
 * @param typeA
 * @param agTypeAnt
 * @param toro
 * @param w
 * @param na
 * @param apagar
 * @param agents
 * @param nagents
 */
void distancia(int *x, int *y, int xNovo, int yNovo, int typeA,
        AGENT_TYPE *agTypeAnt, int toro, WORLD *w, int na, int *apagar,
        struct agentID *agents, int nagents);


/* This function is an implementation of the definition provided by the
 * ::get_agent_info_at() function pointer. It only works for AGENT and WORLD
 * example structures defined in this file. */
unsigned int example_get_ag_info(void *w, unsigned int x, unsigned int y);

/**
 * funcao que vai leer o ficheiro ini.h 
 * @param user
 * @param section
 * @param name
 * @param value
 * @return 
 */
static int handler(void* user, const char* section, const char* name,
        const char* value);


int na1;

/**
 * This `main` function is only an example of: a) how to use the API defined in
 * the showworld.h header (mandatory for the project); and, b) how to use the
 * concrete simple implementation of the API (provided in the
 * showworld_simple.c file).
 *
 * @return Always zero.
 * */
int main(int argc, char **argv) {
    /**inicialicamos o world*/
    WORLD *w = NULL;

    /**inicializamos struct para leer do ini*/
    configuration config;
    if (ini_parse("config.ini", handler, &config) < 0) {
        printf("N�o foi poss�vel encontrar o ficheiro 'config.ini'\n");
        return 1;
    }

    /**numero agentes formado por humas e zombies*/
    int nagents = config.nhumans + config.nzombies;

    /**Inicialicamos struct de agentes a 0*/
    struct agentID agents[nagents];

    for (int i = 0; i < nagents; i++) {
        agents[i].x = 0;
        agents[i].y = 0;
        agents[i].id = 0;
        agents[i].type = 0;
        agents[i].ply = 0;
    }

    /**Criamos o showworld e pasamoslhe x y da grelha e a info do agente
     * chamando a funcao showworld_new que vai fazer um malloc.
     * Tamben vai a gerar a janela da libreria raylib.*/
    SHOWWORLD *sw = showworld_new(config.xdim, config.ydim, example_get_ag_info);

    /**Criamos o World pasandolhe o x e o y definidos no ini chamando a world
     *  new que vai fazer */
    w = world_new(config.xdim, config.ydim);

    /** Funcao para fazer numeros aleatorios que nao correm 
     * predeterminadamente. */
    srand(time(NULL));

    /** Recorremos a variavel nagents que van a ser o noso numero de agentes na
     *grelha e os guardaremos na grelha e na struct de agentes*/
    for (int i = 0; i < nagents; i++) {
        int x, y;
        AGENT_TYPE at;

        /**Damos um valor aleatorio a x e y para metelos na grelha*/
        x = (rand() % config.xdim);
        y = (rand() % config.ydim);

        /** Se no x e o y que recolhe a grelha nao esta nenhum agente entao 
         * metemos um, resta um valor a variavel para que nao saltemos nenhm
         * valor de agente */
        if (world_get(w, x, y) == NULL) {
            /**Recolhemos os humanos e zombies que vamos ter na grelha, que os
             vamos a definir no ini.h */
            if (i >= 0 && i < config.nhumans) {
                at = Human;
            } else if (i >= config.nhumans && i < nagents) {
                at = Zombie;
            }

            /**Criamos uma variavel random para dezir se o agente e controlavel
             con setas ou IA*/
            unsigned int playable = (rand() % 10 == 0);

            /**Criamos o agente novo pasandolhe typo (human/zombie) id e
             * playavel se o agente e jogavel ou nao*/
            AGENT *a = agent_new(at, i, playable);
            /**Metemos o agent no world pasandolhe X, Y e o agente, que o 
             pasaremos como um item para que seja uma funcao mais generica*/
            world_put(w, x, y, (ITEM *) a);

            /**Agora pasaremoslhe as mesmas cosas o noso agent da struct, e 
             * dicir, duplicamos a informacao para logo poder mexer na struc
             * e pasar o movemento na grelha*/
            agents[i].id = i;
            agents[i].x = x;
            agents[i].y = y;
            agents[i].type = at;
            agents[i].ply = playable;
        } else {
            i--;
        }

    }

    /** Fazemos um update do world para que se veia o primeiro turno como e que
     *  empeca a partida*/
    showworld_update(sw, w);
    printf("Pressione ENTER para o seguinte turno...");
    getchar();
    printf("\n");

    /** Criamos um sistema de turnos que vai ir de 0 a 1000*/
    for (int turn = 0; turn < config.maxturns - 1; turn++) {
        /**Funcao que faz o shuffle da struct onde cada turno, vai a ordear
         * aleatoriamente os elementos da estruct de forma que os movementos
         * dos agentes nao seran iguais em nenhum turno*/
        for (int i1 = nagents - 1; i1 > 0; i1--) {
            int index = rand() % i1;
            struct agentID temp = agents[index];
            agents[index] = agents[i1];
            agents[i1] = temp;
        }

        //Imprime o shuffle
        printf("Shuffle: ");
        for (int i2 = 0; i2 < nagents - 1; i2++) {
            printf("id = %d ", agents[i2].id);
        }
        printf("\n\n");

        /**Inicializacao de variaveis
         * A variavel move vai ser para realicar a vecinhanca de moore.
         */
        int move;
        /** XYprincipal sao variaveis para guardar as posicaos iniciais dos
         *  agentes antes de mexer de posicao, serve por se nao precisamos
         *  de mudar a posicao do agente*/
        int xPrincipal = 0;
        int yPrincipal = 0;
        /**xyNovo sao variaveis para guardar a posicao que vamos a mexer para
         *  poder aumentar o diminuir o x e o y*/
        int xNovo = 0;
        int yNovo = 0;
        /** A variaavel aNovo serve para pesquisar pelos agentes que 
         * encontramos coa  vecinhanca de moore */
        int aNovo = 0;

        /** Pesquisamos os agentes na nosa struct de agentes para encontrar co
         *  x e o y dos agentes da struc os agentes na posicao da grelha x e y*/
        for (int na = 0; na < nagents - 1; na++) {
            /**actualicamos as variaveis coas posicoes que precisamos para
             *  realicar os movementos */
            xPrincipal = agents[na].x;
            yPrincipal = agents[na].y;
            xNovo = agents[na].x;
            yNovo = agents[na].y;
            /**typeA sera uma variavel usada para guardar o type dos agentes
             *  que encontramos na nosa struct*/
            int typeA = 0;
            /**inicialicamos a variavel toro para depois usala para a funcao
             *  que realica o movemento toroidal*/
            int toro = 0;
            /**apagar e uma bandeira para realicar a actualizacao do agente que
             *  se vai a mexer, se movemos o agente sem apagar a posicao
             *  anterior ficaria duplicado na posicao que esta e na posicao
             *  na que se mexeu*/
            int apagar = 0;
            /** move toma valor 3 para comecar a pesquisar na vecinhanca de 
             * moore na coordenada superior a onde o agente que pesquisa 
             * esta na grelha*/
            move = 3;
            /**variavel para guardar as letras de teclado que nos vamos a usar
             *  para mexer aos agentes playable*/
            int seta;
            /** xyMexe serao as variaveis que nos mexeremos coas setas*/
            int xMexe = 0;
            int yMexe = 0;

            /** Se o agente e controlavel mexeremos com as setas*/
            if (agents[na].ply == 1) {
                /**Fazemos o while para fazer um loop no que so saldra se
                 *  prememos as setas do pad*/
                while (seta != 1 || seta != 2 || seta != 3 || seta != 4 ||
                        seta != 6 || seta != 7 || seta != 8 || seta != 9) {
                    /**Fazemos um scanf para recolher a tecla premida 
                     * do teclado*/
                    printf("Usa as setas do pad para moverte\n");
                    scanf("%d", &seta);
                    printf("Usa as setas do pad para moverte %d\n", seta);
                    /** Igualamos a posicao principal do agente que vamos a
                     *  mexer coas setas, esta posicao sera a que mudaremos
                     *  coas setas que nos premamos a north south est ou west*/
                    xMexe = agents[na].x;
                    yMexe = agents[na].y;
                    switch (seta) {
                        case 1:
                            xMexe--;
                            yMexe++;

                            break;
                        case 2:
                            yMexe++;
                            break;
                        case 3:
                            xMexe++;
                            yMexe++;
                            break;
                        case 4:
                            xMexe--;
                            break;
                        case 6:
                            xMexe++;
                            break;
                        case 7:
                            xMexe--;
                            yMexe--;
                            break;
                        case 8:
                            yMexe--;
                            break;
                        case 9:
                            xMexe++;
                            yMexe--;
                            break;
                    }
                    /**chamamos a funcao toroidal por se o noso agente que
                     *  vamos a mexer esta nos bordes e ten que aparecer no
                     *  lado contrario da grelha*/
                    toroidal(&xMexe, &yMexe, &toro, &na);

                    /**Se a posicao a que nos vamos a mexer esta vacia 
                     * guardamos a posicao nova do agente e igualamos apagar
                     *  a 1 para depois apagar a posicao principal do agente*/
                    if (world_get(w, xMexe, yMexe) == NULL) {
                        agents[na].x = xMexe;
                        agents[na].y = yMexe;
                        apagar = 1;

                    } else {
                        /**Se ha um agente na posicao na que nos vamos a mexer,
                         *  recorremos a struc de agentes para identificar o
                         *  agente desa posicao*/
                        for (aNovo = 0; aNovo < nagents; aNovo++) {
                            /**Depois, perguntamos se o agente da grelha ten 
                             * tipo distinto ao nosso agente*/
                            if (xMexe == agents[aNovo].x &&
                                    yMexe == agents[aNovo].y &&
                                    agents[na].type != agents[aNovo].type) {
                                /**Depois se nos somos um Zombie e o agente 
                                 * encontrado e um humano, fazemos a infeccao*/
                                if (agents[aNovo].type == Human &&
                                        agents[na].type == 2) {
                                    /**Mudamos o type do humano para zombie*/
                                    agents[aNovo].type = 2;
                                    /**chamamos o agente que vamos a mudar para
                                     *  zombie na grelha na posicao x e y*/
                                    AGENT *a2 = world_get(w, agents[aNovo].x,
                                            agents[aNovo].y);
                                    /**chamamos a funcao mudar agent type 
                                     * mandandolhe o tipo zombie e o agente 
                                     * para mudar*/
                                    mudar_agent_type(agents[aNovo].type,
                                            (AGENT *) a2);
                                    /**metemos o agente mudado no world*/
                                    world_put(w, agents[aNovo].x,
                                            agents[aNovo].y, (ITEM *) a2);
                                }
                            }
                        }
                    }
                    /**quando realicamos o movemento das setas, saimos dos for*/
                    goto movela;
                }
            } else {
                /**  Se o agente nao e controlavel faremos a IA*/
                /**Realizamos a funcao de pesquisar en caracol sem pasar de 
                 * novo pelas posicoes ja pesquisadas*/
                for (int i = 1; i <= ((config.xdim + config.ydim) / 4); i++) {
                    for (int j = 1; j <= 2; j++) {
                        /**Coa variavel move pesquisara o raio do caracol para 
                         * aumentar ou diminuir a pesquisa no x e y*/
                        if (move == 4) {
                            move = 1;
                        } else move += 1;
                        /**Agora  so temos que mudar as posicoes para que 
                         * realice bem a pesquisaF*/
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
                             * estuvera perto dos bordes*/
                            toroidal(&xNovo, &yNovo, &toro, &na);
                            /**Se a posicao pesquisada e distinta de null e que
                             *  ha um agente nessa posicao*/
                            if (w->grid[yNovo * w->xdim + xNovo] != NULL) {
                                /**percorremos a nosa struct de agentes para 
                                 * encontrar o agente pesquisado*/
                                for (aNovo = 0; aNovo < nagents; aNovo++) {
                                    /**Se o tipo do agente e distinto o noso 
                                     * teremos que fazer distintos movementos*/
                                    if (xNovo == agents[aNovo].x &&
                                            yNovo == agents[aNovo].y &&
                                            agents[na].type != agents[aNovo].type) {
                                        /**Se o agente e um zombie guardamos o
                                         * seu type para logo mandar a 
                                         * informacao a funcao distancia*/
                                        if (agents[na].type == Zombie) {
                                            typeA = 1;
                                        }
                                        /**Uma vez todo pesquisado saimos
                                         * dos for*/
                                        goto movela;

                                    }

                                }
                            }
                        }
                    }
                }
            }
            /**movela e a onde o noso programa vai a ir quando pesquisemos 
             * todo o que necesitamos nos for anteriores*/
movela:
            /**funcao que vai fazer todos os movementos da IA*/
            if (agents[na].ply == 0) {
                distancia(&agents[na].x, &agents[na].y, agents[aNovo].x,
                        agents[aNovo].y, typeA, &agents[na].type, toro, w,
                        na, &apagar, agents, nagents);
            }
            /**Uma vez realicados os movementos modificamos o agent que 
             * meteremos no world coas novas posicoes e types*/
            AGENT *a1 = agent_new(agents[na].type, agents[na].ply, agents[na].id);
            /**Se apagar e igual a 1 quuere dezir que o agente vaise a mover e 
             * ten que apagar a sua posicao anterior para que nao seja
             *  duplicado*/
            if (apagar == 1) {
                world_apagar(w, xPrincipal, yPrincipal);
            }
            /**Metemos o agent na nova posicao do world*/
            world_put(w, agents[na].x, agents[na].y, (ITEM *) a1);
            printf("Pressione ENTER para o seguinte turno...");
            getchar();
            /**Actualicamos o ecra do terminal ou da libreria*/
            showworld_update(sw, w);

        }
    }
    /**Libertamos a memoria de showworld, dos items do world e o world*/
    showworld_destroy(sw);
    world_destroy_full(w, (void (*)(ITEM *))agent_destroy);
    return 0;
}

/**
 * Funcao para leer as variaveis do ini.h
 * @param user
 * @param section
 * @param name
 * @param value
 * @return 
 */
static int handler(void* user, const char* section, const char* name,
        const char* value) {
    configuration* pconfig = (configuration*) user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("dim", "xdim")) {
        pconfig->xdim = atoi(value);
    } else if (MATCH("dim", "ydim")) {
        pconfig->ydim = atoi(value);
    } else if (MATCH("nInit", "nzombies")) {
        pconfig->nzombies = atoi(value);
    } else if (MATCH("nInit", "nhumans")) {
        pconfig->nhumans = atoi(value);
    } else if (MATCH("nControl", "nzplayers")) {
        pconfig->nzplayers = atoi(value);
    } else if (MATCH("nControl", "nhplayers")) {
        pconfig->nhplayers = atoi(value);
    } else if (MATCH("turns", "maxturns")) {
        pconfig->maxturns = atoi(value);
    } else {
        return 0; /* unknown section/name, error */
    }
    return 1;
}

/**
 * This function is an implementation of the ::get_agent_info_at() function
 * definition. It only works for ::AGENT and ::WORLD structures defined in this
 * example.
 *
 * It basically receives a pointer to a ::WORLD structure, obtains the AGENT
 * structure in the given coordinates, and returns the agent information in a
 * bit-packed `unsigned int`.
 *
 * @note This is an example which will probably not work in a fully functional
 * game. Students should develop their own implementation of
 * ::get_agent_info_at() and agent/world data structures.
 *
 * @param w Generic pointer to object representing the simulation world.
 * @param x Horizontal coordinate of the simulation world from where to fetch
 * the agent information.
 * @param y Vertical coordinate of the simulation world from where to fetch
 * the agent information.
 * @return An integer containing bit-packed information about an agent, as
 * follows: bits 0-1 (agent type), bit 2 (is agent playable), bits 3-18 (agent
 * ID). Bits 19-31 are available for student-defined agent extensions.
 * */
unsigned int example_get_ag_info(void *w, unsigned int x, unsigned int y) {

    /* The agent information to return. */
    unsigned int ag_info = 0;

    /* Convert generic pointer to world to a WORLD object. */
    WORLD *my_world = (WORLD *) w;

    /* Check if the given (x,y) coordinates are within bounds of the world. */
    if ((x >= my_world->xdim) || (y >= my_world->ydim)) {

        /* If we got here, then the coordinates are off bounds. As such we will
           report that the requested agent is of unknown type. No need to
           specify agent ID or playable status, since the agent is unknown. */
        ag_info = Unknown;

    } else {

        /* Given coordinates are within bounds, let's get and pack the request
           agent information. */

        /* Obtain agent at specified coordinates. */
        AGENT *ag = (AGENT *) world_get(my_world, x, y);

        /* Is there an agent at (x,y)? */
        if (ag == NULL) {

            /* If there is no agent at the (x,y) coordinates, set agent type to
               None. No need to specify agent ID or playable status, since
               there is no agent here. */
            ag_info = None;

        } else {

            /* If we get here it's because there is an agent at (x,y). Bit-pack
               all the agent information as specified by the get_agent_info_at
               function pointer definition. */
            ag_info = (ag->id << 3) | (ag->playable << 2) | ag->type;

        }

    }

    /* Return the requested agent information. */
    return ag_info;

}

/**
 *  Funcao toroidal, que basicamente o que fai e que se o agente esta numa
 *  posicao de qualqueira dos bordes o envia ao outro lado da grelha
 * 
 *  tambem modifica a varievel toro que depois servira para saber a distancia
 *  entre dous agentes a traves da grelha toroidal
 * 
 * @param x 
 * @param y
 * @param toro
 * @param na
 */

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

/**
 * MoveToroidal recolhe a posicao dos agentes a traves da gralha toroidal,
 * dependendo da posicao enviara informacao a distancia para mexer para um
 * dos lados da grelha toroidalmente.
 * @param xN
 * @param yN
 * @param xNovo
 * @param yNovo
 * @param movein
 * @param movedir
 */

void MoveToroidal(int xN, int yN, int xNovo, int yNovo, int *movein,
        int *movedir) {

    int dir = *movedir;
    /** Se a posicao do x e y estan nos quatro bordes ou nas quatro diagonais 
     * da grelha envia uma instruccao a distancia para que realize os
     * diferentes direccoes*/

    if ((xN - xNovo > 10) || (xN - xNovo < -10)) {
        *movein = 1;
        dir = 0;
    } else if ((yN - yNovo > 10) || (yN - yNovo < -10)) {
        *movein = 1;
        dir = 1;
    } else if (((xN - xNovo > 10) || (xN - xNovo < -10)) &&
            ((yN - yNovo > 10) || (yN - yNovo < -10))) {
        *movein = 1;
        dir = 2;
    }

    *movedir = dir;
}

/**
 * Funcao que realica os movementos dos agentes mudando as variaveis,
 *  dependendo de se sao humanos, zombies ou se estan nos bordes para fazer
 *  movementos toroidais.
 * 
 * Tambem realiza a infeccao dos zombies.
 * 
 * @note E uma funcao feia pero ao mesmo tempo funcional e funciona
 * perfeitamente
 * @param x coordenada x do noso agente
 * @param y coordenada y do noso agente
 * @param xNovo coordenada x do agente pesquisado coa vecinhanca de moore
 * @param yNovo coordenada y do agente pesquisado coa vecinhanca de moore
 * @param typeA type do nosso agente
 * @param agTypeAnt type do agente pesquisado pela v. moore
 * @param toro flag para saber se tamos numa posicao que realizaremos um 
 * movemento toroidal
 * @param w o nosso world
 * @param na numero do agente noso na nossa struct
 * @param apagar flag para saber se o noso agente realizara movemento e 
 *  saber se teremos que apagar a sua antiga posicao
 * @param agents apontador para a nossa struct
 * @param nagents numero de agentes para realicar a infeccao e mudanca de type
 */

void distancia(int *x, int *y, int xNovo, int yNovo, int typeA,
        AGENT_TYPE *agTypeAnt, int toro, WORLD *w, int na, int *apagar,
        struct agentID *agents, int nagents) {
    /**Guardamos a posicao que vamos a mudar do noso agente*/
    int xN = *x;
    int yN = *y;
    /**variavel que vai a fazer o movemento do humano ou do zombie para atacar
     *  ou fugir dependendo das distintas opcoes*/
    int movein = 1;
    /**movedir vai indicar en que posicao da grelha esta o noso agente para 
     * realicar os distintos movementos de forma toroidal*/
    int movedir = 4;

    MoveToroidal(xN, yN, xNovo, yNovo, &movein, &movedir);

    if (movedir == 4) {
        movein = -1;
    }

    switch (movedir) {
            /**Verificar en que borde toroidal esta
             este esta no toroidal que movese na x*/
        case 0:
            /**Movemento do Zombie*/
            if (typeA == 1) {
                /**Move se o x e y do noso agente sao maiores ou menores que o
                 *  agente pesquisado coa vecinhanca de moore*/
                if (xN > xNovo && yN > yNovo) {
                    xN += movein;
                    yN -= movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN -= movein;
                    yN += movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN < xNovo && yN > yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN += movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN -= movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN += movein;
                }
            } else {
                /**Movemento do Humano*/

                /**Move se o x e y do noso agente sao maiores ou menores que o
                 *  agente pesquisado coa vecinhanca de moore*/
                if (xN > xNovo && yN > yNovo) {
                    xN -= movein;
                    yN += movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN += movein;
                    yN -= movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN < xNovo && yN > yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN += movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN += movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN -= movein;
                }
            }
            break;
            /**Verificar en que borde toroidal esta
            este esta no toroidal que movese na y*/
        case 1:
            /**Movemento do Zombie*/
            if (typeA == 1) {
                /**Move se o x e y do noso agente sao maiores ou menores que o
                 *  agente pesquisado coa vecinhanca de moore*/
                if (xN > xNovo && yN > yNovo) {
                    xN -= movein;
                    yN += movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN += movein;
                    yN -= movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN < xNovo && yN > yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN += movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN -= movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN += movein;
                }
            } else {
                /**Movemento do Humano*/
                /**Move se o x e y do noso agente sao maiores ou menores que o
                 *  agente pesquisado coa vecinhanca de moore*/
                if (xN > xNovo && yN > yNovo) {
                    xN += movein;
                    yN -= movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN -= movein;
                    yN += movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN < xNovo && yN > yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN += movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN += movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN -= movein;
                }
            }
            break;
            /**Verificar en que borde toroidal esta
            este esta no toroidal que movese na x e y no mesmo tempo*/
        case 2:
            /**Movemento do Zombie*/
            if (typeA == 1) {
                /**Move se o x e y do noso agente sao maiores ou menores que o
                 *  agente pesquisado coa vecinhanca de moore*/
                if (xN > xNovo && yN > yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN += movein;
                    yN -= movein;
                } else if (xN < xNovo && yN > yNovo) {
                    xN -= movein;
                    yN += movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN += movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN -= movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN += movein;
                }
            } else {
                /**Movemento do Humano*/
                /**Move se o x e y do noso agente sao maiores ou menores que o
                 *  agente pesquisado coa vecinhanca de moore*/
                if (xN > xNovo && yN > yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN -= movein;
                    yN += 1;
                } else if (xN < xNovo && yN > yNovo) {
                    xN += 1;
                    yN -= movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN += movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN += movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN -= movein;
                }
            }
            break;
            /**Verificar en que borde toroidal esta
            caso que nao se mexa em toroidal*/
        case 4:
            /**Movemento do Zombie*/
            if (typeA == 1) {
                /**Move se o x e y do noso agente sao maiores ou menores que o
                 *  agente pesquisado coa vecinhanca de moore*/
                if (xN > xNovo && yN > yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN += movein;
                    yN -= movein;
                } else if (xN < xNovo && yN > yNovo) {
                    xN -= movein;
                    yN += movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN += movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN -= movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN += movein;
                }
            } else {
                /**Movemento do Humano*/
                /**Move se o x e y do noso agente sao maiores ou menores que o
                 *  agente pesquisado coa vecinhanca de moore*/
                if (xN > xNovo && yN > yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN -= movein;
                    yN += movein;
                } else if (xN < xNovo && yN > yNovo) {
                    xN += movein;
                    yN -= movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN += movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN += movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN -= movein;
                }
            }
            break;
    }
    /**Chamamos a funcao toroiidal para realicar movementos em toroidal se
     *  fora preciso*/
    toroidal(&xN, &yN, &toro, &na);


    /**Se a posicao a que queremos movernos esta vacia guardamos o x e o
     *  y na nova posicao*/
    if (world_get(w, xN, yN) == NULL) {

        *x = xN;
        *y = yN;
        *apagar = 1;

    } else {
        /**Se a posicao a que vai moverse o agente esta ocupada por outro 
         * agente temos que fazer umas comprobacoes*/
        int aNovo;
        /**buscamos o novo agente na nosa struct*/
        for (aNovo = 0; aNovo < nagents; aNovo++) {
            /**Pergumtamos se o nosso agente e o encontrado tenhen
             *  distinto type*/
            if (xN == agents[aNovo].x && yN == agents[aNovo].y &&
                    agTypeAnt != agents[aNovo].type) {
                /**Se o noso type e Zombie e o novo e humano infectamos*/
                if (agents[aNovo].type == Human && *agTypeAnt == 2) {
                    /**mudamos o type do agente novo para zombi*/
                    agents[aNovo].type = 2;
                    /**recolhemos o agente novo na grelha do world*/
                    AGENT *a2 = world_get(w, agents[aNovo].x, agents[aNovo].y);
                    /**Mudamos o type do agente con esta funcao*/
                    mudar_agent_type(agents[aNovo].type, (AGENT *) a2);
                    /**mandamos a informacao nova do agente a posicao da grelha
                     *  na que estava ese mesmo*/
                    world_put(w, agents[aNovo].x, agents[aNovo].y, (ITEM *) a2);
                    free(a2);

                }
            }
        }
    }
}






