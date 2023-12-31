#include "timeutils.h"
#include "time.h"

#define DEBUG
#include "SerialDebug.h"

#define NTP_SERVER "pool.ntp.org"
#define TIMEZONE_OFFSET 7200 // GMT+2 (2 hours in seconds)
#define DAY_LIGHT_SAVINGS 0	 // 1 Hour Estonia DST (Day Light Savings)

void PrintTime(struct tm timeinfo) // For debugging
{
	int year = timeinfo.tm_year + 1900;
	int month = timeinfo.tm_mon + 1; // Month is 0-based, so add 1 to get the real month (1-12)
	int day = timeinfo.tm_mday;		 // Day of the month
	int hour = timeinfo.tm_hour;
	int minute = timeinfo.tm_min;
	DBG(day);
	DBG("/");
	DBG(month);
	DBG("/");
	DBG(year);
	DBG(" ");
	DBG(hour);
	DBG(":");
	DBG(minute);
	DBGL(" ");
}

struct tm GetEpochTime() // Querys current time from NTP server
{
	time_t now;
	struct tm timeinfo;
	if (!getLocalTime(&timeinfo))
	{
		DBGL("Failed to obtain time");
		return timeinfo;
	}
	time(&now);
	return timeinfo;
}

struct tm ConvertFromTimeStamp(unsigned long long tstamp) // Converts timestamp E.g: 169593923 into struct tm
{
	time_t now = static_cast<time_t>(tstamp);
	struct tm timeinfo;

	if (!getLocalTime(&timeinfo))
	{
		DBGL("Failed to obtain time");
		return timeinfo;
	}
	time(&now);
	return timeinfo;
}

unsigned long long ConvertToTimeStamp(struct tm timeinfo) // Converts struct tm into timestamp 169593923 
{
	time_t timestamp = mktime(&timeinfo);

	if (timestamp != -1)
	{
		return static_cast<unsigned long long>(timestamp) * 1000; // * 1000 for timestamp in ms format
	}
	else
	{
		// Handle the error case, e.g., by returning an error code
		DBGL("ERROR: Convert to time stamp failed");
		return 0;
	}
}

void ConfigTime()
{
	configTime(TIMEZONE_OFFSET, DAY_LIGHT_SAVINGS, NTP_SERVER);
}

// Autumn semester: ~ 04.09 - 21.01 | months: 7, 8, 9, 10, 11, 12, 1
// Spring semester: ~ 29.01 - 12.6  | months: 1, 2, 3, 4, 5, 6

struct tm GetSemesterEnd() // Returns struct tm of semester end
{ 

	struct tm current_time = GetEpochTime();

	current_time.tm_hour = 23;
	current_time.tm_min = 59;
	current_time.tm_sec = 0;

	if (current_time.tm_mon > 5 && current_time.tm_mon <= 11)
	{
		// Autumn semester or summer month
		current_time.tm_mday = 21;
		current_time.tm_mon = 0; // Ends in autumn
		current_time.tm_year += 1;
	}
	else if (current_time.tm_mon >= 0 && current_time.tm_mon <= 5)
	{
		// Spring semester
		if ((current_time.tm_mon == 5 && current_time.tm_mday >= 12) || // Juuni 12 - ...
			(current_time.tm_mon == 0 && current_time.tm_mday < 21))	// January 1-20 -> sygissemester
		{ 
			if (current_time.tm_mon == 5)
			{
				current_time.tm_year += 1;
			}
			current_time.tm_mday = 21;
			current_time.tm_mon = 0; // Ends in autumn
		}
		else
		{
			current_time.tm_mday = 12;
			current_time.tm_mon = 5; // Ends in spring
		}
	}
	PrintTime(current_time);
	DBG("TSTAMP: ");
	DBGL(ConvertToTimeStamp(current_time));
	return current_time;
}