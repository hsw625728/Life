#include <sys/time.h>
#include <signal.h>
#include "system.h"

BOOL initGameTimer(void)
{
	struct itimerval new_value,old_value;
	new_value.it_value.tv_sec = 5;
	new_value.it_value.tv_usec = 0;
	new_value.it_interval.tv_sec = 5;
	new_value.it_interval.tv_usec = 0;

	signal(SIGALRM, updateAllPlayer);
	setitimer(ITIMER_REAL, &new_value, &old_value);
	printf("initGameTimer\n");
	return TRUE;
}
