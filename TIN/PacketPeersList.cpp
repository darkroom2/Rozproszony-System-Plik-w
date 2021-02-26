#include "pch.h"
#include "PacketPeersList.h"


PacketPeersList::PacketPeersList() :
	Packet(PEERS_LIST),
	receivedBytes(0),
	addresses(new std::vector<SOCKADDR_IN>),
	Loading()
{
}


PacketPeersList::~PacketPeersList()
{
}

std::shared_ptr<std::vector<char>> PacketPeersList::getBuf()
{
	std::shared_ptr<std::vector<char>> buf(new std::vector<char>);
	buf->push_back(PEERS_LIST);
	count = addresses->size();
	pushBack(buf, (char*)&count, sizeof(int));
	for (auto address : (*addresses)) {
		buf->push_back(address.sin_addr.S_un.S_un_b.s_b1);
		buf->push_back(address.sin_addr.S_un.S_un_b.s_b2);
		buf->push_back(address.sin_addr.S_un.S_un_b.s_b3);
		buf->push_back(address.sin_addr.S_un.S_un_b.s_b4);
		pushBack(buf, (char*)&address.sin_port, sizeof(USHORT));
	}
	return buf;
}

int PacketPeersList::recBuf(std::shared_ptr<std::vector<char>> buf)
{
	int k = 0;
	SOCKADDR_IN address;
	Loading.setBuffer(&buf);
	Loading.load(&count);
	while (k < count && Loading.i < buf->size()) {
		Loading.load(&address);
		addresses->push_back(address);
		k++;
	}
	received = Loading.isReceived();
	return Loading.i;
}

bool PacketPeersList::isReceived()
{
	return received;
}
