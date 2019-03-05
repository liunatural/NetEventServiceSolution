#include "Util.h"

bool CopyData(char* dest, char* source, int len, int max_len)
{

	if (dest == NULL || source == NULL || len <= 0 || max_len <= 0)
	{
		return false;
	}

	if (len > max_len)
	{
		len = max_len;
	}

	memset(dest, 0, max_len);

	if (len > 0)
	{
		memcpy(dest, source, len);
	}

	return true;
}


Output_Log::Output_Log(LOG_TYPE logType, string logStr)
{
	this->m_logStr = logStr;
	this->m_logType = logType;
}
