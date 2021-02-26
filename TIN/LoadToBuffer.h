#pragma once
class LoadToBuffer
{
public:
	int i;
	int begin;
	int receivedBytes;
	int end;
	std::shared_ptr<std::vector<char>> *buf;
	LoadToBuffer() {
		i = 0;
		begin = 0;
		receivedBytes = 0;
		end = 0;
	}
	~LoadToBuffer() {}
	template<class T>
	void load(T *thing)
	{
		int size = sizeof *thing;
		begin = end;
		end += size;

		while (i < (*buf)->size() && receivedBytes >= begin && receivedBytes < end)
		{
			((char*)(thing))[receivedBytes - begin] = (**buf)[i];
			++receivedBytes;
			++i;
		}
	}
	void load(std::string * thing, int size)
	{
		begin = end;
		end += size;


		while (i < (*buf)->size() && receivedBytes >= begin && receivedBytes < end)
		{
			*thing += (**buf)[i];
			++receivedBytes;
			++i;
		}
	}
	void load(std::vector<char> * thing, int size)
	{
		begin = end;
		end += size;


		while (i < (*buf)->size() && receivedBytes >= begin && receivedBytes < end)
		{
			thing->push_back((**buf)[i]);
			++receivedBytes;
			++i;
		}
	}
	void load(SOCKADDR_IN * address)
	{
		begin = end;
		end += 6;

		while (i < (*buf)->size() && receivedBytes >= begin && receivedBytes < end)
		{
			if ((receivedBytes - 4) % 6 < 4)((char*)(&((*address).sin_addr)))[(receivedBytes - 4) % 6] = (**buf)[i];
			else if ((receivedBytes - 4) % 6 <= 5)((char*)(&((*address).sin_port)))[((receivedBytes - 4) % 6) - 4] = (**buf)[i];

			++receivedBytes;
			++i;
		}
	}
	void  setBuffer(std::shared_ptr<std::vector<char>> *_buf) {
		buf = _buf;
		end = 0;
		i = 0;
	}
	bool isReceived()
	{
		return receivedBytes == end;
	}
};