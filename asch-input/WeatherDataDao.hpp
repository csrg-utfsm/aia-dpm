#ifndef __WEATHERDATADAO_HPP__
#define __WEATHERDATADAO_HPP__

#include "WeatherDao.hpp"

namespace asch_input{

class WeatherDataDao:public WeatherDao{
	/*Dao Interface*/
	virtual void loadData();
	virtual void unloadData();
	
	/*WeatherDao Interface*/
	virtual std::list<WindSpeed *>* getAllWindSpeeds();
	virtual std::list<Humidity *>* getAllHumidities();
	virtual std::list<Temperature *>* getAllTemperatures();
	virtual std::list<Opacity *>* getAllOpacities();

};

}
#endif
