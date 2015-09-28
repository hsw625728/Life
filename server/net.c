#include "system.h"

NetStruct ConnectList[NT_NUM];
sNetConnList dbConnList;
struct event_base* base;

BOOL netHeartBeat(void)
{
	//vce_heartbeat();
	return TRUE;
}

BOOL initNet(void)
{
	if (!initClient())
	{
		LogWrite(LT_SYSTEM, "NET: Error: Initialize client failde.");
		return FALSE;
	}
	if (!initDBWork())
	{
		LogWrite(LT_SYSTEM, "NET: Error: Initialize DBWork failed.");
		return FALSE;
	}
	return TRUE;
}

static int processProtocol(int sock, short event, void* arg)
{
	struct sockaddr_in cli_addr;
	int newfd, sin_size;
	struct sock_ev* ev = (struct sock_ev*)malloc(sizeof(struct sock_ev));
	ev->read_ev = (struct event*)malloc(sizeof(struct event));
	ev->write_ev = (struct event*)malloc(sizeof(struct event));
	sin_size = sizeof(struct sockaddr_in);
	newfd = accept(sock, (struct sockaddr*)&cli_addr, &sin_size);
	event_set(ev->read_ev, newfd, EV_READ | EV_PERSIST, gmsvproto_sv_callback, ev);
	event_base_set(base, ev->read_ev);
	event_add(ev->read_ev, NULL);
	return 1;
}

static int processDBProtocol(int _c, char *_data, int _len)
{
	int protocolID;
	if (dbsvproto_cli_pcallback(_c, _data, _len) < 0)
	{
		char string[256];
		//protocolID = GET_16BIT_INT(_data);
		sprintf(string, "[[PROTOCOL]]dbsvproto_cli_callback called failed.protocolID = %d\n", protocolID);
		return -1;
	}
	return 1;
}

BOOL initClient(void)
{
	if (!initClientConnectList())
	{
		return FALSE;
	}
	pthread_t id;
	int ret = 0;
	ret = pthread_create(&id, NULL, (void*)netThread, NULL);
	if (ret != 0)
	{
		printf("[ERROR]Create net thread fialed!\n");
		return 0;
	}
	return TRUE;
}

