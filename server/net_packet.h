#ifndef __NET_PACKET_H__
#define __NET_PACKET_H__

#define DEFAULT_SIZE 0x1000
#define DEFAULT_INCREASE_SIZE 200

#define NET_PACKET_BEGIN(a,b) NetPacket a;netpackInit(a, b);
#define NET_PACKET_END(a) netpackRelease(a);

struct NetPacket
{
	unsigned int opcode;
	unsigned char* m_buffer;
	size_t m_readPos;
	size_t m_writePos;
	unsigned int m_buffersize;
};

void netpackSetOpcode(NetPacket* packet, unsigned int opcode)
{
	packet.opcode = opcode;
}

void netpackInit(NetPacket* packet, size_t bufferSize = DEFAULT_SIZE)
{
	packet.opcode = 0;
	packet.m_buffer = 0;
	packet.m_readPos = 0;
	packet.m_writePos = 0;
	packet.m_buffersize = 0;
	netpackReserve(packet, bufferSize);
}

void netpackReserve(NetPacket* packet, size_t bufferSize)
{
	if (packet.m_buffer)
		packet.m_buffer = (unsigned char*)realloc(packet.m_buffer, bufferSize);
	else
		packet.m_buffer = (unsigned char*)malloc(bufferSize);

	packet.m_buffersize = bufferSize;
}

unsigned int netpackGetBufferSize(const NetPacket* const packet)
{
	return packet.m_writePos;
}

template<typename T>
T netpackRead(NetPacket* packet)
{
	if (packet.m_readPos + sizeof(T) > packet.m_writePos)
		return (T)0;
	T ret = *(T*)&(packet.m_buffer[m_readPos]);
	packet.m_readPos += sizeof(T);
	return ret;
}

void netpackReadbuff(NetPacket* packet, uint8_t* buffer, size_t len)
{
	if (packet.m_readPos + len > packet.m_writePos)
		len = (packet.m_writePos - packet.m_readPos);
	memcpy(buffer, &(packet.m_buffer[packet.m_readPos]), len);
	packet.m_readPos += len;
}

template<typename T>
void netpackWrite(NetPacket* packet, const T & data)
{
	size_t new_size = packet.m_writePos + sizeof(T);
	if (new_size > packet.m_writePos)
	{
		new_size = (new_size / DEFAULT_INCREASE_SIZE + 1) * DEFAULT_INCREASE_SIZE;
		netpackReserve(packet, new_size);
	}
	*(T*)(&packet.m_buffer[packet.m_writePos]) = data;
	packet.m_writePos += sizeof(T);
}

void netpackWriteBuff(NetPacket* packet, const uint8_t* buffer, size_t len)
{
	size_t new_size = packet.m_writePos + len;
	if (new_size > packet.m_buffersize)
	{
		new_size = (new_size / DEFAULT_INCREASE_SIZE + 1) * DEFAULT_INCREASE_SIZE;
		netpackReserve(packet, new_size);
	}
	memcpy(&(packet.m_buffer[packet.m_writePos]), buffer, len);
	packet.m_writePos += len;
}

bool netpackReadString(NetPacket* packet, std::string& str)
{
	uint32_t len;
	len = netpackRead<uint32_t>(packet);
	if (len == 0)
		return true;
	std::vector<uint8_t> buf;
	buf.reserve(len + 1);
	unsigned int bufSize = netpackGetBufferSize(packet);
	if (bufSize < len)
		return false;
	netpackReadbuff(packet, &(buf[0]), len);
	buf[len] = 0;
	str = (char*)(&(buf[0]));
	return true;
}

bool netpackWriteString(NetPacket* packet, std::string const &str)
{
	uint32_t len = str.size();
	netpackWrite<uint8_t>(packet, len);
	if (len > 0)
		netpackWriteBuff(packet, (const uint8_t*)str.c_str(), len);	
}

void netpackRelease(NetPacket* packet)
{
	if (packet.m_buffer)
		free(packet.m_buffer);
}

#define NET_PACKET_POP_UINT64(a,b) b = netpackRead<uint64_t>(a)
#define NET_PACKET_POP_UINT32(a,b) b = netpackRead<uint32_t>(a)
#define NET_PACKET_POP_UINT16(a,b) b = netpackRead<uint16_t>(a)
#define NET_PACKET_POP_UINT8(a,b) b = netpackRead<uint8_t>(a)
#define NET_PACKET_POP_INT64(a,b) b = netpackRead<int64_t>(a)
#define NET_PACKET_POP_INT32(a,b) b = netpackRead<int32_t>(a)
#define NET_PACKET_POP_INT16(a,b) b = netpackRead<int16_t>(a)
#define NET_PACKET_POP_INT8(a,b) b = netpackRead<int8_t>(a)
#define NET_PACKET_POP_FLOAT(a,b) b = netpackRead<float>(a)
#define NET_PACKET_POP_DOUBLE(a,b) b = netpackRead<double>(a)
#define NET_PACKET_POP_STRING(a,b) netpackReadString(a, b)



#define NET_PACKET_PUSH_UINT64(a,b) netpackWrite<uint64_t>(a,b)
#define NET_PACKET_PUSH_UINT32(a,b) netpackWrite<uint32_t>(a,b)
#define NET_PACKET_PUSH_UINT16(a,b) netpackWrite<uint16_t>(a,b)
#define NET_PACKET_PUSH_UINT8(a,b) netpackWrite<uint8_t>(a,b)
#define NET_PACKET_PUSH_INT64(a,b) netpackWrite<int64_t>(a,b)
#define NET_PACKET_PUSH_INT32(a,b) netpackWrite<int32_t>(a,b)
#define NET_PACKET_PUSH_INT16(a,b) netpackWrite<int16_t>(a,b)
#define NET_PACKET_PUSH_INT8(a,b) netpackWrite<int8_t>(a,b)
#define NET_PACKET_PUSH_FLOAT(a,b) netpackWrite<float>(a,b)
#define NET_PACKET_PUSH_DOUBLE(a,b) netpackWrite<double>(a,b)
#define NET_PACKET_PUSH_STRING(a,b) netpackWriteString(a,b)



#endif
