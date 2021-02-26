#include "pch.h"
#include "Packet.h"

Packet::Packet() :
	type(EMPTY)
{
}

Packet::~Packet()
{
}

Packet::Type Packet::getType()
{
	return type;
}

std::shared_ptr<std::vector<char>> Packet::getBuf()
{
	std::shared_ptr<std::vector<char>> buf(new std::vector<char>);
	buf->push_back(EMPTY);
	return buf;
}

int Packet::recBuf(std::shared_ptr<std::vector<char>> buf)
{
	return 0;
} 

bool Packet::isReceived()
{
	return true;
}



Packet::Packet(Type _type) :
	type(_type)
{
}

void Packet::pushBack(std::shared_ptr<std::vector<char>> vector, const char * buf, int size)
{
	for (int i = 0; i < size; ++i)
	{
		vector->push_back(buf[i]);
	}
}
