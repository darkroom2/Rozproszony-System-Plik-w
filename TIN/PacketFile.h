#pragma once
#include "Packet.h"
class PacketFile :
	public Packet
{
public:
	PacketFile();
	~PacketFile();
	std::shared_ptr<std::vector<char> > getBuf();
	int recBuf(std::shared_ptr<std::vector<char> > buf);
	std::string path;
	int modifiedTime;
	std::vector<char> file;
	bool isReceived();
private:
	LoadToBuffer Loading;
	int receivedBytes;
	int pathSize;
	int fileSize;
	bool received;
};

