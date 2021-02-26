#include "pch.h"
#include "LocalUser.h"

std::shared_ptr<LocalUser> LocalUser::instance = 0;

std::shared_ptr<LocalUser> LocalUser::getInstance()
{
	if (!instance) {
		instance.reset(new LocalUser());
	}
	return instance;
}

std::vector<std::unique_ptr<SOCKADDR_IN>>& LocalUser::getPeerAddresses()
{
	return peerAddresses;
}

bool LocalUser::addPeerAddress(SOCKADDR_IN* newPeerAddress)
{
	for (int i = 0; i < peerAddresses.size(); ++i) {
		if ((newPeerAddress->sin_addr.S_un.S_addr == peerAddresses[i].get()->sin_addr.S_un.S_addr) && (newPeerAddress->sin_port == peerAddresses[i].get()->sin_port)) return false;
	}
	SOCKADDR_IN* peerAddress = new SOCKADDR_IN(*newPeerAddress);
	peerAddresses.push_back(std::unique_ptr<SOCKADDR_IN>(peerAddress));
	return true;
}

void LocalUser::setMinimumPeersCount(int maximumPeersCount)
{
	this->minimumPeersCount = maximumPeersCount;
}

int LocalUser::getMinimumPeersCount()
{
	return minimumPeersCount;
}

LocalUser::LocalUser() :
	minimumPeersCount(0)
{}
