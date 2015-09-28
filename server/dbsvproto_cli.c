#include "system.h"

static double dbsvproto_GMSVStatus_send_counter = 0;
static double dbsvproto_ClientLogin_send_counter = 0;

static double dbsvproto_ClientLoginRes_recv_counter = 0;

#define _SHORTCHECK if(_ofs<0){if(_ofs!=VCE_ETOOSHORT){return 1;}else {return 0;}}
#define _POP_I8(i) _ofs = vce_binary_pop_nb_i8(_data, _len, _ofs, (VCEI64*)&i); _SHORTCHECK;
#define _POP_I4(i) _ofs = vce_binary_pop_nb_i4(_data, _len, _ofs, (int*)&i); _SHORTCHECK;
#define _POP_I2(i) _ofs = vce_binary_pop_nb_i2(_data, _len, _ofs, (short*)&i); _SHORTCHECK;
#define _POP_I1(i) _ofs = vce_binary_pop_nb_i1(_data, _len, _ofs, (char*)&i); _SHORTCHECK;
#define _POP_IA8(ia,ialen) _ofs = vce_binary_pop_nb_ia8(_data, _len, _ofs, (VCEI64*)ia, &ialen); _SHORTCHECK;
#define _POP_IA4(ia,ialen) _ofs = vce_binary_pop_nb_ia4(_data, _len, _ofs, (int*)ia, &ialen); _SHORTCHECK;
#define _POP_IA2(ia,ialen) _ofs = vce_binary_pop_nb_ia2(_data, _len, _ofs, (short*)ia, &ialen); _SHORTCHECK;
#define _POP_IA1(ia,ialen) _ofs = vce_binary_pop_nb_ia1(_data, _len, _ofs, (char*)ia, &ialen); _SHORTCHECK;
#define _POP_STRA(sa,salen,eachmax,i,p) for(i=0;i<salen;i++){p[i]=sa[i];}_ofs=vce_binary_pop_nb_stra(_data,_len,_ofs,p,&salen,eachmax); _SHORTCHECK;






#define _OFSCHECK if(_ofs<0) return _ofs;
#define _LENCHECK(l,max) if((l) > (max)) return VCE_EFULL;
#define _PUSH_STRA(a,alen,maxalen,outmax,eachmax) _LENCHECK(alen,maxalen); _ofs=vce_binary_push_nb_stra(_work, _ofs, outmax, (const char* const*)a,alen,eachmax)
#define _PUSH_IA1(a,alen,maxalen,outmax) _LENCHECK(alen,maxalen); _ofs=vce_binary_push_nb_ia1(_work,_ofs,outmax,(char*)a,alen);_OFSCHECK;
#define _PUSH_IA2(a,alen,maxalen,outmax) _LENCHECK(alen,maxnlen); _ofs=vce_binary_push_nb_ia2(_work,_ofs,outmax,(short*)a,alen);_OFSCHECK;
#define _PUSH_IA4(a,alen,maxalen,outmax) _LENCHECK(alen,maxnlen); _ofs=vce_binary_push_nb_ia4(_work,_ofs,outmax,(int*)a,alen);_OFSCHECK;
#define _PUSH_IA8(a,alen,maxalen,outmax) _LENCHECK(alen,maxnlen); _ofs=vce_binary_push_nb_ia8(_work,_ofs,outmax,(VCEI64*)a,alen);_OFSCHECK;
#define _PUSH_I1(i,outmax) _ofs=vce_binary_push_nb_i1(_work,_ofs,outmax,(char)i);_OFSCHECK;
#define _PUSH_I2(i,outmax) _ofs=vce_binary_push_nb_i2(_work,_ofs,outmax,(short)i);_OFSCHECK;
#define _PUSH_I4(i,outmax) _ofs=vce_binary_push_nb_i4(_work,_ofs,outmax,(int)i);_OFSCHECK;
#define _PUSH_I8(i,outmax) _ofs=vce_binary_push_nb_i8(_work,_ofs,outmax,(VCEI64)i);_OFSCHECK;

static int dbsvproto_cli_current_conn;
int dbsvproto_cli_pcallback(int _c, char *_data, int _len)
{
	int _ofs = 2;
	int _ret;
	unsigned short command;
	if (_len < 2) return 0;
	dbsvproto_cli_current_conn = _c;
	//command = GET_16BIT_INT(_data);
	switch (command)
	{
		case DBSVPROTO_FUNCID_CLIENTLOGINRES:
			{
				char info[256];
				int info_len = 256;
				//_POP_IA1(info, info_len);
				dbsvproto_ClientLoginRes_recv_counter += 1;
				_ret = dbsvproto_ClientLoginRes_recv(_c, info, info_len);
			}
			break;
	}
}

int dbsvproto_GMSVStatus_send(int _c, int serverNo, char *info, int info_len)
{
	char _work[267];
	int _ofs = 0;
	dbsvproto_GMSVStatus_send_counter += 1;
	//_PUSH_I2(DBSVPROTO_FUNCID_GMSVSTATUS, sizeof(_work));
	//_PUSH_I4(serverNo, sizeof(_work));
	//_PUSH_IA1(info, info_len, 256, sizeof(_work));
	return dbsvproto_cli_sender(_c, _work, _ofs);
}

int dbsvproto_ClientLogin_send(int _c, char *info, int info_len)
{
	char _work[8199];
	int _ofs = 0;
	dbsvproto_ClientLogin_send_counter += 1;
	//_PUSH_I2(DBSVPROTO_FUNCID_CLIENTLOGIN, sizeof(_work));
	//_PUSH_IA1(info, info_len, 8192, sizeof(_work));
	return dbsvproto_cli_sender(_c, _work, _ofs);
}
