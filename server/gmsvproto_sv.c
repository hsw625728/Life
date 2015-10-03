#include "system.h"

#define MEM_SIZE 1024

extern struct event_base* base;
extern AllPlayer* allPlayer;

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
	char usr[64];
	memset(usr, 0, 64);
	memcpy(usr, ev->buffer + 4, 32);
	printf("buff=");
	printf(usr);
	updatePlayerSockid(usr, ev->sockid);
	int ret = send(ev->sockid, usr, strlen(usr), 0);
	if (ret < 0)
	{
		printf("send err!n");
	}
}
