#include "classes.hpp"

using namespace std;

WeatherHandler::WeatherHandler()
{
	//Nothing to do here
}

WeatherHandler::~WeatherHandler()
{
	while (!temperatureRecords.empty()) {
		WeatherData* oldWd = temperatureRecords.front();
		temperatureRecords.pop_front();
		delete oldWd;
	}
	while (!humidityRecords.empty()) {
		WeatherData* oldWd = humidityRecords.front();
		humidityRecords.pop_front();
		delete oldWd;
	}
}

void WeatherHandler::initialize(string dataSource, time_t periodStartDate, time_t resolution)
{
	this->resolution = resolution;
	int validCount, nullCount;
	time_t time;
	double value, rms, slope;

	asch_input::WeatherReader *weather = new asch_input::WeatherReader(dataSource);
	list<asch_input::Temperature*> * temperature;
	list<asch_input::Humidity*> * humidity;

	weather->loadTemperatureHistory();
	weather->loadHumidityHistory();
	weather->loadAtmData();

	temperature = weather->getTemperatureRecords();
	humidity = weather->getHumidityRecords();
	atmDataRecords = weather->getAtmDataRecords();

	// Create weather parameter records with normalized time value
	// TEMPERATURE
	validCount = 0;
	nullCount = 0;
	value = 0.0;
	rms = 0.0;
	slope = 0.0;
	time = 0L;
	for ( std::list<asch_input::Temperature*>::iterator it = temperature->begin(); it != temperature->end(); it++) {
		time_t t = (*it)->getTime() * 24.0*60.0*60.0 + 0.5; //convert to seconds
//		cout << "Time: " << t << endl;
		if (time == 0L)
			time = t;

//		cout << "Value: " << (*it)->getValue() << endl;
		if ((*it)->getValue() != INVALID_DATA) {
			value += (*it)->getValue();
			rms += (*it)->getRms();
			slope += (*it)->getSlope();
			validCount++;
		} else {
			nullCount++;
		}

		if (t % resolution == 0 && t > 0L) {
//			cout << "Resolution matched." << endl;
			if(validCount == 0) {
//				cout << "No valid values in interval." << endl;
				value = INVALID_DATA;
				rms = INVALID_DATA;
				slope = INVALID_DATA;
			} else {
				value /= validCount;
				rms /= validCount;
				slope /= validCount; // slope fits based on resolution
			}
			time = (time+t)/2; // interval mid time
			time += periodStartDate; // period relative date
//			cout << "Interval date: " << time << endl;
			WeatherData *newT = new WeatherData(TEMPERATURE, time, value, rms, slope);
			temperatureRecords.push_back(newT);

			validCount = 0;
			nullCount = 0;
			value = 0.0;
			rms = 0.0;
			slope = 0.0;
			time = 0L;
		}
	}

	// HUMIDITY
	validCount = 0;
	nullCount = 0;
	value = 0.0;
	rms = 0.0;
	slope = 0.0;
	time = 0L;
	for ( std::list<asch_input::Humidity*>::iterator it = humidity->begin(); it != humidity->end(); it++) {
		time_t t = (*it)->getTime() * 24.0*60.0*60.0 + 0.5; //convert to seconds
		if (time == 0L)
			time = t;

		if ((*it)->getValue() != INVALID_DATA) {
			value += (*it)->getValue();
			rms += (*it)->getRms();
			slope += (*it)->getSlope();
			validCount++;
		} else {
			nullCount++;
		}

		if (t % resolution == 0 && t > 0L) {
			if(validCount == 0) {
				value = INVALID_DATA;
				rms = INVALID_DATA;
				slope = INVALID_DATA;
			}
			else {
				value /= validCount;
				rms /= validCount;
				slope /= validCount; // slope fits based on resolution
			}
			time = (time+t)/2; // interval mid time
			time += periodStartDate; // period relative date
			WeatherData *newH = new WeatherData(HUMIDITY, time, value, rms, slope);
			humidityRecords.push_back(newH);

			validCount = 0;
			nullCount = 0;
			value = 0.0;
			rms = 0.0;
			slope = 0.0;
			time = 0L;
		}
	}
}

weatherCondition WeatherHandler::getWeatherCondition(time_t begin, time_t end)
{
	weatherCondition weather = {begin, end, INVALID_DATA, INVALID_DATA, INVALID_DATA};

	for ( std::list<WeatherData*>::iterator it = temperatureRecords.begin(); it != temperatureRecords.end(); ++it )
		if ((*it)->time >= begin && (*it)->time <= end) {
			weather.temperature = (*it)->value;
			break;
		}
	if (weather.temperature == INVALID_DATA)
		cerr << "WARNING: No temperature available for interval " << begin << "," << end << endl;

	for ( std::list<WeatherData*>::iterator it = humidityRecords.begin(); it != humidityRecords.end(); ++it )
		if ((*it)->time >= begin && (*it)->time <= end) {
			weather.humidity = (*it)->value;
			break;
		}
	if (weather.humidity == INVALID_DATA)
		cerr << "WARNING: No humidity available for interval " << begin << "," << end << endl;

	// Estimate the PWV based on temperature and humidity
	estimatePwv(weather);

	return weather;
}

