#pragma once
#include <queue>

class ChannelIDGenerator
{
public:
	ChannelIDGenerator();
	~ChannelIDGenerator();

	void init(int start, int size);
	int getId();
	void freeId(int id);
	int getSize();
	int getFreeId();

private:
	std::queue<int> m_ids;
	int m_size;


};

