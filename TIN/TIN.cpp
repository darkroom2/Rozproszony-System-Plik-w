#include "pch.h"
#include "Peer.h"
#include "Packet.h"
#include "PacketFileRequest.h"
#include "PacketFileListUpdate.h"
#include "PacketFileList.h"
#include "PacketPeerPort.h"
#include "PacketPeersList.h"
#include "PacketPeersRequest.h"
#include "LocalUser.h"
#include "Directory.h"
#include "PacketFile.h"
#include "Downloader.h"
#include "Uploader.h"

void send_to_all(std::vector<std::unique_ptr<Peer>>& peers, std::shared_ptr<Packet> packet) {
	// TODO: probably also need to pass eventHandles and check isConnected() etc.
	std::cout << "Wysylam do wszystkich!" << std::endl;
	for (const auto& i : peers) {
		i->send(packet);
	}
	/*for (unsigned int i = 0; i < peers.size(); ++i)
	{
		std::shared_ptr<PacketFileListUpdate> packet(new PacketFileListUpdate);
		packet->path = path.c_str();
		packet->modifiedTime = modifiedTime;
		peers[i]->send(packet);
	}*/
}

SOCKET createServerSocket (int port)
{
	SOCKET socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket == INVALID_SOCKET)
		return INVALID_SOCKET;

	sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (bind(socket, (SOCKADDR *)& address, sizeof(address)) == SOCKET_ERROR)
	{
		closesocket(socket);
		return INVALID_SOCKET;
	}

	if (listen(socket, 1) == SOCKET_ERROR)
	{
		closesocket(socket);
		return INVALID_SOCKET;
	}

	u_long NonBlock = 1;
	if (ioctlsocket(socket, FIONBIO, &NonBlock) == SOCKET_ERROR)
	{
		closesocket(socket);
		return INVALID_SOCKET;
	}

	std::cout << "Listening on " << port << std::endl;

	return socket;
}

SOCKET createClientSocket(SOCKADDR_IN address) {
	//::string input;
	SOCKET socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket == INVALID_SOCKET)
		return INVALID_SOCKET;

	if (connect(socket, (SOCKADDR*)& address, sizeof(address)) == SOCKET_ERROR)
	{
		closesocket(socket);
		return INVALID_SOCKET;
	}

	u_long NonBlock = 1;
	if (ioctlsocket(socket, FIONBIO, &NonBlock) == SOCKET_ERROR)
	{
		closesocket(socket);
		return INVALID_SOCKET;
	}

	return socket;
}

