#include "pch.h"
#include "PacketFileList.h"


PacketFileList::PacketFileList() :
	Packet(FILE_LIST),
	receivedBytes(0),
	Loading()
{
}


PacketFileList::~PacketFileList()
{
}

std::shared_ptr<std::vector<char>> PacketFileList::getBuf()
{
	int k = 0;
	std::shared_ptr<std::vector<char>> buf(new std::vector<char>);
	buf->push_back(FILE_LIST);
	count = Files.size();
	pushBack(buf, (char*)&count, sizeof(int));
	while (k < count)
	{
		pathSize.push_back(Files[k].path.size());

		pushBack(buf, (char*)&pathSize[k], sizeof(int));
		pushBack(buf, Files[k].path.c_str(), pathSize[k]);
		pushBack(buf, (char*)&Files[k].modifiedTime, sizeof(LARGE_INTEGER));
		++k;
	}	
	return buf;
}

int PacketFileList::recBuf(std::shared_ptr<std::vector<char>> buf)
{
	int size = 0;
	File f;
	int k = 0;
	Loading.setBuffer(&buf);
	Loading.load<int>(&count);
	while (k < count && Loading.i < buf->size())
	{
		f.path.clear();
		Loading.load<int>(&size);
		pathSize.push_back(size);
		Loading.load(&f.path, size);
		Loading.load<LARGE_INTEGER>(&f.modifiedTime);
		Files.push_back(f);
		++k;
	}

	received = Loading.isReceived();
	
	return Loading.i;
}

bool PacketFileList::isReceived()
{
	return received;
}
