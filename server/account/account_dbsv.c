#include "../system.h"


BOOL ACCOUNT_clientLoginResRecv(char *info, int size)
{
	char string[256];
	sprintf(string, "[[Protocol]]Account_ClientLoginResRecv success!\n");
	printf(string);
	LogWrite(LT_SYSTEM, string);
	return TRUE;
}
