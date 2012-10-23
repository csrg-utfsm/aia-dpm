#include "XmlReader.hpp"
#include <iostream>
using namespace asch_input;

DateTime xmlDateTime(xsd::cxx::tree::date_time<char, xsd::cxx::tree::simple_type<xsd::cxx::tree::_type> > x){
	return DateTime(x.year(),x.month(),x.day(),x.hours(),x.minutes(),x.seconds());
}

/******
 * Constructor
 ******/

XmlReader::XmlReader(std::string isource):
inputSource(isource)
{
	if (getcwd (cwd, PATH_MAX) == 0)
	{
	  // Buffer too small?
	}

	try{
		xml_schema::properties config_xsd;
		config_xsd.no_namespace_schema_location ("file://" + std::string (cwd) +"/config/config.xsd");
//		config_xsd.schema_location ("http://www.w3.org/XML/1998/namespace", "file://" + std::string (cwd) + "config/xml.xsd");
		std::auto_ptr<Configuration> configRoot(Configuration_(inputSource + "aprc-config.xml",xml_schema::flags::dont_validate,config_xsd));

		/*Loading configured directories to read from*/
		ProjectDirectory = (isource + configRoot->ProjectDirectory() + "/");
		ObservatoryDirectory = (isource + configRoot->ObservatoryDirectory() + "/");
		ExecutiveDirectory = (isource + configRoot->ExecutiveDirectory() + "/");

		/*Latitude and Longitude*/
		Latitude = configRoot->ArrayCenterLatitude();
		Longitude = configRoot->ArrayCenterLongitude();
	}
	catch (const xml_schema::exception& e)
	{
		std::cerr << e << std::endl;
	}

}

/******
 * ExecutiveDao Interface
 ******/

ObsSeason * XmlReader::getCurrentObsSeason()
{
	ObsSeason *ret = NULL;
	/*TODO: GET ACTUAL DATE SOMEHOW*/
	DateTime now(0,0,0,0,0,0);

	try{
		/*Open the file*/
		xml_schema::properties executive_xsd;
		executive_xsd.no_namespace_schema_location ("file://" + std::string (cwd) +"/config/executive.xsd");
//		executive_xsd.schema_location ("http://www.w3.org/XML/1998/namespace", "file://" + std::string (cwd) + "config/xml.xsd");
		std::auto_ptr<ExecutiveData> execRoot(ExecutiveData_(ExecutiveDirectory + "Executives.xml",xml_schema::flags::dont_validate,executive_xsd));

		/*Reading Observing Seasons from ExcecutiveData_*/
		for (ExecutiveData::ObservingSeason_const_iterator i (execRoot->ObservingSeason().begin());
			i != execRoot->ObservingSeason().end ();
			++i)
		{
			/*act: currently parsed observing season*/
			ObservingSeason act = *i;

			/*checking if it belongs to the actual date*/
			if(now.between(xmlDateTime(act.startDate()),xmlDateTime(act.endDate())))
			{
				ret = new ObsSeason(act.name(),xmlDateTime(act.startDate()),xmlDateTime(act.endDate()));
			}
			else
			{
				continue;
			}
			break;
		}
	}
	catch (const xml_schema::exception& e)
	{
		std::cerr << e << std::endl;
	}
	return ret;
}

ObsSeason * XmlReader::getObsSeasonByName(std::string name)
{
	ObsSeason *ret = NULL;

	try{
		xml_schema::properties executive_xsd;
		executive_xsd.no_namespace_schema_location ("file://" + std::string (cwd) +"/config/executive.xsd");
//		executive_xsd.schema_location ("http://www.w3.org/XML/1998/namespace", "file://" + std::string (cwd) + "config/xml.xsd");
		std::auto_ptr<ExecutiveData> execRoot(ExecutiveData_(ExecutiveDirectory + "Executives.xml",xml_schema::flags::dont_validate,executive_xsd));
		for (ExecutiveData::ObservingSeason_const_iterator i (execRoot->ObservingSeason().begin());
			i != execRoot->ObservingSeason().end ();
			++i)
		{
			ObservingSeason act = *i;
			if(std::string(act.name()) == name)
			{
				ret = new ObsSeason(act.name(),xmlDateTime(act.startDate()),xmlDateTime(act.endDate()));
			}
			else
			{
				continue;
			}
			break;
		}
	}
	catch (const xml_schema::exception& e)
	{
		std::cerr << e << std::endl;
	}
	return ret;
}

