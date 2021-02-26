#include "pch.h"
#include "Directory.h"


void Directory::createDirHandle()
{
	this->hDir = CreateFileA(
		this->path,
		FILE_LIST_DIRECTORY,
		FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL
	);
}

bool Directory::listDirectoryContents(const char* sDir)
{
	WIN32_FIND_DATAA fdFile;
	HANDLE hFind = NULL;

	char sPath[2048];

	//Specify a file mask. *.* = We want everything!
	sprintf_s(sPath, "%s\\*.*", sDir);

	if ((hFind = FindFirstFileA(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
	{
		printf("Path not found: [%s]\n", sDir);
		return false;
	}

	do
	{
		//Find first file will always return "."
		//    and ".." as the first two directories.
		if (strcmp(fdFile.cFileName, ".") != 0
			&& strcmp(fdFile.cFileName, "..") != 0)
		{
			//Build up our file path using the passed in
			//  [sDir] and the file/foldername we just found:
			sprintf_s(sPath, "%s", fdFile.cFileName);

			//Is the entity a File or Folder?
			if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				listDirectoryContents(sPath); //Recursion, I love it!
			}
			else {
				printf("File: %s\n", sPath);
				File file;
				file.path = std::string(sPath);
				file.modifiedTime.LowPart = fdFile.ftLastWriteTime.dwLowDateTime;
				file.modifiedTime.HighPart = fdFile.ftLastWriteTime.dwHighDateTime;
				files.push_back(file);
			}
		}
	} while (FindNextFileA(hFind, &fdFile)); //Find the next file.

	FindClose(hFind); //Always, Always, clean things up!

	return true;
}

int Directory::addEventHandle(std::vector<HANDLE>& eventHandles)
{
	eventHandles.push_back(CreateEvent(
		NULL,    // default security attribute 
		TRUE,    // manual-reset event 
		FALSE,    // initial state = signaled 
		NULL)
	);
	if (eventHandles.back() == NULL)
	{
		std::cout << "\nERROR: CreateEvent function failed.\n" << std::endl;
		std::cout << GetLastError() << std::endl;
		return -1;
	}
	this->overlapped.hEvent = eventHandles.back();
	return 0;
}

bool Directory::checkForDirChanges()
{
	// TODO: in the bufer there could be multiple 'changes', handle them
	// TODO: deal with doubling the notification (known problem)
	memset(this->buff, 0, sizeof(this->buff)); // TODO: should not be zeroed as there are previous changes
	if (ReadDirectoryChangesExW(
		this->hDir,
		(LPVOID)& this->buff,
		sizeof(this->buff),
		TRUE,
		FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_SIZE,
		NULL,
		&this->overlapped,
		NULL,
		ReadDirectoryNotifyExtendedInformation) == 0)
	{
		std::cout << "\nERROR: ReadDirectoryChangesW function failed.\n" << std::endl;
		std::cout << GetLastError() << std::endl;
		return true;
	}
	return false;
}

void Directory::updateFileList() {
	auto act = buff[0].Action;
	if (act == FILE_ACTION_ADDED) {
		File file;
		file.path = this->getFileName();
		file.modifiedTime = this->getModifiedTime();
		this->files.push_back(file);
	}
	else if (act == FILE_ACTION_REMOVED) {
		// for now remove and list again, later add finding element or change vector to smth else 
		this->files.clear();
		this->listDirectoryContents(this->path);
	}
	else if (act == FILE_ACTION_MODIFIED or act == FILE_ACTION_RENAMED_OLD_NAME or act == FILE_ACTION_RENAMED_NEW_NAME) {
		this->files.clear();
		this->listDirectoryContents(this->path);
	}
	else {
		std::cout << "Nieprzewidziane zachowanie Directory::updateFileList()" << std::endl;
	}
}

std::string utf8_encode(const std::wstring& wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed/2, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed/2, NULL, NULL);
	return strTo;
}

const std::string Directory::getFileName() {
	// TODO: error checking (if buff not empty...) etc
	std::wstring wstr(this->buff[0].FileName, this->buff->FileNameLength);
	return utf8_encode(wstr);
}

LARGE_INTEGER Directory::getModifiedTime() {
	// TODO: error checking (if buff not empty...) etc
	return buff[0].LastModificationTime;
}

const std::vector<File>& Directory::getFiles() {
	//listDirectoryContents(this->path);
	return files;
}

const DWORD Directory::getAction()
{
	return buff[0].Action;
}

Directory::Directory(const char* path) :
	path(path), hDir(NULL), buff{ 0 }, overlapped({ 0 })
{
	createDirHandle();
	listDirectoryContents(this->path); // this updates the vector<File> files
}


Directory::~Directory()
{
}