atmosphericData WeatherHandler::getFrequencyParameters(double frequency, double pwv)
{
	atmosphericData data = {pwv, frequency, INVALID_DATA, INVALID_DATA};
	interpolateOpacityAndTemperature(data);

	return data;
}

/**
 * Return the receiver temperature
 * 
 * @param frequency (GHz)
 * @return receiver temperature in deg. K
 * @author ab
 */
double getReceiverTemperature (double frequency) {
	if (frequency >=31.3 && frequency <= 45.0)
		return 17.0;
	else if (frequency >=67.0 && frequency <= 90.0)
		return 30.0;
	else if (frequency >=84.0 && frequency <= 116.0)
		return 37.0;
	else if (frequency >=125.0 && frequency < 163.0)
		return 51.0;
	else if (frequency >=163.0 && frequency < 211.0)
		return 65.0;
	else if (frequency >=211.0 && frequency < 275.0)
		return 83.0;
	else if (frequency >=275.0 && frequency <= 373.0)
		return 147.0;
	else if (frequency >=385.0 && frequency <= 500.0)
		return 196.0;
	else if (frequency >=602.0 && frequency <= 720.0)
		return 175.0;
	else if (frequency >=787.0 && frequency <= 950.0)
		return 230.0;
	else
		return -1.0;
}

double WeatherHandler::getSystemTemperature(double ra, double dec, atmosphericData& data)
{
	// sinAltitud Calculation
	double latitudeRad = -23.0229 * M_PI/180.0; //Chajnantor latitude
	double decRad = dec*M_PI/180.0;

	double sinDec = sin(decRad);
	double sinLat = sin(latitudeRad);
	double cosDec = cos(decRad);
	double cosLat = cos(latitudeRad);
	//TODO: Always using hour angle of 12.0 as a simplification.
	double haHours = 12.0; //getHourAngle(date, ra/15.0, longitude);
	double ha = M_PI * haHours / 12.0;
	double cosHa = cos(ha);

	double sinAltitude = sinDec * sinLat + cosDec * cosLat * cosHa;

	double Airmass = 1.0 / sinAltitude;

	double Tamb = 270.0; // Ambient temperature (260 - 280 K)
	double etaFeed = 0.95; // forward efficiency
	double Trx = getReceiverTemperature(data.frequency);

	double tauZero = data.opacity;
	double Tatm = data.brightnessTemperature;

	double f = exp(tauZero * Airmass);
	double Tcmb = 2.725; // [K]

//	Trx  = planck(frequency, Trx);
//	Tatm = planck(frequency, Tatm);
//	Tamb = planck(frequency, Tamb);

	double Tsys = (Trx + Tatm * etaFeed * (1.0 - 1.0 / f)
	            + Tamb * (1.0 - etaFeed));
	// GHz, K
	Tsys = f * Tsys + Tcmb;
	return Tsys;
}


void WeatherHandler::estimatePwv(weatherCondition& weather)
{
	// If any of temperature or humidity is invalid, nothing to do
	if (weather.temperature == INVALID_DATA || weather.humidity == INVALID_DATA)
		return;

	double P_0; // water vapor partial pressure
	double theta; // inverse temperature [K]
	double m_w = 18 * 1.660538782E-27; // mass of a water molecule (18 amu in Kg)
	double H = 1.5E3; // scale height of water vapor distribution
	double rho_l = 1e3; // density of water [Kg/m^3]
	double k = 1.3806503E-23; // Boltzmann constant [m^2 Kg s^-2 K^-1]
	double T_0; // ground temperature in Kelvins

	// convert temperature to degrees Kelvin
	T_0 = weather.temperature + 273.15;
	theta = 300.0/T_0;
	P_0 = 2.409E12 * weather.humidity * pow(theta, 4) * exp(-22.64 * theta);

	weather.pwv = ( m_w * P_0 * H ) / ( rho_l * k * T_0 ) * 1E3; // in mm
}

/*
 * A simple linear interpolation routine.
 *
 * @param x independent variable to interpolate, should be between x1 and x2
 * @param x1 independent variable value 1
 * @param x2 independent variable value 2
 * @param y1 dependent variable value for x1
 * @param y2 dependent variable value for x2
 * @return interpolation for the dependent variable, for the value x
 */
