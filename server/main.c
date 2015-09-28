#include "system.h"

static int GameLoopFlag;

BOOL MainGameLoop()
{
	while(1)
	{
		netHeartBeat();
	}
	return TRUE;
}

BOOL main(void)
{
	//initialize log system
	LogWrite(LT_SYSTEM, "System Initialize.");
	//InitServerStatus();

	//Initialize base system.
	LogWrite(LT_SYSTEM, "MAIN:Initializing base system...");
	if (!CallInitFunc(&InitBaseSystemFunc[0]))
	{
		LogWrite(LT_SYSTEM, "MAIN:WARNING:Failed initialize base sytem.");
		goto ERROR;
	}
	LogWrite(LT_SYSTEM, "MAIN:Initialized base system successfully.");

	GameLoopFlag = TRUE;
	while (GameLoopFlag)
	{
		LogWrite(LT_SYSTEM, "MAIN:Initializing game system...");
		//if (!CallInitFunc(&InitGameSystemFunc[0]))
		//{
		//	LogWrite(LT_SYSTEM, "MAIN:WARNING:Failed to initialize game sytem.");
		//	goto ERROR;
		//}
		LogWrite(LT_SYSTEM, "MAIN:Initialized game system successfully.");

		LogWrite(LT_SYSTEM, "MAIN: Reading resources...");
		if (!ReadResourceSystem(0, 0))
		{
			LogWrite(LT_SYSTEM, "MAIN: Failed to read resources.");
			goto ERROR;
		}
		LogWrite(LT_SYSTEM, "MAIN: Read resources successfully.");

		LogWrite(LT_SYSTEM, "MAIN: Initializing game data...");
		//if (!CallInitFunc(&InitGameDataFunc[0]))
		//{
		//	LogWrite(LT_SYSTEM, "MAIN: Failed to initialize game data.");
		//	goto ERROR;
		//}
		LogWrite(LT_SYSTEM, "MAIN: Initialized game data successfully.");

		MainGameLoop();

		LogWrite(LT_SYSTEM, "MAIN: Closeing game system...");
		//if (CallCloseFunc(CloseGameCloseFunc))
		//	LogWrite(LT_SYSTEM, "MAIN: Closed game system successfully.");
		//else
		//	LogWrite(LT_SYSTEM, "MAIN: WARNING: Closed game system in trouble.");
			
	}
		LogWrite(LT_SYSTEM, "MAIN: Closing base system...");
		//if (CallCloseFunc(CloseSystemCloseFunc))
		//	LogWrite(LT_SYSTEM, "MAIN: Closed base system successfully.");
		//else
		//	LogWrite(LT_SYSTEM, "MAIN: WARNING: Closed base system in trouble.");

		//LogWrite(LT_SYSTEM, "MAIN: Finalizing memory...");
		//if (FreeAllMemory(TRUE))
		//	LogWrite(LT_SYSTEM, "MAIN: Finalized memory successfully.");
		//else
		//	LogWrite(LT_SYSTEM, "MAIN: WARNING: Finalized memory in trouble.");

		return 0;

ERROR:
		LogWrite(LT_SYSTEM, "MIAN:ERROR: Goto Error!");


}

