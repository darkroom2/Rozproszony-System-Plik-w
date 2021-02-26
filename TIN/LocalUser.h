#pragma once
#include "Peer.h"

class LocalUser
{
public:
	static std::shared_ptr<LocalUser> getInstance();
	std::vector<std::unique_ptr<SOCKADDR_IN>>& getPeerAddresses();
	bool addPeerAddress(SOCKADDR_IN* newPeerAddress);
	void setMinimumPeersCount(int maximumPeersCount);
	int getMinimumPeersCount();
	int peerListRequestIndex = 0;
	std::string path;

private:
	int minimumPeersCount;
	static std::shared_ptr<LocalUser> instance;
	std::vector<std::unique_ptr<SOCKADDR_IN>> peerAddresses;

	LocalUser();
};



