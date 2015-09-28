#ifndef __ACCOUNT_H__
#define __ACCOUNT_H__

#include "../type.h"
#include "../gmsv_proto.h"

typedef struct{
	char idkey[IDKEY_SIZE];
}sACCOUNT;

sACCOUNT *searchAccount(int c);
sACCOUNT *setAccountConnInfo(int c);
sACCOUNT *getAccountInfo(char *idkey);
BOOL setAccountStatus(sACCOUNT *clacnt, int status);
BOOL sendClientLoginError(sACCOUNT *clacnt);
BOOL clientLoginRecv(sACCOUNT *clacnt, sSYSTEM_INFO_CLIENT_ACCOUNT *account);

BOOL ACCOUNT_clientLoginResRecv(char *info, int size);

#endif
