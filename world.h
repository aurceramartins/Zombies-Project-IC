#ifndef WORLD_H
#define WORLD_H

typedef void *ITEM;

typedef struct {
    ITEM *grid;
    unsigned int xdim;
    unsigned int ydim;
} WORLD;

WORLD *world_new(unsigned int xdim, unsigned int ydim);
void world_destroy(WORLD *wrld);
void world_destroy_full(WORLD *wrld, void (*item_destroy)(ITEM *item));
void world_apagar(WORLD *wrld, unsigned int x, unsigned int y);


void world_put(WORLD *wrld, unsigned int x, unsigned int y, ITEM * item);
ITEM * world_get(WORLD *wrld, unsigned int x, unsigned int y);

#endif
