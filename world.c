#include "world.h"
#include <stdlib.h>

WORLD *world_new(unsigned int xdim, unsigned int ydim) {

    WORLD *wrld = NULL;
    wrld = malloc(sizeof (WORLD));
    wrld->grid = calloc(xdim * ydim, sizeof (ITEM));
    wrld->xdim = xdim;
    wrld->ydim = ydim;
    return wrld;
}

/* Esta funcao liberta a memoria alocada para o mundo. */
void world_destroy(WORLD *wrld) {
    free(wrld->grid);
    free(wrld);
}

/* Esta funcao liberta a memoria alocada para o mundo e para todos os itens
 * existentes no mundo. */
void world_destroy_full(WORLD *wrld, void (*item_destroy)(ITEM *item)) {
    for (unsigned int i = 0; i < wrld->xdim * wrld->ydim; ++i) {
        if (wrld->grid[i] != NULL) {
            item_destroy(wrld->grid[i]);
        }
    }
    world_destroy(wrld);
}

void world_apagar(WORLD *wrld, unsigned int x, unsigned int y) {
    wrld->grid[y * wrld->xdim + x] = NULL;

}

void world_put(WORLD *wrld, unsigned int x, unsigned int y, ITEM *item) {

    wrld->grid[y * wrld->xdim + x] = item;

}

ITEM *world_get(WORLD *wrld, unsigned int x, unsigned int y) {

    return wrld->grid[y * wrld->xdim + x];
}

