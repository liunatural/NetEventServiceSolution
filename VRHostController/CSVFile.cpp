//**************************************************************************
//  File......... : CSVFile.cpp
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-11 
//  Description.. : Implementation file of the class CSVFile used as read/write .csv file and create
//							user_seat map data.
//  History...... : First created by Liu Zhi 2018-11
//								update by Liu Zhi 2019-02
//***************************************************************************

#include "CSVFile.h"
#include <windows.h>
#include <time.h>
#include "protocol.h"

CSVFile::CSVFile()
{
}

CSVFile::~CSVFile()
{
	m_UserSeatMapFile.close();
}

int CSVFile::ReadUserSeatMap()
{
	string lineStr;

	if (!m_UserSeatMapFile.is_open())
	{
		return -1;
	}

	int seatNumber;
	char userID[USER_ID_LENGTH + 1] = { 0 };
	char ip[IP_ADDR_LENGTH+ 1] = { 0 };
	char timeStr[9] = { 0 };
	while (getline(m_UserSeatMapFile, lineStr))
	{
		//stringstream ss(lineStr);
		//string str;
		//vector<string> lineArray;

		//// ���ն��ŷָ�
		//while (getline(ss, str, ','))
		//	lineArray.push_back(str);
		
		sscanf(lineStr.c_str(), "%d,%[^,],%[^,],%s", &seatNumber, userID, ip, timeStr);


		User_Seat_Map::iterator it;

		it = m_UserSeatMap.find(seatNumber);

		if (it != m_UserSeatMap.end())
		{
			it->second = userID;
		}
		else
		{
			pair<map<int, string>::iterator, bool> insert_Pair;
			insert_Pair = m_UserSeatMap.insert(make_pair(seatNumber, userID));

			if (insert_Pair.second != true)
			{
				return -1;
			}

		}
	}

	return 0;

}

int CSVFile::OpenFile()
{

	char modulePath[MAX_PATH_LEN] = { 0 };
	GetModuleFileNameA(NULL, modulePath, MAX_PATH_LEN);
	(strrchr(modulePath, '\\'))[0] = 0;

	char dateStr[11] = { 0 };
	GetCurrentSystemDate(dateStr);

	sprintf(m_csvFilePath, "%s\\user_seat_map_%s.csv", modulePath, dateStr);

	m_UserSeatMapFile.open(m_csvFilePath, ios::in | ios::out | ios::app);
	//m_UserSeatMapFile << "SeatNumber" << ',' << "UserID" << endl;

	return 0;

}

int CSVFile::Write(int seatNumber, char *userID, char *ip)
{
	if (!m_UserSeatMapFile.is_open())
	{
		return -1;
	}

	char timeStr[9] = { 0 };
	GetCurrentSystemTime(timeStr);

	m_UserSeatMapFile.clear();
	m_UserSeatMapFile.seekp(0, ios::end);
	m_UserSeatMapFile << seatNumber  << ',' << userID << ',' << ip << ',' << timeStr << endl;
	m_UserSeatMapFile.flush();
	return 0;
}

void CSVFile::GetCurrentSystemDate(char* dateStr)
{
	struct tm* p_tm;
	time_t timep;

	time(&timep);
	p_tm = localtime(&timep);

	sprintf(dateStr, "%d-%02d-%02d", (int)p_tm->tm_year + 1900, (int)p_tm->tm_mon + 1, (int)p_tm->tm_mday);
}



void CSVFile::GetCurrentSystemTime(char* timeStr)
{
	struct tm* p_tm;
	time_t timep;

	time(&timep);
	p_tm = localtime(&timep);

	sprintf(timeStr, "%02d:%02d:%02d", p_tm->tm_hour , p_tm->tm_min, p_tm->tm_sec);
}
