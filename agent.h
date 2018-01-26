#ifndef AGENT_H
#define AGENT_H
#include "showworld.h"

/**
 * Structure defining agent properties.
 *
 * @note This is an example which will probably not work in a fully functional
 * game. Students should develop their own implementation of
 * ::get_agent_info_at() and agent/world data structures.
 * */
typedef struct {
    AGENT_TYPE type; /**< Agent type.        */
    unsigned char playable; /**< Is agent playable? */
    unsigned short id; /**< Agent ID.          */
} AGENT;

void mudar_agent_type(AGENT_TYPE type, AGENT *agent);
AGENT * agent_new(AGENT_TYPE type, unsigned short id, unsigned char playable);
void agent_destroy(AGENT * agent);

#endif
