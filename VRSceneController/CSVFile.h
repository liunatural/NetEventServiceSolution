//**************************************************************************
//  File......... : CSVFile.h
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-11 
//  Description.. : Head file of the CSVFile class used as read/write .csv file and create
//						user_seat map data.
//  History...... : First created by Liu Zhi 2018-11
//
//***************************************************************************
#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>

using namespace std;

typedef map<int, string>  User_Seat_Map;

class CSVFile
{
public:
	CSVFile();
	~CSVFile();

	int OpenFile();

	int CreateUserSeatMap();
	int Write(int seatNumber, char* userID);
	User_Seat_Map& GetUserSeatMap() { return m_UserSeatMap; }

private:
	void GetCurrentSystemTime(char* dateStr);

private:
	char m_csvFilePath[260] = { 0 };
	fstream m_UserSeatMapFile;

	User_Seat_Map  m_UserSeatMap;
};

