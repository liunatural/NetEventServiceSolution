#include "SLog.h"
#include <iostream>  
#include <string>  

#include <boost/filesystem.hpp>  
#include <boost/log/sources/logger.hpp>  
#include <boost/log/sources/record_ostream.hpp>  
#include <boost/log/sources/global_logger_storage.hpp>  
#include <boost/log/utility/setup/file.hpp>  
#include <boost/log/utility/setup/common_attributes.hpp>  
#include <boost/log/sinks/text_ostream_backend.hpp>  
#include <boost/log/attributes/named_scope.hpp>  
#include <boost/log/expressions.hpp>  
#include <boost/log/support/date_time.hpp>  
#include <boost/log/detail/format.hpp>  
#include <boost/log/detail/thread_id.hpp>  
#include <boost/log/utility/setup/console.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;

boost::log::sources::severity_logger<boost::log::trivial::severity_level > SLog::s_slg;


//****************************************************************************************
//  File......... : SLog.cpp
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-09 
//  Description.. : Implementation file of the class SLog used as Logger service.
//
//  History...... : First created by Liu Zhi 2018-10
//
//****************************************************************************************
SLog::SLog()
{
}

SLog::~SLog()
{
}

void SLog::Init(const char* dir)
{
	if (boost::filesystem::exists(dir) == false)
	{
		boost::filesystem::create_directories(dir);
	}

	string log_dir = dir;


	auto asink = logging::add_console_log
	(std::clog,
		keywords::format =
		(
			expr::stream
			//<< "[" << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
			<< "[" << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
			//<< " " << expr::attr< boost::log::aux::thread::id >("ThreadID")
			//<< " " << expr::attr<unsigned int>("LineID")
			<< " " << logging::trivial::severity
			<< "] " << expr::smessage
			)
	);

	logging::core::get()->add_sink(asink);

	auto pSink = logging::add_file_log
	(
		keywords::open_mode = std::ios::app,
		keywords::file_name = log_dir + "/%Y-%m-%d.log",
		keywords::rotation_size = 10 * 1024 * 1024,
		keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
		//keywords::format = "[%TimeStamp% %ThreadID%]: %Message%"  
		keywords::format =
		(
			expr::stream
			//<< "[" << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
			<< "[" << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
			//<< " " << expr::attr< boost::log::aux::thread::id >("ThreadID")
			//<< " " << expr::attr<unsigned int>("LineID")
			<< " " << logging::trivial::severity
			<< "] " << expr::smessage
		)
	);

	pSink->locked_backend()->auto_flush(true);//使日志实时更新  
	//pSink->imbue(std::locale("zh_CN.UTF-8")); // 本地化   
	logging::add_common_attributes();
}

void SLog::Log(int slv, const char* format)
{

	boost::log::trivial::severity_level e_slv = boost::log::trivial::severity_level(slv);

	BOOST_LOG_FUNCTION(); 
	BOOST_LOG_SEV((SLog::s_slg), (e_slv)) << format;
}

