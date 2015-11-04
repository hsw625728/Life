#ifndef __GMSVPROTP_SV_H__
#define __GMSVPROTO_SV_H__

#include "net_packet.h"

enum
{
	GMSVPROTO_FUNCID_CLIENTLOGIN = 16,
}GMSVPROTO_FUNCID;
void gmsvproto_sv_callback(int sock, short event, void* arg);
void processLogin(struct sock_ev* ev);
void processGetSchedule(struct sock_ev* ev);
void processDominoUploadTilemap(struct sock_ev* ev);
void processDominoDownloadTilemap(struct sock_ev* ev);


#endif
