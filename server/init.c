#include "system.h"

sSETUP_DATA ConfigData;
sCONFIG_LINE SetUpConf[]={
	{ "clusterID", eCTYPE_INT, &ConfigData.clusterID ,0},
	{ "serverID", eCTYPE_INT, &ConfigData.serverID ,0},
	{ "maxConn", eCTYPE_INT, &ConfigData.maxConn ,0},
	{ "portNum", eCTYPE_INT, &ConfigData.portNum ,0},
	{ "adminiPort", eCTYPE_INT, &ConfigData.adminiPort ,0},
	{ "adminiMaxNum", eCTYPE_INT, &ConfigData.adminiMaxNum ,0},
	{ "adminiPass", eCTYPE_STRING, &ConfigData.adminiPass ,32},
	{ "serverTime", eCTYPE_INT, &ConfigData.serverTime ,0},
	{ "logLevel", eCTYPE_INT, &ConfigData.logLevel ,0},
	{ "userNum", eCTYPE_INT, &ConfigData.userNum ,0},
	{ "charNum", eCTYPE_INT, &ConfigData.charNum ,0},
	{ "userReleaseInterval", eCTYPE_INT, &ConfigData.userReleaseInterval ,0},
	{ "cronReleaseInterval", eCTYPE_INT, &ConfigData.cronReleaseInterval ,0},
	{ "userReleaseMaxACron", eCTYPE_INT, &ConfigData.userReleaseMaxACron ,0},
	{ "charSaveMaxACron", eCTYPE_INT, &ConfigData.charSaveMaxACron ,0},
	{ "userDisconnectACron", eCTYPE_INT, &ConfigData.userDisconnectACron ,0},
	{ "cronPingInterval", eCTYPE_INT, &ConfigData.cronPingInterval ,0},
	{ "playerNum", eCTYPE_INT, &ConfigData.playerNum, 0},
	{ "gmsvNo", eCTYPE_INT, &ConfigData.gmsvNo, 0},
	
	{ "dbsvIP", eCTYPE_STRING, &ConfigData.dbsvIP, 32},
	{ "dbsvPort", eCTYPE_INT, &ConfigData.dbsvPort, 0},
	{ "userDumpFile", eCTYPE_STRING, &ConfigData.userDumpFile ,32},
	{ "charDumpFile", eCTYPE_STRING, &ConfigData.charDumpFile ,32},
	{ "gmFilePath", eCTYPE_STRING, &ConfigData.gmFilePath ,32},
    { "mysqlHostName", eCTYPE_STRING, &ConfigData.mysqlHostName ,32},
    { "mysqlUserName", eCTYPE_STRING, &ConfigData.mysqlUserName ,32},
    { "mysqlPassword", eCTYPE_STRING, &ConfigData.mysqlPassword ,32},
    { "mysqlDatabase", eCTYPE_STRING, &ConfigData.mysqlDatabase ,32},
    { "userDumpFile", eCTYPE_STRING, &ConfigData.userDumpFile ,32},
    { "charDumpFile", eCTYPE_STRING, &ConfigData.charDumpFile ,32},
    { "gmFilePath", eCTYPE_STRING, &ConfigData.gmFilePath ,32},

};
	
INITFUNC InitBaseSystemFunc[] = {
	InitLogSystem,
	readConfigFile,
	initNet,
	initDB,
	NULL
};
INITFUNC InitGameSystemFunc[] = {
	NULL
};
INITFUNC InitGameDataFunc[] = {
	NULL
};
CLOSEFUNC CloseGameCloseFunc[] = {
	NULL
};
CLOSEFUNC CloseSystemCloseFunc[] = {
	NULL
};
BOOL CallInitFunc(INITFUNC * functable)
{
	INITFUNC *f;
	char msg[128];

	LogWrite(LT_SYSTEM, "INIT: OK: step 0/0");

	for ( f = functable; *f; ++f)
		;
	int totalStep = f - functable;
	int curStep = 1;

	for (f = functable; *f; ++f, ++curStep)
	{
		if (!(*f)())
		{
			sprintf(msg, "[[INIT] A called function is failed. Step %d/%d", curStep, totalStep);
			LogWrite(LT_ERROR, msg);

			sprintf(msg, "INIT: NG: Step %d/%d", curStep, totalStep);
			LogWrite(LT_SYSTEM, msg);

			sprintf(msg, "INIT: NG: Error 1/%d", totalStep);
			LogWrite(LT_SYSTEM, msg);

			return FALSE;
		}

		sprintf(msg, "INIT: OK: Step %d/%d", curStep, totalStep);
		LogWrite(LT_SYSTEM, msg);
	}

	sprintf(msg, "INIT: OK: Error 0/%d", totalStep);
	LogWrite(LT_SYSTEM, msg);
	return TRUE;
}

