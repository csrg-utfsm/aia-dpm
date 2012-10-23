#include "classes.hpp"

using namespace std;

WeatherData::WeatherData (short type, time_t time, double value, double rms, double slope):
	type (type),
	time (time),
	value (value),
	rms (rms),
	slope (slope)	
{
	//Nothing to do here
}

WeatherData::~WeatherData()
{
	//Nothing to do here
}

/*
short WeatherData::getType(){
	return type;
}

time_t WeatherData::getTime(){ 
	return time; 
}

double WeatherData::getValue(){ 
	return value; 
}

double WeatherData::getRms(){
	return rms;
}
double WeatherData::getSlope(){
	return slope;
}*/

