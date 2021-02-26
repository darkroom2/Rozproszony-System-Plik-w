#pragma once
#include "Packet.h"
class PacketPeersRequest :
	public Packet
{
public:
	PacketPeersRequest();
	~PacketPeersRequest(); std::shared_ptr<std::vector<char> > getBuf();
	int recBuf(std::shared_ptr<std::vector<char> > buf);
};

