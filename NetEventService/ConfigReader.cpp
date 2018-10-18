//**************************************************************************
//
//  File......... : ConfigReader.cpp
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-10 
//  Description.. : implementation file of the class ConfigReader used as XML configuration file reader API
//
//  History...... : first created by Liu Zhi 2018-10
//
//**************************************************************************

#include "ConfigReader.h"
#include <boost/program_options/detail/utf8_codecvt_facet.hpp>
//std::locale utf8Locale(std::locale(), new boost::program_options::detail::utf8_codecvt_facet());

//std::locale utf8Locale(std::locale(), new std::codecvt_utf8);

using namespace std;

ConfigReader::ConfigReader()
{
}

ConfigReader::~ConfigReader()
{
}

bool ConfigReader::OpenFile(const char* file)
{
	bool ret = false;

	try
	{
		std::ifstream fs(file);
		//fs.imbue(utf8Locale);

		if (fs)
		{
			boost::property_tree::read_xml(fs, mXmlRoot);
			ret = true;
		}
	}
	catch (...)
	{
		ret =  false;
	}

	return ret;
}

bool  ConfigReader::GetStr(const char* path, char* rel)
{

	string val =   mXmlRoot.get<string>(path);
	strcpy(rel, val.c_str());

	return true;

}

int  ConfigReader::GetInt(const char* path)
{
	return mXmlRoot.get(path, 0);
}

double  ConfigReader::GetDouble(const char* path)
{
	return mXmlRoot.get(path, 0.0);
}


