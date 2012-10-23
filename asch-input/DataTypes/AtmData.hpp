#ifndef __ATMDATA_HPP__
#define __ATMDATA_HPP__

namespace asch_input{

class AtmData {

  public:
	AtmData (double pwv, double frequency, double opacity, double temperature) {
		this->pwv = pwv;
		this->frequency = frequency;
		this->opacity = opacity;
		this->temperature = temperature;
	};
	double getPwv() { return pwv; };
	double getFrequency() { return frequency; };
	double getOpacity() { return opacity; };
	double getTemperature() { return temperature; };

  private:
	double pwv;
	double frequency;
	double opacity;
	double temperature;
};
}

#endif
