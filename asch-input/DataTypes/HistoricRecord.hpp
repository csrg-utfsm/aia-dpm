#ifndef __HISTORICRECORD_HPP__
#define __HISTORICRECORD_HPP__

namespace asch_input{

class HistoricRecord{
private:
	double time;
	double value;
	double rms;
	double slope;
public:
	HistoricRecord( double time, double value, double rms, double slope ){
		this->time = time;
		this->value = value;
		this->rms = rms;
		this->slope = slope;
	};
	double getTime(){ return time; };
	double getValue(){ return value; };
	double getRms(){return rms;};
	double getSlope(){return slope;};
};
}

#endif
