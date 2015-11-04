#ifndef __DB_H__
#define __DB_H__

#include "type.h"

#define LENGTH_OF_QUERY 64*1024

BOOL initDB(void);
BOOL sendQuery(char* fmt, ...);
BOOL closeDB(void); 


int readUsrAndPwd(void);
int db_readAllPlayer(void);
int db_readPlayerSchedule(const char* id);
int db_updatePlayerHistory(const char* id);
int db_updateDominoTilemap(char* usr, int index, const unsigned char* tilemap);
int db_readDominoTilemap(char* usr, int index, unsigned char* tilemap);




#endif
