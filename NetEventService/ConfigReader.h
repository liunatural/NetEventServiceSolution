//**************************************************************************
//
//  File......... : ConfigReader.h
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-10 
//  Description.. : head file of the class ConfigReader used as XML configuration file reader API
//
//  History...... : first created by Liu Zhi 2018-10
//
//**************************************************************************
#pragma once
#include "NetEventService.h"
#include <boost/property_tree/xml_parser.hpp>

using namespace boost::property_tree;

class ConfigReader : public ConfigService
{
public:
	ConfigReader();
	virtual ~ConfigReader();

	bool	OpenFile(const char* file);
	bool	GetStr(const char* path, char* rel);
	int		GetInt(const char* path);
	double	GetDouble(const char* path);

private:
	ptree  mXmlRoot;
};
