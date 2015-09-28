#ifndef __DBSV_PROTO_H__
#define __DBSV_PROTO_H__

#include "proto_info.h"

typedef struct
{
	int status;
	int loadWeight;
	int playerNum;
	int playerMax;
}sDBInfoGmsvStatus;
typedef struct
{
	int clid;
	char cdKey[IDKEY_SIZE];
	char pass[IDKEY_SIZE];
	char memberID[AUTH_MEMBERID_SIZE];
	char authString[AUTH_STRING_SIZE];
	unsigned char physicalAddress[8];
}sDB_INFO_CLIENT_LOGIN;
enum
{
	ePROTO_DBSV_CS_PING = 1,
	ePROTO_DBSV_CS_GMSVSTATUS = 18,
	ePROTO_DBSV_CS_CLIENTLOGIN = 64,
};
//多DB版本
//BOOL dbsvSendProtocol(sACCOUNT* clacnt, int protoNo, int *arg);


#endif
