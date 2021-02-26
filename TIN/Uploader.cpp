#include "pch.h"
#include "Uploader.h"


Uploader::Uploader(const std::string& _path, Peer* _peer):
	peer(_peer),
	path(_path),
	end(false),
	requests(0)
{
	file.open(LocalUser::getInstance()->path + "\\" + path, std::ifstream::binary);
	if (!file)
	{
		std::cout << "Failed to open file\n";
		end = true;
		return;
	}

	file.seekg(0, file.end);
	length = file.tellg();
	file.seekg(0, file.beg);
	maxRequests = length / 1000000 + (length % 1000000 ? 1 : 0);

	uploadThread.reset(new std::thread(&Uploader::upload, std::ref(*this)));
	sendThread.reset(new std::thread(&Uploader::send, std::ref(*this)));
}

Uploader::~Uploader()
{
	end = true;
	isRequestPacket.notify_one();
	isFilePacket.notify_one();
	uploadThread->join();
	sendThread->join();
}

void Uploader::receivePacket(std::unique_ptr<PacketFileRequest>& _packet)
{
	if (requests < maxRequests)
	{
		++requests;
		isRequestPacket.notify_one();
	}
}

bool Uploader::isEnd()
{
	return end;
}

const std::string & Uploader::getPath()
{
	return path;
}

Peer * Uploader::getPeer()
{
	return peer;
}

void Uploader::upload()
{
	for (int i = 0; i < maxRequests && !end; ++i)
	{
		std::unique_lock<std::mutex> lk(requestMutex);
		while (!end && i == requests) isRequestPacket.wait(lk);
		lk.unlock();
		if (end) return;

		std::unique_ptr<PacketFile> packetFile(new PacketFile);
		packetFile->path = path;
		packetFile->modifiedTime = 54325;
		packetFile->file.resize(min(1000000, length - 1000000 * i));
		file.read(packetFile->file.data(), packetFile->file.size());
		
		std::unique_lock<std::mutex> fileLock(fileMutex);
		filePackets.push(std::move(packetFile));
		fileLock.unlock();
		isFilePacket.notify_one();
	}
	sendEndOfFile();
	file.close();
	end = true;
	isFilePacket.notify_one();
}

void Uploader::sendEndOfFile()
{
	std::unique_ptr<PacketFile> packetFile(new PacketFile);
	packetFile->path = path;
	packetFile->modifiedTime = 54325;
	std::unique_lock<std::mutex> fileLock(fileMutex);
	filePackets.push(std::move(packetFile));
	fileLock.unlock();
	isFilePacket.notify_one();
}

void Uploader::send()
{
	std::unique_lock<std::mutex> fileLock(fileMutex);
	while (!end || !filePackets.empty())
	{
		while (!end && filePackets.empty()) isFilePacket.wait(fileLock);
		if (end && filePackets.empty()) return;
		std::shared_ptr<PacketFile> packet(filePackets.front().release());
		filePackets.pop();
		fileLock.unlock();

		peer->send(packet);
		fileLock.lock();
	}
}
