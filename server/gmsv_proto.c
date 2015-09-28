#include "system.h"
#include "./account/account.h"

int gmsvproto_ClientLogin_recv(int _c, int acnt, char *info, int size)
{
	sACCOUNT *account;
	account = searchAccount(_c);
	if (account == NULL)
	{
		account = setAccountConnInfo(_c);
		if (account == NULL)
		{
			return -1;
		}
	}
	else
	{
		sendClientLoginError(account);
		return -1;
	}
	if(!clientLoginRecv(account, (sSYSTEM_INFO_CLIENT_ACCOUNT*)info))
	{
		return -1;
	}
	return 0;

}
