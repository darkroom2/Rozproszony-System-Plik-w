#include "pch.h"
#include "Downloader.h"


Downloader::Downloader(const std::string& _path, Peer* _peer):
	peer(_peer),
	path(_path),
	end(false),
	endOfFile(false)
{
	thread.reset(new std::thread(&Downloader::download, std::ref(*this)));
}

Downloader::~Downloader()
{
	end = true;
	isPacket.notify_one();
	thread->join();
}

void Downloader::receivePacket(std::unique_ptr<PacketFile>& packet)
{
	if (packet->file.empty())
	{
		endOfFile = true;
	}
	else
	{
		std::unique_lock<std::mutex> lk(mutex);
		packets.push(std::unique_ptr<PacketFile>(packet.release()));
	}
	isPacket.notify_one();
}

bool Downloader::isEnd()
{
	return end;
}

const std::string & Downloader::getPath()
{
	return path;
}

void Downloader::download()
{
	sendFirstRequest();
	std::shared_ptr<PacketFileRequest> rpacket(new PacketFileRequest);
	rpacket->path = path;
	for (int i = 0; i < 4; ++i)peer->send(rpacket);
	
	std::ofstream file(LocalUser::getInstance()->path + "\\" + path, std::ofstream::binary);
	if (!file)
	{
		std::cout << "Failed to open file\n";
		end = true;
	}

	while (!end && (!packets.empty() || !endOfFile))
	{
		std::unique_lock<std::mutex> lk(mutex);
		while (packets.empty() && !end && (!packets.empty() || !endOfFile)) isPacket.wait(lk);
		if (end || (packets.empty() && endOfFile)) break;
		std::unique_ptr<PacketFile> packet(packets.front().release());
		packets.pop();
		lk.unlock();

		file.write(packet->file.data(), packet->file.size());

		if (!endOfFile)
		{
			rpacket.reset(new PacketFileRequest);
			rpacket->path = path;
			peer->send(rpacket);
		}
	}
	file.close();
	end = true;
}

void Downloader::sendFirstRequest()
{
	std::shared_ptr<PacketFileFirstRequest> rpacket(new PacketFileFirstRequest);
	rpacket->path = path;
	peer->send(rpacket);
}
