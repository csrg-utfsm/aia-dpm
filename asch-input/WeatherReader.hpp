#ifndef __WEATHERREADER_HPP__
#define __WEATHERREADE_HPP__

#include "DataTypes/Temperature.hpp"
#include "DataTypes/Humidity.hpp"
#include "DataTypes/Opacity.hpp"
#include "DataTypes/WindSpeed.hpp"
#include "DataTypes/AtmData.hpp"
#include <list>
#include <string>

#include "xsd-head/config.hxx"
#include <unistd.h> // getcwd
#include <limits.h> // PATH_MAX

namespace asch_input
{

class WeatherReader
{
private:
    std::list<Temperature*>* temperatureRecords;
    std::list<Humidity*>* humidityRecords;
    std::list<Opacity*>* opacityRecords;
    std::list<WindSpeed*>* windspeedRecords;
    std::map< double, std::list<AtmData*> >* atmDataRecords;

    std::string temperatureFile;
    std::string opacityFile;
    std::string windspeedFile;
    std::map<double,std::string> pwvToAtmDataFiles;

public:
    WeatherReader(std::string isource = "../PMS/tmp/");
    ~WeatherReader();

    void loadTemperatureHistory();
    void loadHumidityHistory();
    void loadOpacityHistory();
    void loadWindSpeedHistory();
    void loadAtmData();

	std::list<Temperature*>* getTemperatureRecords();
	std::list<Humidity*>* getHumidityRecords();
	std::list<Opacity*>* getOpacityRecords();
	std::list<WindSpeed*>* getWindspeedRecords();
	std::map<double, std::list<AtmData*> >* getAtmDataRecords();

};

}

#endif
