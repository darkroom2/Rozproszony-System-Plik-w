#pragma once

#include "Packet.h"

class PacketFileListUpdate :
        public Packet {
public:
    PacketFileListUpdate();

    ~PacketFileListUpdate();

    std::shared_ptr<std::vector<char> > getBuf();

    int recBuf(std::shared_ptr<std::vector<char> > buf);

    std::string path;
	LARGE_INTEGER modifiedTime;
	DWORD action;

    bool isReceived() override;

private:
	LoadToBuffer Loading;
    int receivedBytes;
    int pathSize;
    bool received;
};

