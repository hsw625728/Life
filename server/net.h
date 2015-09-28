#ifndef __NET_H__
#define __NET_H__

#include "type.h"

typedef struct
{
	int tcp;
	unsigned short bindPort;
	unsigned short connectPort;
	int maxconn;
	int rblen;
	int wblen;
	int timeout;
	int shkeylen;
	int maxcirc;
	int nonblock;
	int nodelay;
	int rbsize;
}NetStruct;

enum
{
	NT_CLIENT,
	NT_DBSV,
	NT_NUM,
};

typedef struct
{
	int port;
	char host[32];
	int cli;
}sNetConnList;

typedef struct
{
	int id;
	char typeName[32];
	char protoName[32];
}sPROTOLIST;

BOOL initNet(void);

void netThread(void);

BOOL initClient(void);
BOOL initClientConnectList(void);

BOOL initDBWork(void);
BOOL initDBConnectList(void);

BOOL waitConnection(int c, int loopCount);

int getProtocolIndex(int protoNo, sPROTOLIST *list, int listSize);
BOOL netHeartBeat(void);
int* getDBConnInfo(int dbsvID);

#endif

