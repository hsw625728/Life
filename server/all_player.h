#ifndef __ALL_PLAYER_H__
#define __ALL_PLAYER_H__
#include "type.h"

#define MIN_ALL_PLAYER_COUNT 64
typedef struct
{
	char ID[33];
	char usr[64];
	char pwd[64];
	int sockid;
}AllPlayer;

BOOL initAllPlayer(void);
void updatePlayerSockid(const char* id, int sockid);


#endif 
