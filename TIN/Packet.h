#pragma once
#include "LoadToBuffer.h"

struct File
{
	std::string path;
	LARGE_INTEGER modifiedTime;
};

class Packet
{
public:
	enum Type
	{
		EMPTY,
		PEERS_REQUEST,
		PEERS_LIST,
		PEER_PORT,
		FILE_LIST_REQUEST,
		FILE_LIST,
		FILE_LIST_UPDATE,
		FILE_REQUEST,
		FILE_FIRST_REQUEST,
		FILE
	};
	Packet();
	~Packet();
	Type getType();
	virtual std::shared_ptr<std::vector<char> > getBuf();
	virtual int recBuf(std::shared_ptr<std::vector<char> > buf);
	virtual bool isReceived();
protected:
	Packet(Type type);
	void pushBack(std::shared_ptr<std::vector<char> > vector, const char* buf, int size);
	Type type;
};

