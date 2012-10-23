#ifndef __DATETIME_HPP__
#define __DATETIME_HPP__

#include <string>
#include <ctime>

namespace asch_input{

class DateTime{
private:
	int year;
	int month;
	int day;
	int hours;
	int minutes;
	double seconds;

public:
	DateTime (unsigned int y, int mo, int d, int h, int mi, double s):
	year(y),
	month(mo),
	day(d),
	hours(h),
	minutes(mi),
	seconds(s){};

	int getYear(){return this->year;}
	int getMonth(){return this->month;}
	int getDay(){return this->day;}
	int getHours(){return this->hours;}
	int getMinutes(){return this->minutes;}
	double getSeconds (){return this->seconds;}

	bool leq(DateTime aft)
	{
		if(aft.getYear() < this->year) return false;
		if(aft.getYear() > this->year) return true;
		
		if(aft.getMonth() < this->month) return false;
		if(aft.getMonth() > this->month) return true;
		
		if(aft.getDay() < this->day) return false;
		if(aft.getDay() > this->day) return true;
		
		if(aft.getHours() < this->hours) return false;
		if(aft.getHours() > this->hours) return true;
		
		if(aft.getMinutes() < this->minutes) return false;
		if(aft.getMinutes() > this->minutes) return true;
		
		if(aft.getSeconds() < this->seconds) return false;
		if(aft.getSeconds() > this->seconds) return true;
		
		return true;
	}
	
	bool between(DateTime bef, DateTime aft)
	{
		if(this->leq(aft) && bef.leq(*this))
		{
			return true;
		}
		return false;
	}
	
	struct tm getTm()
	{
		struct tm out_tm;

		time_t rawtime;
		time ( &rawtime );
		out_tm = *localtime ( &rawtime );
		out_tm.tm_year = this->year - 1900;
		out_tm.tm_mon = this->month - 1;
		out_tm.tm_mday = this->day;
		out_tm.tm_hour = this->hours;
		out_tm.tm_min = this->minutes;
		out_tm.tm_sec = (int) (this->seconds);
		mktime(&out_tm);
		return out_tm;
	}
};

}


#endif