SOCKET createClientSocket(std::string &ip, std::string &port)
{
	sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;

	inet_pton(AF_INET, (PCSTR)ip.c_str(), &address.sin_addr.s_addr);

	address.sin_port = htons(atoi(port.c_str()));

	return createClientSocket(address);
}

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		std::cout << argv[0] << " <port>";
		std::cout << " <minimum peer count>";
		std::cout << " [<dir>]" << std::endl;
		return -1;
	}

	// default path to store files
	const char* path = "C:\\wtf";
	// if specified different path use it
	if (argc == 4) {
		path = argv[3];
	}

	LocalUser::getInstance()->path = path;

	auto dir = new Directory(path);

	LocalUser::getInstance()->setMinimumPeersCount(atoi(argv[2]));

	// init socket
	WSADATA wsaData;
	WORD ver = MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &wsaData);

	if (wsOk != 0)
	{
		std::cerr << "Cannot initialize\n";
		return -1;
	}
	int serverPort = atoi(argv[1]);
	SOCKET serverSocket = createServerSocket(serverPort);
	if (serverSocket == INVALID_SOCKET)
	{
		std::cerr << "Error creating socket\n";
		WSACleanup();
		return -1;
	}
	std::vector<std::unique_ptr<Peer>> peers;
	std::vector<HANDLE> eventHandles;
	std::vector<std::unique_ptr<Downloader>> downloaders;
	std::vector<std::unique_ptr<Uploader>> uploaders;
	dir->addEventHandle(eventHandles);
	eventHandles.push_back(GetStdHandle(STD_INPUT_HANDLE));
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(serverSocket, &fds);
	HANDLE sockEvent = WSACreateEvent();
	WSAEventSelect(serverSocket, sockEvent, FD_ACCEPT);
	eventHandles.push_back(sockEvent);
	bool abort = false;
	while (!abort)
	{

		abort = dir->checkForDirChanges();

		DWORD event = WaitForMultipleObjects(eventHandles.size(), eventHandles.data(), false, INFINITE);
		
		if (event == WAIT_FAILED)
		{
			std::cout << "ERROR: Wait failed " << GetLastError() << std::endl;
			abort = true;
		}
		if (event == 0) {
			// TODO: pull information about actual change and send it to peers
			//dir->updateFileList();
			std::shared_ptr<PacketFileListUpdate> packet(new PacketFileListUpdate);
			packet->path = dir->getFileName();
			packet->modifiedTime = dir->getModifiedTime();
			packet->action = dir->getAction();
			send_to_all(peers, packet);
			ResetEvent(eventHandles[event]);
			continue;
		}
		else if (event == 1)
		{
			while (_kbhit())
			{
				int key = _getch();
				if (key == 27) abort = true;
				else if (key == 's') {
					int id;
					std::string path;
					std::cout << "Peer id: ";
					std::cin >> id;
					std::cout << "File path: ";
					std::cin >> path;
					if (id >= 0 && id < peers.size())
					{
						downloaders.push_back(std::unique_ptr<Downloader>(new Downloader(path, peers[id].get())));
					}

				}
				else if (key == 'd') {
					std::shared_ptr<PacketFileListUpdate> packet(new PacketFileListUpdate);
					int id;
					std::cout << "Peer id: ";
					std::cin >> id;
					std::cout << "File path: ";
					std::cin >> packet->path;
					if (id >= 0 && id < peers.size()) peers[id]->send(packet);

				}
				else if (key == 'l') {
					//std::shared_ptr<PacketFileList> packet(new PacketFileList);
					//packet->Files = dir->getFiles();
					//send_to_all(peers, packet);
					for (const auto& i : peers) {
						std::cout << "Peer " << i << " file list:" << std::endl;
						for (const auto& j : i->files) {
							std::cout << "\t" << j.path << " | " << j.modifiedTime.QuadPart << std::endl;
						}
					}
				}
				else if (key == 'a')
				{
					std::string ip;
					std::cout << "IP: ";
					std::cin >> ip;

					std::string port;
					std::cout << "port: ";
					std::cin >> port;

					SOCKET newSocket = createClientSocket(ip, port);
					if (newSocket != SOCKET_ERROR)
					{
						Peer* peer = new Peer(newSocket);
						peers.push_back(std::unique_ptr<Peer>(peer));
						std::shared_ptr<PacketPeerPort> packetPeerPort(new PacketPeerPort);
						packetPeerPort->port = serverPort;
						std::cout << "Connection established\n";
						sockEvent = WSACreateEvent();
						WSAEventSelect(newSocket, sockEvent, FD_READ|FD_CLOSE);
						eventHandles.push_back(sockEvent);
						std::shared_ptr<PacketPeersRequest> packetPeersRequest(new PacketPeersRequest);
						peer->send(packetPeersRequest);
						peer->send(packetPeerPort);
						++LocalUser::getInstance()->peerListRequestIndex;

						std::shared_ptr<PacketFileList> packetFileList(new PacketFileList);
						packetFileList->Files = dir->getFiles();
						peer->send(packetFileList);
					}
					else
						std::cout << "Connection error\n";
				}
				else if (key == 'p')
				{
					std::cout << "Addresses list: " << std::endl;

					for (int i = 0; i < LocalUser::getInstance()->getPeerAddresses().size(); ++i) {
						std::cout << (int)LocalUser::getInstance()->getPeerAddresses()[i].get()->sin_addr.S_un.S_un_b.s_b1 << ".";
						std::cout << (int)LocalUser::getInstance()->getPeerAddresses()[i].get()->sin_addr.S_un.S_un_b.s_b2 << ".";
						std::cout << (int)LocalUser::getInstance()->getPeerAddresses()[i].get()->sin_addr.S_un.S_un_b.s_b3 << ".";
						std::cout << (int)LocalUser::getInstance()->getPeerAddresses()[i].get()->sin_addr.S_un.S_un_b.s_b4 << ":";
						std::cout << LocalUser::getInstance()->getPeerAddresses()[i].get()->sin_port << std::endl;
					}
						
				}
				else if (key == 'x')
				{
					std::cout << "Peers adresses list: " << std::endl;
					for (int i = 0; i < peers.size(); ++i) {
						std::cout << *peers[i] << std::endl;
					}
				}
			}
			FlushConsoleInputBuffer(eventHandles[event]);
		}
		else if (event == 2)
		{
			SOCKET acceptSocket = accept(serverSocket, NULL, NULL);
			if (acceptSocket != SOCKET_ERROR)
			{
				Peer* newPeer = new Peer(acceptSocket);
				peers.push_back(std::unique_ptr<Peer>(newPeer));
				sockEvent = WSACreateEvent();
				WSAEventSelect(acceptSocket, sockEvent, FD_READ|FD_CLOSE);
				eventHandles.push_back(sockEvent);
				std::shared_ptr<PacketPeerPort> packetPeerPort(new PacketPeerPort);
				packetPeerPort->port = serverPort;
				newPeer->send(packetPeerPort);
				std::cout << "New peer " << *newPeer << std::endl;
			}
			WSANETWORKEVENTS networkEvents;
			WSAEnumNetworkEvents(serverSocket, eventHandles[event], &networkEvents);
		}
		else
		{
			Peer* peer = peers[event - 3].get();
			for (auto packet = peer->receive(); packet; packet = peer->receive())
			{
				if (packet->getType() == Packet::FILE_FIRST_REQUEST)
				{
					std::unique_ptr<PacketFileFirstRequest> fileRequest(new PacketFileFirstRequest);
					*fileRequest = *((PacketFileFirstRequest*)&(*packet));
					uploaders.push_back(std::unique_ptr<Uploader>(new Uploader(fileRequest->path, peer)));
				}
				else if (packet->getType() == Packet::FILE_REQUEST)
				{
					std::unique_ptr<PacketFileRequest> fileRequest(new PacketFileRequest);
					*fileRequest = *((PacketFileRequest*)&(*packet));
					for (int i = 0; i < uploaders.size(); ++i)
					{
						if (uploaders[i]->isEnd())
						{
							uploaders.erase(uploaders.begin() + i);
							--i;
							continue;
						}
						if (uploaders[i]->getPath() == fileRequest->path && uploaders[i]->getPeer() == peer)
						{
							uploaders[i]->receivePacket(fileRequest);
							break;
						}
					}
				}
				else if (packet->getType() == Packet::FILE)
				{
					PacketFile *packetFile = (PacketFile*)&(*packet);
					for (int i = 0; i < downloaders.size(); ++i)
					{
						if (downloaders[i]->isEnd())
						{
							downloaders.erase(downloaders.begin() + i);
							--i;
							continue;
						}
						if (downloaders[i]->getPath() == packetFile->path)
						{
							std::unique_ptr<PacketFile> pointer(new PacketFile(*packetFile));
							downloaders[i]->receivePacket(pointer);
							break;
						}
					}
				}
				else if (packet->getType() == Packet::FILE_LIST_UPDATE)
				{
					PacketFileListUpdate* fileListUpdate = (PacketFileListUpdate*) & (*packet);
					auto act = fileListUpdate->action;
					std::cout << "File list update from " << event - 3 << " path: " << fileListUpdate->path << " modified at: " << fileListUpdate->modifiedTime.QuadPart << " | action: " << act << std::endl;
					if (act == FILE_ACTION_ADDED) {
						File file;
						file.path = fileListUpdate->path;
						file.modifiedTime = fileListUpdate->modifiedTime;
						peer->files.push_back(file);
						// also download file there (send file req) ???????????????
					}
					else if (act == FILE_ACTION_REMOVED) {
						// for now remove and list again, later add finding element or change vector to smth else 
						// add removing path from vector
						// also remove the physical file there?????
					}
					else if (act == FILE_ACTION_MODIFIED or act == FILE_ACTION_RENAMED_OLD_NAME or act == FILE_ACTION_RENAMED_NEW_NAME) {
						// find file and update modified time
						// download the file and update it... ????
					}
					else {
						std::cout << "Nieprzewidziane zachowanie Directory::updateFileList()" << std::endl;
					}
				}
				else if (packet->getType() == Packet::PEER_PORT) {
					PacketPeerPort* peerPort = (PacketPeerPort*) & (*packet);
					std::cout << "Peer port from " << event - 3 << " port: " << peerPort->port << std::endl;
					SOCKADDR_IN address = *peer->getAddress();
					address.sin_port = peerPort->port;
					//peer->setPort(peerPort->port);
					LocalUser::getInstance()->addPeerAddress(&address);
				}
				else if (packet->getType() == Packet::FILE_LIST) {
					PacketFileList* fileList = (PacketFileList*) & (*packet);
					std::cout << "File list from " << event - 3 << " :" << std::endl;
					peer->files = fileList->Files;
					for (const auto& i : fileList->Files) {
						std::cout << i.path << " | modified on: " << i.modifiedTime.QuadPart << std::endl;
					}
				}
				else if (packet->getType() == Packet::PEERS_REQUEST) {
					std::cout << "Peers request from " << event - 3 << std::endl;
					PacketPeersList* packetPeersList = new PacketPeersList();
					packetPeersList->count = LocalUser::getInstance()->getPeerAddresses().size();
					for (int i = 0; i < LocalUser::getInstance()->getPeerAddresses().size(); ++i) {
						packetPeersList->addresses->push_back(*LocalUser::getInstance()->getPeerAddresses()[i]);
					}
					peer->send(std::shared_ptr<PacketPeersList>(packetPeersList));
				}
				else if (packet->getType() == Packet::PEERS_LIST) {
					std::cout << "Peers list from " << event - 3 << std::endl;
					PacketPeersList* packetPeersList = (PacketPeersList*) & (*packet);
					for (int i = 0; i < packetPeersList->count; ++i) {
						LocalUser::getInstance()->addPeerAddress(&(*(packetPeersList->addresses.get()))[i]);
					}

					if (peers.size() < LocalUser::getInstance()->getMinimumPeersCount()
						&& LocalUser::getInstance()->getPeerAddresses().size() != LocalUser::getInstance()->peerListRequestIndex) {
						
						char str[INET_ADDRSTRLEN];
						inet_ntop(AF_INET, &(LocalUser::getInstance()->getPeerAddresses()[LocalUser::getInstance()->peerListRequestIndex].get()->sin_addr), str, INET_ADDRSTRLEN);
						std::string  port = std::to_string(LocalUser::getInstance()->getPeerAddresses()[LocalUser::getInstance()->peerListRequestIndex].get()->sin_port);
						std::string string(str);
						SOCKET newSocket = createClientSocket(string, port);
						++LocalUser::getInstance()->peerListRequestIndex;

						if (newSocket != SOCKET_ERROR)
						{
							Peer* peer = new Peer(newSocket);
							peers.push_back(std::unique_ptr<Peer>(peer));
							std::shared_ptr<PacketPeerPort> packetPeerPort(new PacketPeerPort);
							packetPeerPort->port = serverPort;
							sockEvent = WSACreateEvent();
							WSAEventSelect(newSocket, sockEvent, FD_READ | FD_CLOSE);
							eventHandles.push_back(sockEvent);
							std::shared_ptr<PacketPeersRequest> packetPeersRequest(new PacketPeersRequest);
							if(peers.size() < LocalUser::getInstance()->getMinimumPeersCount()) peer->send(packetPeersRequest);
							peer->send(packetPeerPort);
						}
					}
				}
			}
			if (!peer->isConnected())
			{
				std::cout << "Peer " << *peer << " disconnected\n";
				eventHandles.erase(eventHandles.begin() + event);
				peers.erase(peers.begin() + (event - 3));
				continue;
			}
			WSANETWORKEVENTS networkEvents;
			WSAEnumNetworkEvents(peer->getSocket(), eventHandles[event], &networkEvents);
		}
	}

	delete dir;

	downloaders.clear();
	uploaders.clear();
	peers.clear();
	closesocket(serverSocket);

	WSACleanup();

	return 0;
}