asch_input::Executive * XmlReader::getExecutiveByName(std::string name, ObsSeason actualObs)
{
	asch_input::Executive *ret = NULL;

	try{
		xml_schema::properties executive_xsd;
		executive_xsd.no_namespace_schema_location ("file://" + std::string (cwd) +"/config/executive.xsd");
//		executive_xsd.schema_location ("http://www.w3.org/XML/1998/namespace", "file://" + std::string (cwd) + "config/xml.xsd");
		std::auto_ptr<ExecutiveData> execRoot(ExecutiveData_(ExecutiveDirectory + "Executives.xml",xml_schema::flags::dont_validate,executive_xsd));

		ExecutivePercentage ep("",0);

		for (ExecutiveData::ObservingSeason_const_iterator i(execRoot->ObservingSeason().begin());
			i != execRoot->ObservingSeason().end ();
			++i)
		{
			ObservingSeason act = *i;
			if(std::string(act.name()) == actualObs.getName())
			{
				for (ObservingSeason::ExecutivePercentage_const_iterator j(act.ExecutivePercentage().begin());
					j != act.ExecutivePercentage().end();
					++j)
				{
					if(std::string(j->executiveRef()) == name)
					{
						ep = *j;
					}
					else
					{
						continue;
					}
					break;
				}
			}
			else
			{
				continue;
			}
			break;
		}

		for (ExecutiveData::Executive_const_iterator i (execRoot->Executive().begin());
			i != execRoot->Executive().end ();
			++i)
		{
			::Executive act = *i;
			if(act.name() == name)
			{
				ret = new asch_input::Executive(
							string(act.name()),
							act.defaultPercentage(),
							ep.percentage(),
							Optional<float>(
								ep.totalObsTimeForSeason().present()?
									ep.totalObsTimeForSeason().get():
									0.0,
								ep.totalObsTimeForSeason().present()));
			}
			else
			{
				continue;
			}
			break;
		}
	}
	catch (const xml_schema::exception& e)
	{
		std::cerr << e << std::endl;
	}

	return ret;
}

std::list<asch_input::Executive *> * XmlReader::getAllExecutives(ObsSeason actualObs)
{
	list<asch_input::Executive *>* ret = new list<asch_input::Executive *>();

	try{
		xml_schema::properties executive_xsd;
		executive_xsd.no_namespace_schema_location ("file://" + std::string (cwd) +"/config/executive.xsd");
//		executive_xsd.schema_location ("http://www.w3.org/XML/1998/namespace", "file://" + std::string (cwd) + "config/xml.xsd");
		std::auto_ptr<ExecutiveData> execRoot(ExecutiveData_(ExecutiveDirectory + "Executives.xml",xml_schema::flags::dont_validate,executive_xsd));

		/*Find Actual Observing Season*/
		for (ExecutiveData::ObservingSeason_const_iterator i(execRoot->ObservingSeason().begin());
			i != execRoot->ObservingSeason().end ();
			++i)
		{
			ObservingSeason act = *i;
			if(std::string(act.name()) == actualObs.getName())
			{
				/*Retrieve all Executives with their actual percentages for the Season*/
				for (ObservingSeason::ExecutivePercentage_const_iterator j(act.ExecutivePercentage().begin());
					j != act.ExecutivePercentage().end();
					++j)
				{
					ExecutivePercentage ep = *j;

					for (ExecutiveData::Executive_const_iterator k (execRoot->Executive().begin());
						k != execRoot->Executive().end ();
						++k)
					{
						::Executive act = *k;
						if( std::string(ep.executiveRef()) == std::string(act.name()))
						{
							ret->push_back(
								new asch_input::Executive(
									string(act.name()),
									act.defaultPercentage(),
									ep.percentage(),
									Optional<float>(ep.totalObsTimeForSeason().present()?
										ep.totalObsTimeForSeason().get():
										0.0
									,ep.totalObsTimeForSeason().present())));
						}
						else
						{
							continue;
						}
						break;
					}
				}
			}
			else
			{
				continue;
			}
			break;
		}
	}
	catch (const xml_schema::exception& e)
	{
		std::cerr << e << std::endl;
	}

	return ret;
}

