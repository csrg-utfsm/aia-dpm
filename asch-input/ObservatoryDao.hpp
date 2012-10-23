#ifndef __OBSERVATORYDAO_HPP__
#define __OBSERVATORYDAO_HPP__

#include "Dao.hpp"
#include "DataTypes/Observatory.hpp"
#include "DataTypes/DateTime.hpp"
#include <list>

namespace asch_input{

class ObservatoryDao: public Dao {
	public:
	virtual ArrayConfiguration * getArrayConfigByDate(DateTime date)=0;
	virtual std::list<ArrayConfiguration *>* getAllArrayConfigs()=0;
};

}

#endif
