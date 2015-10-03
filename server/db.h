#ifndef __DB_H__
#define __DB_H__

#include "type.h"

#define LENGTH_OF_QUERY 64*1024

BOOL initDB(void);
BOOL sendQuery(char* fmt, ...);
BOOL closeDB(void); 


int readUsrAndPwd(void);
int readAllPlayer(void);




#endif
