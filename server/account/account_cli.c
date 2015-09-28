#include "../system.h"
#include "account.h"

BOOL clientLoginRecv(sACCOUNT *clacnt, sSYSTEM_INFO_CLIENT_ACCOUNT *account)
{
	char string[256];
	sDB_INFO_CLIENT_LOGIN dbinfo;
	sprintf(string, "[[ACCOUTN]] clientLoginRecv() cdkey=%s\n", account->IDKey);
	LogWrite(LT_SYSTEM, string);
	if (getAccountInfo(account->IDKey)!=NULL)
	{
		sendClientLoginError(clacnt);
		return FALSE;
	}
	memset(&dbinfo, 0, sizeof(dbinfo));
	memcpy(dbinfo.cdKey, account->IDKey, sizeof(dbinfo.cdKey));
	memcpy(dbinfo.pass, account->IDKey, sizeof(dbinfo.pass));
	memcpy(dbinfo.memberID, account->memberID, sizeof(dbinfo.memberID));
	memcpy(dbinfo.authString, account->authString, sizeof(dbinfo.authString));
	memcpy(clacnt->idkey, account->IDKey, sizeof(clacnt->idkey));
	setAccountStatus(clacnt, eACCOUNT_STATUS_CLIENTLOGIN_WAIT);
	long arg[2];
	arg[0] = (long)&dbinfo;
	arg[1] = sizeof(dbinfo);
	dbsvSendProtocol(NULL, ePROTO_DBSV_CS_CLIENTLOGIN, arg);
	return TRUE; 
}
