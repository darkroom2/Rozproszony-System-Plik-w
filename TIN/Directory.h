#pragma once
#include "Packet.h"
class Directory
{
private:
	// variables
	const char* path;
	HANDLE hDir;
	FILE_NOTIFY_EXTENDED_INFORMATION buff[1024];
	OVERLAPPED overlapped;
	// local file list (TODO: always updated, ready to be sent)
	std::vector<File> files;

	// initializations
	void createDirHandle();

	// priv functions
	bool listDirectoryContents(const char* sDir);
public:
	// publ functions
	int addEventHandle(std::vector<HANDLE>& eventHandles);
	bool checkForDirChanges();
	void updateFileList();

	// getters
	const std::string getFileName();
	LARGE_INTEGER getModifiedTime();
	const std::vector<File>& getFiles();
	const DWORD getAction();

	// ctors
	Directory(const char* path);
	virtual ~Directory();
};

