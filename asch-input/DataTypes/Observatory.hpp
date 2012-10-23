#ifndef __OBSERVATORY_HPP__
#define __OBSERVATORY_HPP__

#include "Optional.hpp"

namespace asch_input{

class ArrayConfiguration{
	private:
	
	Optional<DateTime> startTime;
	Optional<DateTime> endTime;
	Optional<double> resolution;
	Optional<double> uvCoverage;
	
	public:
	
	ArrayConfiguration(Optional<DateTime> s,Optional<DateTime> e,Optional<double> r,Optional<double> uv):startTime(s),endTime(e),resolution(r),uvCoverage(uv){}
	Optional<DateTime> getStartTime(){return this->startTime;}
	Optional<DateTime> getEndTime(){return this->endTime;}
	Optional<double> getResolution(){return this->resolution;}
	Optional<double> getUvCoverage(){return this->uvCoverage;}
	
	
};
}

#endif
