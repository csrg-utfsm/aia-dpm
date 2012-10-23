#ifndef __EXECUTIVEDAO_HPP__
#define __EXECUTIVEDAO_HPP__

#include "Dao.hpp"
#include "DataTypes/Executive.hpp"
#include <list>

namespace asch_input{

class ExecutiveDao: public Dao {
	public:
	virtual ObsSeason * getCurrentObsSeason()=0;
	virtual ObsSeason * getObsSeasonByName(std::string name)=0;
	virtual Executive * getExecutiveByName(std::string name,ObsSeason actual)=0;
	virtual std::list<Executive * >* getAllExecutives(ObsSeason actual)=0;
};

}

#endif
