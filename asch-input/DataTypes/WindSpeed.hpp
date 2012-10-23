#ifndef __WINDSPEED_HPP__
#define __WINDSPEED_HPP__

#include "HistoricRecord.hpp"


namespace asch_input{

class WindSpeed : public HistoricRecord {
public:
	WindSpeed( double time, double value, double rms, double slope ): HistoricRecord( time, value, rms, slope){
	};
};
}

#endif
