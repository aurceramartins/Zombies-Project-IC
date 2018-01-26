#include "agent.h"
#include <stdlib.h>

AGENT *agent_new(AGENT_TYPE type, unsigned short id, unsigned char playable) {
    AGENT *agent = NULL;
    agent = malloc(sizeof (AGENT));
    agent->type = type;
    agent->id = id;
    agent->playable = playable;
    return agent;
}

void mudar_agent_type(AGENT_TYPE type, AGENT *agent) {
    agent->type = type;
    return agent;
}
void agent_destroy(AGENT *agent) {
    free(agent);
}

