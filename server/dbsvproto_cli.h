#ifndef __DBSVPROTO_CLI_H__
#define __DBSVPROTO_CLI_H__

typedef enum _DBSVPROT_FUNCID{
	DBSVPROTO_FUNCID_PING = 1,
	DBSVPROTO_FUNCID_GMSVSTATUS = 18,
	DBSVPROTO_FUNCID_CLIENTLOGIN = 64,
	DBSVPROTO_FUNCID_CLIENTLOGINRES = 65,
}DBSVPROTO_FUNID;

int dbsvproto_cli_pcallback(int _c, char *data, int len);

int dbsvproto_GMSVStatus_send(int _c, int serverNo, char *info, int info_len);


int dbsvproto_ClientLoginRes_recv(int _c, char *info, int info_len);
#endif
