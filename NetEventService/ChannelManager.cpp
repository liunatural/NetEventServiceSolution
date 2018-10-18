//**************************************************************************
//  File......... : ChannelManager.cpp
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-10 
//  Description.. : implementation file for class ChannelManager used as channelID management 
//
//  History...... : First created by Liu Zhi 2018-10
//
//**************************************************************************


#include "ChannelManager.h"

ChannelManager::ChannelManager()
{
	m_size = 0;
}


ChannelManager::~ChannelManager()
{
}

void ChannelManager::Init(int start, int size)
{

	m_size = size;
	for (int i = start; i < start + m_size; i++)
	{
		m_ids.push(i);
	}

}

int ChannelManager::GetFreeID()
{
	std::lock_guard<std::mutex> lock(m_cid_mtx);
	if (m_ids.empty())
		return -1;

	int id = m_ids.front();
	m_ids.pop();
	return id;
}

void ChannelManager::ReleaseID(int id)
{
	std::lock_guard<std::mutex> lock(m_cid_mtx);

	m_ids.push(id);
}

int ChannelManager::TotalIDs()
{
	return m_size;
}

int ChannelManager::TotalFreeIDs()
{
	return (int)m_ids.size();
}

