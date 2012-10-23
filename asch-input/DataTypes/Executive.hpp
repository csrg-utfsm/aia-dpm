#ifndef __EXECUTIVE_HPP__
#define __EXECUTIVE_HPP__

#include <string>
#include "DateTime.hpp"
#include "Optional.hpp"

namespace asch_input{

class ObsSeason{
	
	std::string name;
	DateTime startDate;
	DateTime endDate;
	
	public:
	ObsSeason(std::string n,DateTime s,DateTime e):name(n),startDate(s),endDate(e){}
	
	std::string getName(){return this->name;}
	DateTime getStartDate(){return this->startDate;}
	DateTime getEndDate(){return this->endDate;}
	
};


class Executive{
	private:
	std::string name;
	float defaultPercentage;

	float percentage;
	
	Optional<float> totalObsTimeForSeason;
	
	
	public:
	Executive(std::string n,float dp,float per,Optional<float> totfs):
		name(n),
		defaultPercentage(dp),
		percentage(per),
		totalObsTimeForSeason(totfs)
		{}
	
	std::string getName(){return this->name;}
	float getDefaultPercentage(){return this->defaultPercentage;}

	float getPercentage(){return this->percentage;}
	Optional<float> getTotalObsTimeForSeason(){return this->totalObsTimeForSeason;}
};
}
#endif