/******
 * ObservatoryDao Interface
 ******/

asch_input::ArrayConfiguration * XmlReader::getArrayConfigByDate(DateTime date)
{
	asch_input::ArrayConfiguration *ret = NULL;

	try{
		xml_schema::properties observatory_xsd;
		observatory_xsd.no_namespace_schema_location ("file://" + std::string (cwd) +"/config/observatory.xsd");
//		observatory_xsd.schema_location ("http://www.w3.org/XML/1998/namespace", "file://" + std::string (cwd) + "config/xml.xsd");
		std::auto_ptr<ObservatoryCharacteristics> obseRoot(ObservatoryCharacteristics_(ObservatoryDirectory + "Observatory.xml",xml_schema::flags::dont_validate,observatory_xsd));

		ArrayConfigurationSetT array = obseRoot->Array();

		for (ArrayConfigurationSetT::ArrayConfiguration_const_iterator i (array.ArrayConfiguration().begin());
			i != array.ArrayConfiguration().end ();
			++i)
		{
			::ArrayConfigurationT act = *i;
			if(	act.startTime().present() &&
				act.endTime().present() &&
				date.between(
					xmlDateTime(act.startTime().get()),
					xmlDateTime(act.endTime().get())
				)
			)
			{
				ret = new asch_input::ArrayConfiguration(
					Optional<DateTime>(xmlDateTime(act.startTime().get()),true),
					Optional<DateTime>(xmlDateTime(act.endTime().get()),true),
					Optional<double>(act.resolution().present()?act.resolution().get():0,act.resolution().present()),
					Optional<double>(act.uvCoverage().present()?act.uvCoverage().get():0,act.uvCoverage().present()));
			}
		}
	}
	catch (const xml_schema::exception& e)
	{
		std::cerr << e << std::endl;
	}

	return ret;
}

std::list<ArrayConfiguration *>* XmlReader::getAllArrayConfigs()
{
	std::list<ArrayConfiguration *>* ret = new std::list<ArrayConfiguration *>();

	try{
		xml_schema::properties observatory_xsd;
		observatory_xsd.no_namespace_schema_location ("file://" + std::string (cwd) +"/config/observatory.xsd");
//		observatory_xsd.schema_location ("http://www.w3.org/XML/1998/namespace", "file://" + std::string (cwd) + "config/xml.xsd");
		std::auto_ptr<ObservatoryCharacteristics> obseRoot(ObservatoryCharacteristics_(ObservatoryDirectory + "Observatory.xml",xml_schema::flags::dont_validate,observatory_xsd));

		ArrayConfigurationSetT array = obseRoot->Array();

		for (ArrayConfigurationSetT::ArrayConfiguration_const_iterator i (array.ArrayConfiguration().begin());
			i != array.ArrayConfiguration().end ();
			++i)
		{
			::ArrayConfigurationT act = *i;

			ret->push_back( new asch_input::ArrayConfiguration(
					Optional<DateTime>(xmlDateTime(act.startTime().get()),true),
					Optional<DateTime>(xmlDateTime(act.endTime().get()),true),
					Optional<double>(act.resolution().present()?act.resolution().get():0,act.resolution().present()),
					Optional<double>(act.uvCoverage().present()?act.uvCoverage().get():0,act.uvCoverage().present())));
		}
	}
	catch (const xml_schema::exception& e)
	{
		std::cerr << e << std::endl;
	}

	return ret;
}
/******
 * ObsProjectDao Interface
 ******/

