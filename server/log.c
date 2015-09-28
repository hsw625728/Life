#include "system.h"

static char LogFileName[LT_FILE_NUM][64];
static LogSetUp gLogSetUp[] ={
	{"ALL", "serverlog", FALSE, FALSE},
	{"SYSTEM", "system", FALSE, TRUE},
	{"ERROR", "error", FALSE, TRUE}
};

BOOL InitLogSystem(void)
{
	int i;
	for (i = 0; i < LT_FILE_NUM; i++)
	{
		FILE *f;
		strcpy(LogFileName[i], LOG_FILE_PATH);
		strcat(LogFileName[i], gLogSetUp[i].filename);
		strcat(LogFileName[i], ".log");
		if (gLogSetUp[i].append)
		{
			f = fopen(LogFileName[i], "a");
		}
		else
		{
			f = fopen(LogFileName[i], "w");
		}
		if (NULL == f)
		{
			printf("LOG:WARNING:Failed to initialize log(%s).\n", LogFileName[i]);
			return FALSE;
		}
		fclose(f);
	}
	return TRUE;
}

BOOL LogWrite(int type, char* msg)
{
	char errMsg[128];
	if (type < 0 || type >= LT_FILE_NUM)
	{
		sprintf(errMsg, "LOG:ERROR: Wrong log type(%d).", (int)type);
		LogWrite(LT_ERROR, errMsg);
		return FALSE;
	}

	if (!msg)
	{
		sprintf(errMsg, "LOG:ERROR: Message is null.");
		LogWrite(LT_ERROR, errMsg);
		return FALSE;
	}

	FILE *f = fopen(LogFileName[type], "a");
	if (f==NULL)
	{
		return FALSE;
	}
	fputs(msg, f);
	fputs("\n", f);
	fclose(f);
	return TRUE;
}

