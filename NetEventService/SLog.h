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

	// 在使用之前必须先调用此函数  
	static void Init(const char* dir);
	static void Log(int slv, const char* format);

private:
	static boost::log::sources::severity_logger<boost::log::trivial::severity_level > s_slg;

};