std::list<asch_input::ObsProject *>* XmlReader::getAllObsProject()
{
	std::list<asch_input::ObsProject *>* ret = new std::list<asch_input::ObsProject *>();

	DIR *dir;
	struct dirent* dp;


	if (!(dir = opendir(ProjectDirectory.c_str()))) {
		std::cerr << "Dir Not Found" << std::endl;
		return ret;
	}

	/*FOR xmls in the dir*/
	while ((dp = readdir(dir))) {
		std::string actFilename(dp->d_name);

		/*Check if it's a proyect file*/
		//if(dp->d_type != DT_REG) continue;

		int start_char = actFilename.find("ObsProject");
		if(start_char < 0) continue;
		if(actFilename.find(".xml")!=actFilename.length()-4) continue;

		/*Get the PID*/
		long pid = atoi( actFilename.substr(10 + start_char, actFilename.length() - 4 - (10 + start_char) ).c_str() );

		actFilename = ProjectDirectory + actFilename;

		/*Open xml*/
		try{
			xml_schema::properties obsunit_xsd;
			obsunit_xsd.no_namespace_schema_location ("file://" + std::string (cwd) +"/config/obsunit.xsd");
//			obsunit_xsd.schema_location ("http://www.w3.org/XML/1998/namespace", "file://" + std::string (cwd) + "config/xml.xsd");
			std::auto_ptr< ::ObsProject> obspRoot(ObsProject_(actFilename,xml_schema::flags::dont_validate,obsunit_xsd));

			/*Get Proyect Info*/
			ret->push_back(new asch_input::ObsProject(pid, obspRoot->PrincipalInvestigator(), obspRoot->ScientificScore(), obspRoot->ScientificRank()));
		}
		catch (const xml_schema::exception& e)
		{
			std::cerr << e << std::endl;
		}
	}

	closedir(dir);

	return ret;

}

