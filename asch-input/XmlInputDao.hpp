#ifndef __XMLINPUTDAO_HPP__
#define __XMLINPUTDAO_HPP__

#include "ExecutiveDao.hpp"
#include "ObservatoryDao.hpp"
#include "ObsProjectDao.hpp"

namespace asch_input{

class XmlInputDao:public ExecutiveDao,public ObsProjectDao,public ObservatoryDao{
	public:

	/*ExecutiveDao Interface*/
	virtual ObsSeason * getCurrentObsSeason() =0;
	virtual ObsSeason * getObsSeasonByName(std::string name)=0;
	virtual Executive * getExecutiveByName(std::string name,ObsSeason actual)=0;
	virtual std::list<Executive *>* getAllExecutives(ObsSeason actual)=0;

	/*ObservatoryDao Interface*/
	virtual ArrayConfiguration * getArrayConfigByDate(DateTime date)=0;
	virtual std::list<ArrayConfiguration *>* getAllArrayConfigs()=0;

	/*ObsProjectDao Interface*/
	virtual std::list<SchedBlock *>* getAllSBs()=0;
	virtual std::list<ObsProject *>* getAllObsProject()=0;
	virtual std::list<SchedBlock *>* getSBsByObsProject(ObsProject * id)=0;

};

}
#endif
