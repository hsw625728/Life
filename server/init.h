#ifndef __INIT_H__
#define __INIT_H__

#include "type.h"
#include "system.h"

typedef BOOL (*INITFUNC)(void);
typedef BOOL (*CLOSEFUNC)(void);

extern INITFUNC InitBaseSystemFunc[];
extern INITFUNC InitGameSystemFunc[];
extern INITFUNC InitGameDataFunc[];
extern CLOSEFUNC CloseGameCloseFunc[];
extern CLOSEFUNC CloseSystemCloseFunc[];

BOOL CallInitFunc(INITFUNC * functable);
BOOL CallCloseFunc(CLOSEFUNC *functable);

#define CONFIG_FILENAME "gmsv.cf"

typedef struct
{
	int clusterID;
	int serverID;
	int maxConn;
	int portNum;
	int adminiPort;
	int adminiMaxNum;
	char adminiPass[32];
	int serverTime;
	int logLevel;
	int userNum;
	int charNum;
	int userReleaseInterval;
	int cronReleaseInterval;
	int userReleaseMaxACron;
	int charSaveMaxACron;
	int userDisconnectACron;
	int cronPingInterval;
	int playerNum;
	int gmsvNo;
	
	char dbsvIP[32];
	int dbsvPort;
	char userDumpFile[32];
	char charDumpFile[32];
	char gmFilePath[32];
	char mysqlHostName[32];
	char mysqlUserName[32];
	char mysqlPassword[32];
	char mysqlDatabase[32];
}sSETUP_DATA;

extern sSETUP_DATA ConfigData;

typedef struct
{
	char *target;
	int type;
	void *val;
	int size;
}sCONFIG_LINE;
BOOL readConfigFile(void);
#endif
