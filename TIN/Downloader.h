#pragma once

#include "Peer.h"
#include "PacketFile.h"
#include "PacketFileRequest.h"
#include "PacketFileFirstRequest.h"
#include "LocalUser.h"

class Downloader
{
public:
	Downloader(const std::string& path, Peer* peer);
	~Downloader();
	void receivePacket(std::unique_ptr<PacketFile>& packet);
	bool isEnd();
	const std::string& getPath();
private:
	std::unique_ptr<std::thread> thread;
	std::condition_variable isPacket;
	std::mutex mutex;
	Peer* peer;
	std::string path;
	void download();
	std::queue<std::unique_ptr<PacketFile>> packets;
	std::atomic_bool end;
	std::atomic_bool endOfFile;
	void sendFirstRequest();
};

