#ifndef __XMLREADER_HPP__
#define __XMLREADER_HPP__

#include "XmlInputDao.hpp"

#include "xsd-head/config.hxx"
#include "xsd-head/executive.hxx"
#include "xsd-head/observatory.hxx"
#include "xsd-head/obsunit.hxx"

#include <iostream>
#include <dirent.h>
#include <unistd.h> // getcwd
#include <limits.h> // PATH_MAX

namespace asch_input{

class XmlReader:public XmlInputDao{
	public:
	XmlReader(std::string isource = "../PMS/tmp/");
	
	/*ExecutiveDao Interface*/
	ObsSeason * getCurrentObsSeason();
	ObsSeason * getObsSeasonByName(std::string name);
	Executive * getExecutiveByName(std::string name,ObsSeason actual);
	std::list<Executive *>* getAllExecutives(ObsSeason actual);

	/*ObservatoryDao Interface*/
	ArrayConfiguration * getArrayConfigByDate(DateTime date);
	std::list<ArrayConfiguration *>* getAllArrayConfigs();

	/*ObsProjectDao Interface*/
	std::list<SchedBlock *>* getAllSBs();
	std::list<ObsProject *>* getAllObsProject();
	std::list<SchedBlock *>* getSBsByObsProject(ObsProject * id);

	/*Configuration*/
	double getLatitude();
	double getLongitude();

	private:
	std::string inputSource;
	
	std::string ProjectDirectory;
	std::string ObservatoryDirectory;
	std::string ExecutiveDirectory;
	double Latitude;
	double Longitude;
	char cwd[PATH_MAX];
	
};

}
#endif
