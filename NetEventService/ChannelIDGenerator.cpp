#include "ChannelIDGenerator.h"



ChannelIDGenerator::ChannelIDGenerator()
{
	m_size = 0;
}


ChannelIDGenerator::~ChannelIDGenerator()
{
}

void ChannelIDGenerator::init(int start, int size)
{

	m_size = size;
	for (int i = start; i < start + m_size; i++)
	{
		m_ids.push(i);
	}

}

int ChannelIDGenerator::getId()
{
	if (m_ids.empty())
		return -1;

	int id = m_ids.front();
	m_ids.pop();
	return id;
}

void ChannelIDGenerator::freeId(int id)
{
	m_ids.push(id);
}

int ChannelIDGenerator::getSize()
{
	return m_size;
}

int ChannelIDGenerator::getFreeId()
{
	return (int)m_ids.size();
}

