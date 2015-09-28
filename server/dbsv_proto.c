#include "system.h"

static BOOL sendProtocol(int dbsvID, int protoNo, long *arg)
{
	int *c = NULL;
	c = getDBConnInfo(dbsvID);
	if (c == NULL)
	{
		char string[256];
		sprintf(string, "[[NET]] GMSV can't find a connect to DBSV.\n");
		LogWrite(LT_SYSTEM, string);
		return FALSE;
	}
	switch (protoNo)
	{
		case ePROTO_DBSV_CS_GMSVSTATUS:
			dbsvproto_GMSVStatus_send(*c, (int)arg[0], (char*)arg[1], (int)arg[2]);
			break;
		case ePROTO_DBSV_CS_CLIENTLOGIN:
			dbsvproto_ClientLogin_send(*c, (char*)arg[0], arg[1]);
			break;
		defautl:
			break;
	}
	return TRUE;
}

int dbsvproto_cli_sender(int c, char *data, int len)
{
	int proto;
	int acnt = 0;
	int index;
	int writableSize = 0;
	char string[256];
	//proto = GET_16BIT_INT(data);
//	vce_binary_pop_nb_i4(data, len, 2, &acnt);
	//我的理解这个index的作用是打印日志用的
	index = getProtocolIndex(proto, dbsvProtoList, utilArraySizeof(dbsvProtoList));
	//打开这个宏就可以打印消息日志
#if 0
	sprintf(string, "%s %s", dbsvProtoList[index].typeName, dbsvProtoList[index].protoName);
	LogWrite(LT_SYSTEM, string);
#endif
	//writableSize = vce_conn_writable(c);
	if (writableSize < len + 2)
	{
		char msg[256];
		sprintf(msg, "[[NET]]Send buffer full.writable size = %d.\n", writableSize);
		LogWrite(LT_SYSTEM, msg);
		//vce_conn_break_heartbeat();
	}
	printf("%s\n", data);
	return 0;//vce_protocol_unparser_bin16(c, data, len);
}

BOOL dbsvSendProtocol(int* clacnt, int protoNo, long *arg)
{
	return sendProtocol(0, protoNo, arg);
}



int dbsvproto_ClientLoginRes_recv(int _c, char *info, int size)
{
	if (!ACCOUNT_clientLoginResRecv(info, size))
	{
		return -1;
	}
	return 0;
}
