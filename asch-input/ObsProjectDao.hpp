#ifndef __OBSPROJECTDAO_HPP__
#define __OBSPROJECTDAO_HPP__

#include "Dao.hpp"
#include "DataTypes/ObsUnit.hpp"
#include <list>

namespace asch_input{

class ObsProjectDao: public Dao {
	public:
	virtual std::list<SchedBlock *>* getAllSBs()=0;
	virtual std::list<ObsProject *>* getAllObsProject()=0;
	virtual std::list<SchedBlock *>* getSBsByObsProject(ObsProject * id)=0;
};

}

#endif
