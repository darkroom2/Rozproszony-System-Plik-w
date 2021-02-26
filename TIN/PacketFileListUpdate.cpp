#include "pch.h"
#include "PacketFileListUpdate.h"


PacketFileListUpdate::PacketFileListUpdate() :
        Packet(FILE_LIST_UPDATE),
        receivedBytes(0),
        received(false),
		Loading()
{
}


PacketFileListUpdate::~PacketFileListUpdate() {
}

std::shared_ptr<std::vector<char> > PacketFileListUpdate::getBuf() {
    std::shared_ptr<std::vector<char>> buf(new std::vector<char>);
    buf->push_back(FILE_LIST_UPDATE);
    this->pathSize = this->path.size();
    pushBack(buf, reinterpret_cast<const char *>(&this->pathSize), sizeof(int));
    pushBack(buf, this->path.c_str(), this->pathSize);
    pushBack(buf, reinterpret_cast<const char *>(&this->modifiedTime), sizeof(LARGE_INTEGER));
    pushBack(buf, reinterpret_cast<const char *>(&this->action), sizeof(DWORD));

    return buf;
}

int PacketFileListUpdate::recBuf(std::shared_ptr<std::vector<char> > buf) {
	Loading.setBuffer(&buf);
	Loading.load<int>(&pathSize);
	Loading.load(&path, pathSize);
	Loading.load<LARGE_INTEGER>(&modifiedTime);
	Loading.load(&action);
    received = Loading.isReceived();
    return Loading.i;
}

bool PacketFileListUpdate::isReceived() {
    return received;
}