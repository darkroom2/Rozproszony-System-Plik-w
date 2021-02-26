#include "pch.h"
#include "PacketFileRequest.h"


PacketFileRequest::PacketFileRequest():
	Packet(FILE_REQUEST),
	receivedBytes(0),
	Loading()
{
}


PacketFileRequest::~PacketFileRequest()
{
}

std::shared_ptr<std::vector<char>> PacketFileRequest::getBuf()
{
	std::shared_ptr<std::vector<char>> buf(new std::vector<char>);
	buf->push_back(type);
	pathSize = path.size();
	pushBack(buf, (char*)&pathSize, sizeof(int));
	pushBack(buf, path.c_str(), pathSize);
	return buf;
}

int PacketFileRequest::recBuf(std::shared_ptr<std::vector<char>> buf)
{
	Loading.setBuffer(&buf);
	Loading.load<int>(&pathSize);
	Loading.load(&path, pathSize);
	received = Loading.isReceived();
	return Loading.i;
}

bool PacketFileRequest::isReceived()
{
	return received;
}