BOOL CallCloseFunc(CLOSEFUNC *functable)
{
	CLOSEFUNC *f;
	char msg[128];

	LogWrite(LT_SYSTEM, "CLOSE: OK: Step 0/0");
	for (f = functable; *f; ++f)
		;
	int ngCount = 0;
	int totalStep = f - functable;
	int curStep = 1;
	for (f = functable; *f; ++f, ++curStep)
	{
		if (!(*f)())
		{
			sprintf(msg, "CLOSE: OK: Step %d/%d", curStep, totalStep);
		}
		else
		{
			++ngCount;
			sprintf(msg, "CLOSE: NG: Step %d/%d", curStep, totalStep);
		}

		LogWrite(LT_SYSTEM, msg);
	}

	if (ngCount == 0)
	{
		sprintf(msg, "CLOSE: OK: Error 0/%d", totalStep);
	}
	else
	{
		sprintf(msg, "CLOSE: NG: Error %d/%d", ngCount, totalStep);
	}
	LogWrite(LT_SYSTEM, msg);

	return (ngCount == 0);
}

BOOL readConfigFile(void)
{
	int i;
	int linenum=0;
	char filename[256];
	char line[256];
	FILE *fp;

	LogWrite(LT_SYSTEM, "INIT: CONFIG: Reading configuration file.\n");
    ConfigData.mysqlHostName[0] = '\0';
    ConfigData.mysqlUserName[0] = '\0';
    ConfigData.mysqlPassword[0] = '\0';
    ConfigData.mysqlDatabase[0] = '\0';

	ConfigData.dbsvIP[0] = '\0';
	ConfigData.clusterID = 1;
	ConfigData.serverID = 1;
	
	strcpy(filename, CONFIG_FILENAME);	
	fp = fopen(filename, "r");
	if (NULL == fp)
	{
		LogWrite(LT_SYSTEM, "INIT: CONFIG: Failed to open the configuration file.\n");
		return FALSE;
	}
	while (fgets(line, sizeof(line), fp))
	{
		char target[256];
		char value[256];

		linenum++;
		if (line[0] == '#' || line[0] == '\n')
		{
			continue;
		}
		utilRemoveBlankFromString(line);
		if (!utilGetDelimiterString(line, "=", 1, target, sizeof(target)))
		{
			continue;
		}
		if (!utilGetDelimiterString(line, "=", 2, value, sizeof(value)))
		{
			continue;
		}
		for (i = 0; i < utilArraySizeof(SetUpConf); i++)
		{
			BOOL *flag;
			int *data;
			char *string;
			sCONFIG_LINE *conf = &SetUpConf[i];

			conf = &SetUpConf[i];
			if (strcmp(target, conf->target) == 0)
			{
				switch (conf->type)
				{
					case eCTYPE_BOOL:
						flag = (BOOL*)conf->val;
						*flag = FALSE;
						if (strcmp("on", value) == 0 ||
							strcmp("ON", value) == 0)
						{
							*flag = TRUE;
						}

						break;
					case eCTYPE_INT:
						data = (int*)conf->val;
						*data = atoi(value);
						break;
					case eCTYPE_STRING:
						string = (char*)conf->val;
						if (strlen(value) >= conf->size)
						{
							printf("字符过多(line=%d)\n", linenum);
							break;
						}
						strncpy( conf->val, value, conf->size);
						break;
				}
				break;
			}
		}
	}
	if (ConfigData.serverID == 0)
	{
		printf("Bad serverID(%d).\n", ConfigData.serverID);
		return FALSE;
	}
	if (ConfigData.clusterID <= 0)
	{
		printf("Bas clusterID(%d).\n", ConfigData.clusterID);
		return FALSE;
	}
	if (ConfigData.userReleaseMaxACron <= 0)
		ConfigData.userReleaseMaxACron = 5000;

	LogWrite(LT_SYSTEM, "Init: CONFIG: Read configuration file successfully!\n");
	return TRUE;
}
