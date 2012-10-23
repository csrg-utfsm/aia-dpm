#ifndef __HUMIDITY_HPP__
#define __HUMIDITY_HPP__

#include "HistoricRecord.hpp"

namespace asch_input{

class Humidity : public HistoricRecord {

public:
	Humidity( double time, double value, double rms, double slope) : HistoricRecord( time, value, rms, slope ){
	}
};
}

#endif