SchedBlock * read_scheduling_block(SchedBlockT sb, long pid)
{
	/*Instrument Spec*/
	list<InstrumentSpec *>* sbIsL = new list<InstrumentSpec *>();

	for(SchedBlockT::InstrumentSpec_const_iterator j(sb.InstrumentSpec().begin());
		j != sb.InstrumentSpec().end();
		j++
	)
	{
		InstrumentSpecT actis = *j;

		Optional<InstrumentSpec::ObservingModeType> om(InstrumentSpec::Holography,false);

		if(actis.ObservingMode().present())
		{
			switch(actis.ObservingMode().get())
			{
				case ObservingMode::OpticalPointing:
					om = Optional<InstrumentSpec::ObservingModeType>(InstrumentSpec::OpticalPointing,true);
					break;
				case ObservingMode::SingleFieldInterferometry:
					om = Optional<InstrumentSpec::ObservingModeType>(InstrumentSpec::SingleFieldInterferometry,true);
					break;
				case ObservingMode::TotalPower:
					om = Optional<InstrumentSpec::ObservingModeType>(InstrumentSpec::TotalPower,true);
					break;
				case ObservingMode::Holography:
					om = Optional<InstrumentSpec::ObservingModeType>(InstrumentSpec::Holography,true);
					break;
			}
		}

		Optional<HardwareRequirements> hr(HardwareRequirements(NULL),false);

		if(actis.HardwareRequirements().present())
		{

			HardwareRequirementsT acthr = actis.HardwareRequirements().get();

			list<EquipmentRequirement *>* er = new list<EquipmentRequirement *>();

			for(HardwareRequirementsT::EquipmentRequirement_const_iterator k(acthr.EquipmentRequirement().begin());
				k != acthr.EquipmentRequirement().end();
				k++
			)
			{
				EquipmentRequirementT acter = *k;

				list<AssemblyRequirement *>* ar = new list<AssemblyRequirement *>();

				for(EquipmentRequirementT::AssemblyRequirement_const_iterator m(acter.AssemblyRequirement().begin());
					m != acter.AssemblyRequirement().end();
					m++
				)
				{
					AssemblyRequirementT actar = *m;

					ar->push_back(new AssemblyRequirement(
						actar.name().present()?
							Optional<string>(string(actar.name().get()),true):
							Optional<string>(string(""),false)
						));

				}

				er->push_back(new EquipmentRequirement(ar));
			}

			hr = Optional<HardwareRequirements>(HardwareRequirements(er),true);
		}

		sbIsL->push_back(new InstrumentSpec(
			om,
			actis.SpectralSpec().present()?
				Optional<SpectralSpec>(SpectralSpec(),true):
				Optional<SpectralSpec>(SpectralSpec(),false),
			actis.OpticalCameraSpec().present()?
				Optional<OpticalCameraSpec>(OpticalCameraSpec(),true):
				Optional<OpticalCameraSpec>(OpticalCameraSpec(),false),
			hr
			));



	}

	/*Target*/
	list<Target *>* sbTL = new list<Target *>();

	for(SchedBlockT::Target_const_iterator j(sb.Target().begin());
		j != sb.Target().end();
		j++
	)
	{
		TargetT acttrgt = *j;

		sbTL->push_back(new Target(
			string(acttrgt.id()),
			string(acttrgt.sourceIdRef()),
			string(acttrgt.instrumentSpecIdRef()),
			acttrgt.obsParametersIdRef().present()?
				string(acttrgt.obsParametersIdRef().get()):
				string("")
			));
	}

	/*FieldSource*/
	list<FieldSource *>* sbFsL = new list<FieldSource *>();

	for(SchedBlockT::FieldSource_const_iterator j(sb.FieldSource().begin());
		j != sb.FieldSource().end();
		j++
	)
	{
		FieldSourceT actfs = *j;

		sbFsL->push_back(new FieldSource(
			string(actfs.id()),
			actfs.name().present()?
				Optional<string>(string(actfs.name().get()),true):
				Optional<string>(string(""),false),
			actfs.RA().present()?
				Optional<double>(actfs.RA().get(),true):
				Optional<double>(0.0,false),
			actfs.Dec().present()?
				Optional<double>(actfs.Dec().get(),true):
				Optional<double>(0.0,false),
			actfs.ephemeris().present()?
				Optional<string>(string(actfs.ephemeris().get()),true):
				Optional<string>(string(""),false),
			actfs.pmRA().present()?
				Optional<double>(actfs.pmRA().get(),true):
				Optional<double>(0.0,false),
			actfs.pmDec().present()?
				Optional<double>(actfs.pmDec().get(),true):
				Optional<double>(0.0,false)
			));
	}

	/*Scheduling Block Control*/
	SchedBlockControlT sbc = sb.SchedBlockControl();

	SchedulingBlockControl sbSBC(sbc.MaximumTime(),sbc.EstimatedExecutionTime(),SchedulingBlockControl::ACA,sbc.IndefiniteRepeat());


	switch(sbc.ArrayRequested())
	{
		case ArrayTypeT::ACA:
			sbSBC = SchedulingBlockControl(sbc.MaximumTime(),sbc.EstimatedExecutionTime(),SchedulingBlockControl::ACA,sbc.IndefiniteRepeat());
			break;
		case ArrayTypeT::SEVEN_M:
			sbSBC = SchedulingBlockControl(sbc.MaximumTime(),sbc.EstimatedExecutionTime(),SchedulingBlockControl::SEVEN_M,sbc.IndefiniteRepeat());
			break;
		case ArrayTypeT::TP_ARRAY:
			sbSBC = SchedulingBlockControl(sbc.MaximumTime(),sbc.EstimatedExecutionTime(),SchedulingBlockControl::TP_ARRAY,sbc.IndefiniteRepeat());
			break;
		case ArrayTypeT::TWELVE_M:
			sbSBC = SchedulingBlockControl(sbc.MaximumTime(),sbc.EstimatedExecutionTime(),SchedulingBlockControl::TWELVE_M,sbc.IndefiniteRepeat());
			break;
	}

	/*Weather Constrains*/
	WeatherConstraintsT wc;
	WeatherConstrains wci(
				Optional<double>(0.0,false),
				Optional<double>(0.0,false),
				Optional<double>(0.0,false),
				Optional<double>(0.0,false));

	if(sb.WeatherConstraints().present())
	{
		wc = sb.WeatherConstraints().get();
		wci = WeatherConstrains(
			wc.maxWindVelocity().present()?
				Optional<double>(wc.maxWindVelocity().get(),true):
				Optional<double>(0.0,false),
			wc.maxOpacity().present()?
				Optional<double>(wc.maxOpacity().get(),true):
				Optional<double>(0.0,false),
			wc.minPhaseStability().present()?
				Optional<double>(wc.minPhaseStability().get(),true):
				Optional<double>(0.0,false),
			wc.maxSeeing().present()?
				Optional<double>(wc.maxSeeing().get(),true):
				Optional<double>(0.0,false)
		);
	}

	/*Scheduling Constrains*/
	SchedulingConstraintsT sc = sb.SchedulingConstraints();

	/*Preconditions*/
	PreconditionsT pc = sb.Preconditions();

	/*ObsParameters*/
	ObsParametersT op = sb.ObsParameters();


	return new SchedBlock(
		pid,
		wci,
		SchedulingConstrains(
			sc.maxAngularResolution().present()?
				Optional<double>(sc.maxAngularResolution().get(),true):
				Optional<double>(0.0,false),
			sc.representativeFrequency().present()?
				Optional<double>(sc.representativeFrequency().get(),true):
				Optional<double>(0.0,false),
			sc.representativeTargetIdRef().present()?
				Optional<string>(string(sc.representativeTargetIdRef().get()),true):
				Optional<string>(string(""),false)
		),
		Preconditions(
			pc.maxAllowedHA().present()?
				Optional<double>(pc.maxAllowedHA().get(),true):
				Optional<double>(0.0,false),
			pc.minAllowedHA().present()?
				Optional<double>(pc.minAllowedHA().get(),true):
				Optional<double>(0.0,false)

		),
		sbSBC,
		ObsParameters(
			string(op.id()),
			ScienceParameters(
				op.ScienceParameters().duration().present()?
					Optional<double>(op.ScienceParameters().duration().get(),true):
					Optional<double>(0.0,false),
				op.ScienceParameters().representativeBandwidth().present()?
					Optional<double>(op.ScienceParameters().representativeBandwidth().get(),true):
					Optional<double>(0.0,false),
				op.ScienceParameters().representativeFrequency().present()?
					Optional<double>(op.ScienceParameters().representativeFrequency().get(),true):
					Optional<double>(0.0,false),
				op.ScienceParameters().sensitivityGoal().present()?
					Optional<double>(op.ScienceParameters().sensitivityGoal().get(),true):
					Optional<double>(0.0,false)
			)
		),
		sbIsL,
		sbTL,
		sbFsL);
}


