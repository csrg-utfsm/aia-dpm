#ifndef __WEATHERDAO_HPP__
#define __WEATHERDAO_HPP__

#include "Dao.hpp"
#include "DataTypes/WindSpeed.hpp"
#include "DataTypes/Humidity.hpp"
#include "DataTypes/Temperature.hpp"
#include "DataTypes/Opacity.hpp"
#include <list>

namespace asch_input{

class WeatherDao: public Dao {
	public:
	virtual std::list<WindSpeed *>* getAllWindSpeeds()=0;
	virtual std::list<Humidity *>* getAllHumidities()=0;
	virtual std::list<Temperature *>* getAllTemperatures()=0;
	virtual std::list<Opacity *>* getAllOpacities()=0;
};

}

#endif
