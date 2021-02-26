#pragma once
#include "Packet.h"
#include <vector>


class PacketFileList :
	public Packet
{
public:
	PacketFileList();
	~PacketFileList();
	std::shared_ptr<std::vector<char> > getBuf();
	int recBuf(std::shared_ptr<std::vector<char> > buf);	
	int count;
	std::vector <File> Files;
	bool isReceived();
private:
	LoadToBuffer Loading;
	int receivedBytes;
	std::vector <int> pathSize;
	bool received;
};