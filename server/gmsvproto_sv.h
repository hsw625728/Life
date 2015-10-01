#ifndef __GMSVPROTP_SV_H__
#define __GMSVPROTO_SV_H__

enum
{
	GMSVPROTO_FUNCID_CLIENTLOGIN = 16,
}GMSVPROTO_FUNCID;
void gmsvproto_sv_callback(int sock, short event, void* arg);


#endif
