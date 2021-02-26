#include "pch.h"
#include "PacketFile.h"


PacketFile::PacketFile():
	Packet(FILE),
	receivedBytes(0),
	received(false),
	Loading()
{
}


PacketFile::~PacketFile()
{
}

std::shared_ptr<std::vector<char>> PacketFile::getBuf()
{
	std::shared_ptr<std::vector<char>> buf(new std::vector<char>);
	buf->push_back(FILE);
	pathSize = path.size();
	pushBack(buf, (char*)&pathSize, sizeof(int));
	pushBack(buf, path.c_str(), pathSize);
	pushBack(buf, (char*)&modifiedTime, sizeof(int));
	fileSize = file.size();
	pushBack(buf, (char*)&fileSize, sizeof(int));
	pushBack(buf, file.data(), fileSize);
	return buf;
}

int PacketFile::recBuf(std::shared_ptr<std::vector<char>> buf)
{
	Loading.setBuffer(&buf);
	Loading.load<int>(&pathSize);
	Loading.load(&path, pathSize);
	Loading.load<int>(&modifiedTime);
	Loading.load<int>(&fileSize);
	Loading.load(&file, fileSize);
	received = Loading.isReceived();
	return Loading.i;
}

bool PacketFile::isReceived()
{
	return received;
}
