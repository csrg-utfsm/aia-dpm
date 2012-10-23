#ifndef __TEMPERATURE_HPP__
#define __TEMPERATURE_HPP__

#include "HistoricRecord.hpp"

namespace asch_input{

class Temperature : public HistoricRecord {

public:
	Temperature( double time, double value, double rms, double slope) : HistoricRecord( time, value, rms, slope ){
	}
};
}

#endif