void netThread(void)
{
	printf("thread begin!\n");
	struct sockaddr_in my_addr;

	ConnectList[NT_CLIENT].tcp = socket(AF_INET, SOCK_STREAM, 0);
	int yes = 1;
	setsockopt(ConnectList[NT_CLIENT].tcp, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(ConfigData.portNum);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	bind(ConnectList[NT_CLIENT].tcp, (struct sockaddr*)&my_addr, sizeof(struct sockaddr));
	listen(ConnectList[NT_CLIENT].tcp, ConfigData.maxConn);

	if (!ConnectList[NT_CLIENT].tcp)
	{
		LogWrite(LT_SYSTEM, "NET: Error: Create socket failed.");
		return;
	}
	struct event listen_ev;
	base = event_base_new();
	event_set(&listen_ev, ConnectList[NT_CLIENT].tcp, EV_READ | EV_PERSIST, processProtocol, NULL);
	event_base_set(base, &listen_ev);
	event_add(&listen_ev, NULL);
	event_base_dispatch(base);
}

BOOL initClientConnectList(void)
{
	ConnectList[NT_CLIENT].bindPort = ConfigData.portNum;	
	ConnectList[NT_CLIENT].maxconn = 8;
	ConnectList[NT_CLIENT].rblen = 1024*1024;
	ConnectList[NT_CLIENT].wblen =1024*1024; 
	ConnectList[NT_CLIENT].timeout = 10*365*24*60*60;
	ConnectList[NT_CLIENT].shkeylen = 8;
	ConnectList[NT_CLIENT].nonblock = 0;
	ConnectList[NT_CLIENT].nodelay = 0;
	ConnectList[NT_CLIENT].rbsize =1000;
	return TRUE;
}


BOOL waitConnection(int c, int loopCount)
{
	int i;
	int res;
	for (i = 0; i < loopCount; i++)
	{
		//res = vce_conn_writable(c);
		if (res < 0)
		{
			return FALSE;
		}
		else if(res > 0)
		{
			break;
		}
		//vce_heartbeat();
		sleep(1);
	}
	if (i >= loopCount)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL initDBWork(void)
{
	return TRUE;
	char string[256];
	sDBInfoGmsvStatus info;
	memset(&dbConnList, 0, sizeof(sNetConnList));
	if (!initDBConnectList())
	{
		sprintf(string,"[[NET]]Create dbsv connectinfo failed!\n");
		LogWrite(LT_SYSTEM, string);
		return FALSE;
	}

	ConnectList[NT_DBSV].tcp = vce_tcpcontext_create(
		0,
		NULL,
		0,
		ConnectList[NT_DBSV].maxconn,
		ConnectList[NT_DBSV].rblen,
		ConnectList[NT_DBSV].wblen,
		ConnectList[NT_DBSV].timeout,
		ConnectList[NT_DBSV].shkeylen,
		ConnectList[NT_DBSV].nonblock,
		ConnectList[NT_DBSV].nodelay,
		ConnectList[NT_DBSV].rbsize);

	if (!ConnectList[NT_DBSV].tcp)
	{
		sprintf(string, "[[NET]]Create dbsv socket failed!\n");
		LogWrite(LT_SYSTEM, string);
		return FALSE;
	}

//	vce_tcpcontext_set_conn_parser(ConnectList[NT_DBSV].tcp, vce_protocol_parser_bin16,processDBProtocol);
//	vce_tcpcontext_set_conn_call_parser_per_heartbeat(ConnectList[NT_DBSV].tcp, 10);

	printf("dbsvIP=%s,port=%d\n",ConfigData.dbsvIP, ConnectList[NT_DBSV].connectPort);
//	dbConnList.cli = vce_tcpcontext_connect(ConnectList[NT_DBSV].tcp, ConfigData.dbsvIP, ConnectList[NT_DBSV].connectPort);
	if (!waitConnection(dbConnList.cli, 1000))
	{
		sprintf(string, "[[NET]] It failed in the connection with DBSV!\n");
		LogWrite(LT_SYSTEM, string);
		return FALSE;
	}

	sprintf(string, "[[NET]]Connected to DBSV(%s).\n", ConfigData.dbsvIP);
	LogWrite(LT_SYSTEM, string);

	info.status = eSERVER_STATUS_RUNNING;
	info.playerMax = ConfigData.playerNum;
	info.playerNum = 0;
	info.loadWeight = eSERVER_LOADAVERAGE_BLUE;
	long sendStatus[4];
	sendStatus[0] = ConfigData.gmsvNo;
	sendStatus[1] = (long)&info;
	sendStatus[2] = sizeof(info);

	dbsvSendProtocol(NULL, ePROTO_DBSV_CS_GMSVSTATUS,sendStatus);

	return TRUE;
}


BOOL initDBConnectList(void)
{
	ConnectList[NT_DBSV].bindPort = ConfigData.portNum;	
	ConnectList[NT_DBSV].connectPort = ConfigData.dbsvPort;
	ConnectList[NT_DBSV].maxconn = 8;
	ConnectList[NT_DBSV].rblen = 1024*1024;
	ConnectList[NT_DBSV].wblen =1024*1024; 
	ConnectList[NT_DBSV].timeout = 10*365*24*60*60;
	ConnectList[NT_DBSV].shkeylen = 8;
	ConnectList[NT_DBSV].nonblock = 0;
	ConnectList[NT_DBSV].nodelay = 0;
	ConnectList[NT_DBSV].rbsize =1000;
	return TRUE;
}

int getProtocolIndex(int protoNo, sPROTOLIST *list, int listSize)
{
	int i;
	int rtn = 0;
	if (list == NULL)
	{
		return 0;
	}
	for (i = 0; i < listSize; i++)
	{
		if (list[i].id == protoNo)
		{
			rtn = i;
			break;
		}
	}
	return rtn;
}

int* getDBConnInfo(int dbsvID)
{
	return &(dbConnList.cli);
}
