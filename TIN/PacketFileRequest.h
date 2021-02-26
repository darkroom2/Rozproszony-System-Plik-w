#pragma once
#include "Packet.h"
class PacketFileRequest :
	public Packet
{
public:
	PacketFileRequest();
	~PacketFileRequest();
	std::shared_ptr<std::vector<char> > getBuf();
	int recBuf(std::shared_ptr<std::vector<char> > buf);
	std::string path;
	bool isReceived();
private:
	LoadToBuffer Loading;
	int receivedBytes;
	int pathSize;
	bool received;
};

