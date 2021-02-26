#include "pch.h"
#include "PacketPeersRequest.h"


PacketPeersRequest::PacketPeersRequest() :
	Packet(PEERS_REQUEST)
{
}

PacketPeersRequest::~PacketPeersRequest()
{
}

std::shared_ptr<std::vector<char>> PacketPeersRequest::getBuf()
{
	std::shared_ptr<std::vector<char>> buf(new std::vector<char>);
	buf->push_back(PEERS_REQUEST);
	return buf;
}

int PacketPeersRequest::recBuf(std::shared_ptr<std::vector<char>> buf)
{
	return 0;
}
