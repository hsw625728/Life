#include "system.h"

AllPlayer* allPlayer = 0;
int allPlayerNum = 0;

BOOL initAllPlayer(void)
{
	readAllPlayer();
}

void updatePlayerSockid(const char* id, int sockid)
{
	int i = 0;
	for (; i < allPlayerNum; i++)
	{
		if (!strcmp(allPlayer[i].usr, id))
		{
			allPlayer[i].sockid = sockid;
			printf("UpdatePlayerSockid(%s)=%d\n", id, sockid);
			break;
		}
	}
}
