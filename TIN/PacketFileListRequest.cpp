#include "pch.h"
#include "PacketFileListRequest.h"


PacketFileListRequest::PacketFileListRequest() :
	Packet(FILE_LIST_REQUEST)
{
}


PacketFileListRequest::~PacketFileListRequest()
{
}

std::shared_ptr<std::vector<char>> PacketFileListRequest::getBuf()
{
	std::shared_ptr<std::vector<char>> buf(new std::vector<char>);
	buf->push_back(FILE_LIST_REQUEST);
	return buf;
}
