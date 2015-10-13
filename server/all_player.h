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

typedef struct
{
	char ID[33];
	int act_num;
	int* actid;
	char** act_time;
}PlayerSchedule;

BOOL initAllPlayer(void);
void updatePlayerSockid(const char* id, int sockid);
void closePlayerSocketBySockid(int sockid);
void updatePlayerSchedule(const char* id);
void updateAllPlayer(int signal);
PlayerSchedule* getPlayerScheduleByid(const char* id);
void getCurrentActionAndElapse(PlayerSchedule* ps, int* act, unsigned int* elapse);
void createNewDayScheduleForAllPlayer(void);

#endif 
