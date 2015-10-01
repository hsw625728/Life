#include "system.h"


static double gmsvproto_ClientLogin_recv_counter = 0;

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

#define MEM_SIZE 1024
extern struct event_base* base;
void release_sock_event(struct sock_ev* ev)
{
	event_del(ev->read_ev);
	free(ev->read_ev);
	free(ev->write_ev);
	free(ev->buffer);
	free(ev);
}

void on_write(int sock, short event, void* arg)
{
	int protocolID;
	struct sock_ev* ev = (struct sock_ev*)arg;
	memcpy(&protocolID, ev->buffer, sizeof(int));
	printf("protocolID = %d\n", protocolID);
	switch (protocolID)
	{
		case MSG_CS_LOGIN:
			processLogin(ev);
		break;
	}
	readUsrAndPwd();
}

void gmsvproto_sv_callback(int sock, short event, void* arg)
{
	struct event* write_ev;
	int size;
	struct sock_ev* ev = (struct sock_ev*)arg;
	ev->buffer = (char*)malloc(MEM_SIZE);
	bzero(ev->buffer, MEM_SIZE);
	size = recv(sock, ev->buffer, MEM_SIZE, 0);
	printf("sock = %d, receive data:%s, size:%d\n",sock, ev->buffer, size);
	if (size == 0) {
		release_sock_event(ev);
		close(sock);
		printf("sock = %d, closed!\n", sock);
		return;
	}


//	readUsrAndPwd();
	event_set(ev->write_ev, sock, EV_WRITE, on_write, ev);
	event_base_set(base, ev->write_ev);
	event_add(ev->write_ev, NULL);
}

void processLogin(struct sock_ev* ev)
{
	char uniqueID[64];
	memset(uniqueID, 0, 64);
	memcpy(uniqueID, ev->buffer + 4, 32);
	uniqueID[32] = '\0';
	printf("buff=");
	printf(uniqueID);
	int ret = send(ev->sockid, uniqueID, strlen(uniqueID), 0);
	if (ret < 0)
	{
		printf("send err!n");
	}
}
