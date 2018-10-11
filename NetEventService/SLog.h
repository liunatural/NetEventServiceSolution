//****************************************************************************************
//  File......... : SLog.h
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-09 
//  Description.. : Head file of the class SLog used as Logger service.
//
//  History...... : First created by Liu Zhi 2018-10
//
//****************************************************************************************

#pragma once  

#include <string>  
#include <boost/log/trivial.hpp>  
#include "NetEventService.h"

using namespace std;

class SLog 
{
public:
	SLog();
	~SLog(void);

	// ��ʹ��֮ǰ�����ȵ��ô˺���  
	static void Init(const char* dir);
	static void Log(int slv, const char* format);

private:
	static boost::log::sources::severity_logger<boost::log::trivial::severity_level > s_slg;

};


