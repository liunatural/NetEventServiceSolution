//**************************************************************************
//  File......... : ChannelManager.
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-10 
//  Description.. : Header file for class ChannelManager used as channelID management 
//
//  History...... : First created by Liu Zhi 2018-10
//
//**************************************************************************

#pragma once
#include <queue>
#include <mutex>

class ChannelManager
{
public:
	ChannelManager();
	~ChannelManager();

	void Init(int start, int size);
	int GetFreeID();
	void ReleaseID(int id);
	int TotalIDs();
	int TotalFreeIDs();

private:
	std::queue<int> m_ids;
	int m_size;
	std::mutex m_cid_mtx;

};

