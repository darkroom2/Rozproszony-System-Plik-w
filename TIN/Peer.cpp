#include "pch.h"
#include "Peer.h"
#include "PacketFile.h"
#include "PacketFileList.h"
#include "PacketFileListRequest.h"
#include "PacketFileListUpdate.h"
#include "PacketFileRequest.h"
#include "PacketFileFirstRequest.h"
#include "PacketPeersList.h"
#include "PacketPeersRequest.h"
#include "PacketPeerPort.h"

Peer::Peer(SOCKET _socket) :
	socket(_socket),
	packet(nullptr),
	connected(true)
{
}


Peer::~Peer()
{
	std::unique_lock<std::mutex> lk(sendMutex);
	closesocket(socket);
}

bool Peer::isConnected() const
{
	return connected;
}

void Peer::send(std::shared_ptr<Packet> packet)
{
	std::unique_lock<std::mutex> lk(sendMutex);
    auto msg = packet->getBuf();
	int send_bytes = ::send(this->socket, msg->data(), msg->size(), 0);
}

void Peer::setPort(USHORT port)
{
	this->port = port;
}

std::shared_ptr<Packet> Peer::receive()
{
	while (true)
	{
		if (!buf)
		{
			buf.reset(new std::vector<char>(500000));
			int recBytes = recv(this->socket, buf->data(), buf->size(), 0);
			if (recBytes == 0)
			{
				connected = false;
				return std::shared_ptr<Packet>();
			}
			else if (recBytes == SOCKET_ERROR)
			{
				int error = WSAGetLastError();
				if (error != WSAEWOULDBLOCK)
				{
					std::cout << "Receive error " << WSAGetLastError() << std::endl;
					connected = false;
					return std::shared_ptr<Packet>();
				}
				buf.reset();
				return std::shared_ptr<Packet>();
			}
			buf->resize(recBytes);
		}
		if (packet == nullptr)
		{
			packet = GetPacketObject((Packet::Type)(*buf)[0]);
			if (!packet)
			{
				connected = false;
				return std::shared_ptr<Packet>();
			}
			buf->erase(buf->begin());
		}
		int recBytes = packet->recBuf(buf);
		if (recBytes < buf->size())
		{
			buf->erase(buf->begin(), buf->begin() + recBytes);
		}
		else
		{
			buf.reset();
		}
		if (packet->isReceived())
		{
			auto result = packet;
			packet.reset();
			return result;
		}
	}
	return std::shared_ptr<Packet>();
}

std::shared_ptr<SOCKADDR_IN> Peer::getAddress() const
{
	SOCKADDR_IN *address = new SOCKADDR_IN;
	int addrsize = sizeof(SOCKADDR_IN);
	getpeername(socket, (sockaddr*)address, &addrsize);
	if(port!=0) address->sin_port = port;
	return std::shared_ptr<SOCKADDR_IN>(address);
}

SOCKET Peer::getSocket() const
{
	return socket;
}

std::shared_ptr<Packet> Peer::GetPacketObject(Packet::Type type)
{
	std::shared_ptr<Packet> packet;
	switch (type)
	{
	case Packet::EMPTY: packet.reset(new Packet); break;
	case Packet::FILE: packet.reset(new PacketFile); break;
	case Packet::FILE_LIST: packet.reset(new PacketFileList); break;
	case Packet::FILE_LIST_REQUEST: packet.reset(new PacketFileListRequest); break;
	case Packet::FILE_LIST_UPDATE: packet.reset(new PacketFileListUpdate); break;
	case Packet::FILE_FIRST_REQUEST: packet.reset(new PacketFileFirstRequest); break;
	case Packet::FILE_REQUEST: packet.reset(new PacketFileRequest); break;
	case Packet::PEERS_LIST: packet.reset(new PacketPeersList); break;
	case Packet::PEERS_REQUEST: packet.reset(new PacketPeersRequest); break;
	case Packet::PEER_PORT: packet.reset(new PacketPeerPort); break;
	}
	return packet;
}

std::ostream &operator<<(std::ostream &os, Peer const &peer)
{
	auto address = peer.getAddress();
	return os << (int)address->sin_addr.S_un.S_un_b.s_b1
	   << "." << (int)address->sin_addr.S_un.S_un_b.s_b2
	   << "." << (int)address->sin_addr.S_un.S_un_b.s_b3
	   << "." << (int)address->sin_addr.S_un.S_un_b.s_b4
	   << ":" << (int)address->sin_port;
}