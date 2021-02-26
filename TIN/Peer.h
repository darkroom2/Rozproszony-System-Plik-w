#pragma once

#include "Packet.h"

class Peer
{
public:
	std::vector <File> files;
	Peer(SOCKET socket);
	~Peer();
	bool isConnected() const;
	void send(std::shared_ptr<Packet> packet);
	void setPort(USHORT newPort);
	std::shared_ptr<Packet> receive();
	std::shared_ptr<SOCKADDR_IN> getAddress() const;
	SOCKET getSocket() const;
private:
	std::shared_ptr<Packet> GetPacketObject(Packet::Type type);
	SOCKET socket;
	std::shared_ptr<Packet> packet;
	bool connected;
	std::shared_ptr<std::vector<char>> buf;
	USHORT port;
	std::mutex sendMutex;
};

std::ostream &operator<<(std::ostream &os, Peer const &peer);