std::list<asch_input::SchedBlock *>* XmlReader::getSBsByObsProject(asch_input::ObsProject *id)
{

	std::list<asch_input::SchedBlock *>* ret = new std::list<asch_input::SchedBlock *>();

	DIR *dir;
	struct dirent* dp;

	if (!(dir = opendir(ProjectDirectory.c_str()))) {
        std::cerr << "Dir Not Found" << std::endl;
        return ret;
    }

	/*FOR xmls in the dir*/
	 while ((dp = readdir(dir))) {
		std::string actFilename(dp->d_name);

		/*Check if it's a proyect file*/
		//if(dp->d_type != DT_REG) continue;

		int start_char = actFilename.find("ObsProject");
		if(start_char < 0) continue;
		if(actFilename.find(".xml")!=actFilename.length()-4) continue;

		/*Get the PID*/
		long pid = atoi( actFilename.substr(10 + start_char, actFilename.length() - 4 - (10 + start_char) ).c_str() );

		/*Check if it's the needed pid*/
		if(id != NULL)
			if(pid != id->getId()) continue;

		actFilename = ProjectDirectory + actFilename;

		/*Open xml*/
		int counter = 0;
		try{
			xml_schema::properties obsunit_xsd;
			obsunit_xsd.no_namespace_schema_location ("file://" + std::string (cwd) +"/config/obsunit.xsd");

			std::auto_ptr< ::ObsProject> obspRoot(ObsProject_(actFilename,xml_schema::flags::dont_validate,obsunit_xsd));

			/*FOR SchedBlock in the project*/
			ObsUnitSetT schedBlockList2 = obspRoot->ObsUnitSet();

			for(ObsUnitSetT::ObsUnitSet_const_iterator j(schedBlockList2.ObsUnitSet().begin());
				j != schedBlockList2.ObsUnitSet().end();
				j++
			)
			{
				ObsUnitSetT schedBlockList = (*j);

				for(ObsUnitSetT::SchedBlock_const_iterator i(schedBlockList.SchedBlock().begin());
					i != schedBlockList.SchedBlock().end();
					i++
				)
				{
					ret->push_back(read_scheduling_block(*i,pid));
					counter ++;
				}
			}

			/* Old format compatibility */
			for(ObsUnitSetT::SchedBlock_const_iterator i(schedBlockList2.SchedBlock().begin());
				i != schedBlockList2.SchedBlock().end();
				i++
			)
			{
				ret->push_back(read_scheduling_block(*i,pid));
			}

		}
		catch (const xml_schema::exception& e)
		{
			std::cerr << e << std::endl;
		}

		if(id != NULL)
			break;
	}

	closedir(dir);

	return ret;
}


std::list<SchedBlock *>* XmlReader::getAllSBs()
{
	return getSBsByObsProject(NULL);
}

double XmlReader::getLatitude(){
	return Latitude;
}

double XmlReader::getLongitude(){
	return Longitude;
}
