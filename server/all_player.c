#include "system.h"
#include <signal.h>

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
void updateAllPlayer(int signo)
{
	if (signo == SIGALRM)
	{
		int i = 0;
		printf ("Receive SIGALRM\n");
		for (; i < allPlayerNum; i++)
		{
			if (allPlayer[i].sockid != 0)
			{
				//这里给在线的角色发送数据
				char string[256];
				static int j = 0;
				sprintf(string, "Message:%d\n", j++);
				send(allPlayer[i].sockid, string, strlen(string), 0);
			}
		}
		signal(SIGALRM, updateAllPlayer);
	}
}
