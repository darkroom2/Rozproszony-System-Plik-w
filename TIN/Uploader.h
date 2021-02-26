#pragma once

#include "Peer.h"
#include "PacketFile.h"
#include "PacketFileRequest.h"
#include "LocalUser.h"

class Uploader
{
public:
	Uploader(const std::string& path, Peer* peer);
	~Uploader();
	void receivePacket(std::unique_ptr<PacketFileRequest>& packet);
	bool isEnd();
	const std::string& getPath();
	Peer* getPeer();
private:
	std::unique_ptr<std::thread> uploadThread;
	std::unique_ptr<std::thread> sendThread;
	std::condition_variable isRequestPacket;
	std::mutex requestMutex;
	Peer* peer;
	std::string path;
	void upload();
	std::atomic_bool end;
	std::atomic_int requests;
	int maxRequests;
	int length;
	std::ifstream file;
	void sendEndOfFile();
	std::queue<std::unique_ptr<PacketFile>> filePackets;
	std::mutex fileMutex;
	std::condition_variable isFilePacket;
	void send();
};

