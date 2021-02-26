#include "pch.h"
#include "PacketPeerPort.h"


PacketPeerPort::PacketPeerPort():
	Packet(PEER_PORT),
	receivedBytes(0),
	port(0),
	Loading()
{
}


PacketPeerPort::~PacketPeerPort()
{
}

std::shared_ptr<std::vector<char>> PacketPeerPort::getBuf()
{
	std::shared_ptr<std::vector<char>> buf(new std::vector<char>);
	buf->push_back(PEER_PORT);
	pushBack(buf, (char*)&port, sizeof(USHORT));
	return buf;
}

int PacketPeerPort::recBuf(std::shared_ptr<std::vector<char>> buf)
{
	Loading.setBuffer(&buf);
	Loading.load<USHORT>(&port);
	SOCKADDR_IN address;
	received = Loading.isReceived();
	return Loading.i;
}
