#pragma once
#include "Packet.h"
class PacketFileListRequest :
	public Packet
{
public:
	PacketFileListRequest();
	~PacketFileListRequest();
	std::shared_ptr<std::vector<char> > getBuf();
};
