#pragma once
#include "Packet.h"
class PacketPeerPort :
	public Packet
{
public:
	PacketPeerPort();
	~PacketPeerPort();
	USHORT port;
	std::shared_ptr<std::vector<char>> getBuf();
	int recBuf(std::shared_ptr<std::vector<char>> buf);

private:
	LoadToBuffer Loading;
	int receivedBytes;
	bool received;
};

