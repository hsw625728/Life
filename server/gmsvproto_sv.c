#include "system.h"

#define MEM_SIZE 1024

extern struct event_base* base;
extern AllPlayer* allPlayer;

void processLogin(struct sock_ev* ev);

void release_sock_event(struct sock_ev* ev)
{
	event_del(ev->read_ev);
	free(ev->read_ev);
	free(ev->write_ev);
	NET_PACKET_END(ev->packet);
	ev->packet = 0;
	free(ev);
}

void on_write(int sock, short event, void* arg)
{
	if (arg == 0)
		return;
	struct sock_ev* ev = (struct sock_ev*)arg;
	if (ev->packet == 0)
		return;
	netpackGetOpcode(ev->packet, &(ev->packet->opcode));
	printf("protocolID = %d\n", ev->packet->opcode);
	switch (ev->packet->opcode)
	{
		case MSG_CS_LOGIN:
			processLogin(ev);
		break;
	}
	//释放通信NetPacket以及内部buffer空间
	//这部分空间在如下函数中申请
	//函数gmsvproto_sv.c:gmsvproto_sv_callback()
	NET_PACKET_END(ev->packet);
	ev->packet = 0;
}

void gmsvproto_sv_callback(int sock, short event, void* arg)
{
	struct event* write_ev;
	int size;
	struct sock_ev* ev = (struct sock_ev*)arg;
	//初始化通信NetPacket以及内部buffer
	//这部分内容在如下函数中释放
	//函数gmsvproto_sv.c:on_write()最后部分
	ev->packet = (struct NetPacket*)malloc(sizeof(struct NetPacket));
	if (ev->packet == NULL)
	{
		char  string[256];
		sprintf(string, "[ERROR]gmsvproto_sv.c:gmsvproto_sv_callback() malloc ev->packet failed!\n");
		LogWrite(LT_SYSTEM, string);
		printf(string);
	}
	ev->packet->m_buffer = (unsigned char*)malloc(DEFAULT_SIZE);
	if (ev->packet->m_buffer == NULL)
	{
		char string[256];
		sprintf(string, "[ERROR]gmsvproto_sv.c:gmsvproto_sv_callback() malloc ev->packet->m_buffer failed!\n");
		LogWrite(LT_SYSTEM, string);
		printf(string);
	}
	bzero(ev->packet->m_buffer, DEFAULT_SIZE);
	//接受服务器数据
	size = recv(sock, ev->packet->m_buffer, DEFAULT_SIZE, 0);
	if (size == -1)
	{
		if (errno == 104)
		{
			//暂时忽略104 Connection reset by peer
			//因为目前客户端断开连接会收到这个消息
			//不处理会导致服务器崩溃
			return;
		}
	}
	if (size == 0)
	{
		//客户端断开连接
		//这里需要处理断开连接的处理
		closePlayerSocketBySockid(sock);
	}
	printf("sock = %d, receive data:%s, size:%d\n",sock, ev->packet->m_buffer, size);
	//获取数据包的长度
	memcpy(&(ev->packet->m_writePos), ev->packet->m_buffer + sizeof(uint16_t), sizeof(uint16_t));
	if (size == 0) {
		release_sock_event(ev);
		close(sock);
		printf("sock = %d, closed!\n", sock);
		return;
	}


	event_set(ev->write_ev, sock, EV_WRITE, on_write, ev);
	event_base_set(base, ev->write_ev);
	event_add(ev->write_ev, NULL);
}

void processLogin(struct sock_ev* ev)
{
	char usr[64];
	memset(usr, 0, 64);
	NET_PACKET_POP_STRING(ev->packet, usr, 64);
	printf("buff=");
	printf(usr);
	updatePlayerSockid(usr, ev->sockid);
	updatePlayerSchedule(usr);
	//int ret = send(ev->sockid, usr, strlen(usr), 0);
	NET_PACKET_BEGIN(packet, DEFAULT_SIZE);
	NET_PACKET_PUSH_UINT16(packet, MSG_SC_LOGIN_RES);
	NET_PACKET_PUSH_UINT16(packet, 40);
	NET_PACKET_PUSH_STRING(packet, "Login success!");
	int ret = send(ev->sockid, packet->m_buffer, packet->m_writePos, 0);
	NET_PACKET_END(packet);
	if (ret < 0)
	{
		printf("send err!n");
	}
}
