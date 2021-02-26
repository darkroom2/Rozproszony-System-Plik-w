#pragma once
#include "Packet.h"
class PacketPeersList :
	public Packet
{
public:
	PacketPeersList();
	~PacketPeersList();
	std::shared_ptr<std::vector<char>> getBuf();
	int recBuf(std::shared_ptr<std::vector<char>> buf);
	int count;
	std::shared_ptr<std::vector<SOCKADDR_IN>> addresses;
	bool isReceived();
private:
	LoadToBuffer Loading;
	int receivedBytes;
	bool received;
};

