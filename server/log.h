#ifndef __LOG_H__
#define __LOG_H__

#include "type.h"
#include "system.h"

#define LOG_FILE_PATH "./log/"

enum LOG_TYPE
{
	LT_GENERAL,
	LT_SYSTEM,
	LT_ERROR,
	LT_FILE_NUM
};

typedef struct
{
	char logname[32];
	char filename[32];
	BOOL append;
	BOOL general;
}LogSetUp;

BOOL InitLogSystem(void);
BOOL LogWrite(int type, char* msg);

#endif

