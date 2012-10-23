#ifndef __OPACITY_HPP__
#define __OPACITY_HPP__

#include "HistoricRecord.hpp"

namespace asch_input{

class Opacity : public HistoricRecord {

public:
	Opacity( double time, double value, double rms, double slope) : HistoricRecord( time, value, rms, slope ){
	}
};
}

#endif
