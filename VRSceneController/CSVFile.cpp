#include "CSVFile.h"
#include <windows.h>
#include <time.h>

CSVFile::CSVFile()
{
}


CSVFile::~CSVFile()
{
	m_UserSeatMapFile.close();
}

int CSVFile::CreateUserSeatMap()
{
	string lineStr;

	if (!m_UserSeatMapFile.is_open())
	{
		return -1;
	}

	int seatNumber;
	char userID[32] = { 0 };

	while (getline(m_UserSeatMapFile, lineStr))
	{
		//stringstream ss(lineStr);
		//string str;
		//vector<string> lineArray;

		//// ∞¥’’∂∫∫≈∑÷∏Ù
		//while (getline(ss, str, ','))
		//	lineArray.push_back(str);
		
		sscanf(lineStr.c_str(), "%d,%s", &seatNumber, userID);


		pair<map<int, string>::iterator, bool> insert_Pair;
		insert_Pair = m_UserSeatMap.insert(make_pair(seatNumber, userID));

		if (insert_Pair.second != true)
		{
			return -1;
		}


	}

	return 0;

}

int CSVFile::OpenFile()
{

	char modulePath[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, modulePath, MAX_PATH);
	(strrchr(modulePath, '\\'))[0] = 0;

	char dateStr[11] = { 0 };
	GetCurrentSystemTime(dateStr);

	sprintf(m_csvFilePath, "%s\\user_seat_map_%s.csv", modulePath, dateStr);

	m_UserSeatMapFile.open(m_csvFilePath, ios::in | ios::out | ios::app);
	//m_UserSeatMapFile << "SeatNumber" << ',' << "UserID" << endl;

	return 0;

}

int CSVFile::Write(int seatNumber, char* userID)
{
	if (!m_UserSeatMapFile.is_open())
	{
		return -1;
	}
	
	m_UserSeatMapFile.clear();
	m_UserSeatMapFile.seekp(0, ios::end);
	m_UserSeatMapFile << seatNumber  << ',' << userID  << endl;
	m_UserSeatMapFile.flush();
	return 0;
}

void CSVFile::GetCurrentSystemTime(char* dateStr)
{
	struct tm* p_tm;
	time_t timep;

	time(&timep);
	p_tm = localtime(&timep);

	sprintf(dateStr, "%d-%02d-%02d", (int)p_tm->tm_year + 1900, (int)p_tm->tm_mon + 1, (int)p_tm->tm_mday);
}