double interpolate(double x, double x1, double x2, double y1, double y2)
{
	return y1 + ( y2 - y1 ) * ( x - x1 ) / ( x2 - x1 );
}

void WeatherHandler::interpolateOpacityAndTemperature(atmosphericData& data)
{
	// If any of PWV or frequency is invalid, nothing to do
	if (data.pwv == INVALID_DATA || data.frequency == INVALID_DATA)
		return;

	// First get the PWV interval
	double* pwvInterval = getEnclosingPwvInterval(data.pwv);
	// For the PWV lower bound, interpolate opacity and temperature as functions of frequency
	atmosphericData* atm;
	atm = getEnclosingIntervalForPwvAndFreq(pwvInterval[0], data.frequency);

	double interpOpacity1 = interpolate(data.frequency, atm[0].frequency, atm[1].frequency,
		atm[0].opacity, atm[1].opacity);
	double interpTemp1 = interpolate(data.frequency, atm[0].frequency, atm[1].frequency,
		atm[0].brightnessTemperature, atm[1].brightnessTemperature);
	
	// For the PWV upper bound, interpolate opacity and temperature as functions of frequency
	atm = getEnclosingIntervalForPwvAndFreq(pwvInterval[1], data.frequency);
	double interpOpacity2 = interpolate(data.frequency, atm[0].frequency, atm[1].frequency,
		atm[0].opacity, atm[1].opacity);
	double interpTemp2 = interpolate(data.frequency, atm[0].frequency, atm[1].frequency,
		atm[0].brightnessTemperature, atm[1].brightnessTemperature);

	// Finally, interpolate opacity and temperature again as functions of PWV.
	// Do this only if the PWV's are different, if not just return the first interpolated
	// values.
	if (pwvInterval[0] != pwvInterval[1]) {
		double finalOpacity = interpolate(data.pwv, pwvInterval[0], pwvInterval[1],
			interpOpacity1, interpOpacity2);
		double finalTemp = interpolate(data.pwv, pwvInterval[0], pwvInterval[1],
			interpTemp1, interpTemp2);
		data.opacity = finalOpacity;
		data.brightnessTemperature = finalTemp;
	} else {
		data.opacity = interpOpacity1;
		data.brightnessTemperature = interpTemp1;
	}
}

double* WeatherHandler::getEnclosingPwvInterval(double pwv) {
	double* retVal = new double[2];
	retVal[0] = retVal[1] = INVALID_DATA;
	double previous = atmDataRecords->begin()->first;
	double current = INVALID_DATA;
	map< double, list<asch_input::AtmData*> >::iterator mapIter;
	for (mapIter=atmDataRecords->begin(); mapIter!=atmDataRecords->end(); mapIter++) {
		current = mapIter->first;
		if (pwv < current)
			break;
		if (pwv >= previous && pwv < current) {
			retVal[0] = previous;
			retVal[1] = current;
			break;
		}
		previous = current;
	}
	if (retVal[0] == INVALID_DATA || retVal[1] == INVALID_DATA) {
//		cerr << "WARNING: PWV out of higher range: " << pwv << endl;
		retVal[0] = retVal[1] = current;
	} else if (retVal[0] == 0.0 || retVal[1] == 0.0) {
//		cerr << "WARNING: PWV out of lower range: " << pwv << endl;
		retVal[0] = retVal[1] = current;
	}

	return retVal;
}

atmosphericData* WeatherHandler::getEnclosingIntervalForPwvAndFreq(double pwv, double frequency) {
	atmosphericData* retVal = new atmosphericData[2];
	double parentPwv = getEnclosingPwvInterval(pwv)[0];
//	cout << "PWV, frequency: " << pwv << "(" << parentPwv << ")" << "," << frequency << endl;

	list<asch_input::AtmData*> atmList = atmDataRecords->find(parentPwv)->second;
	list<asch_input::AtmData*>::iterator listIter;
	asch_input::AtmData* previous = *(atmList.begin());
	for (listIter=atmList.begin(); listIter!=atmList.end(); listIter++) {
		asch_input::AtmData* current = (*listIter);
		if (frequency >= previous->getFrequency() && frequency <= current->getFrequency()) {
			retVal[0].pwv = previous->getPwv();
			retVal[0].frequency = previous->getFrequency();
			retVal[0].opacity = previous->getOpacity();
			retVal[0].brightnessTemperature = previous->getTemperature();
			retVal[1].pwv = current->getPwv();
			retVal[1].frequency = current->getFrequency();
			retVal[1].opacity = current->getOpacity();
			retVal[1].brightnessTemperature = current->getTemperature();
			break;
		}
		previous = current;
	}

	return retVal;
}

