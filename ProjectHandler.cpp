#include "classes.hpp"


bool FileExists(string strFilename) {
	struct stat stFileInfo;
	bool blnReturn;
	int intStat;

	// Attempt to get the file attributes
	intStat = stat(strFilename.c_str(),&stFileInfo);
	if(intStat == 0) {
		// We were able to get the file attributes
		// so the file obviously exists.
		blnReturn = true;
	} else {
		// We were not able to get the file attributes.
		// This may mean that we don't have permission to
		// access the folder which contains this file. If you
		// need to do that level of checking, lookup the
		// return values of stat which will give you
		// more details on why stat failed.
		blnReturn = false;
	}

	return(blnReturn);
}



using namespace std;

ProjectHandler::ProjectHandler():
	startDate(0L),
	endDate(0L)
{
}

ProjectHandler::~ProjectHandler(){
	while (!projects.empty()) {
		Project* oldProject = projects.front();
		projects.pop_front();
		delete oldProject;
	}
}

void ProjectHandler::initialize(string dataSource){
	cout << "ProjectHandler::initialize() Start." << endl;

	if(!FileExists(dataSource + string("aprc-config.xml")))
	{

		cerr 	<< "ProjectHandler::initialize() error:"
				<< "Could not open \"" << dataSource + string("aprc-config.xml") << "\"." << endl;
		return;
	}

	asch_input::XmlReader * reader = new asch_input::XmlReader(dataSource);

	//Initialize Latitude and Longitude
	latitude = reader->getLatitude();
	longitude = reader->getLongitude();

	/*WARNING! UT Modifier HARDCODED!*/
	UT = -4;

	//Initialize period startDate and endDate.
	// Using "EarlyScience" Data
	asch_input::ObsSeason * os = reader->getObsSeasonByName(string("EarlyScience2011"));

	struct tm aux_tm = os->getStartDate().getTm();

	this->startDate = mktime(&aux_tm);

	aux_tm = os->getEndDate().getTm();
	this->endDate = mktime(&aux_tm);

	delete os;

	/* Get all Projects */
	list<asch_input::ObsProject *>* projects = reader->getAllObsProject();

	int project_cant = projects->size();
	if(project_cant <= 0 )
	{
		/*NO DATA IN INPUT FILES ERROR!*/
		cerr << "ProjectHandler::initialize() error:" "No projects found." << endl;

		delete reader;
		delete projects;
		return;
	}

	int sb_counter = 0;

	for(	list<asch_input::ObsProject *>::iterator actp_it = projects->begin();
			actp_it != projects->end();
			actp_it++)
	{
		asch_input::ObsProject *actp = *actp_it;

		Project * nproj = new Project(this, actp->getId(), actp->getScientificScore());

		list<asch_input::SchedBlock *>* schedblocks = reader->getSBsByObsProject(actp);
		int sb_index = 0;
		/* Get SBs from Project */
		for(	list<asch_input::SchedBlock *>::iterator acts_it = schedblocks->begin();
				acts_it != schedblocks->end();
				acts_it++)
		{

			asch_input::SchedBlock *acts = *acts_it;
			asch_input::WeatherConstrains wc = acts->getWeatherConstrains();

			/*Get LTS rise & LTS set values*/
			list<asch_input::Target *>* tg_list = acts->getTarget();
			list<asch_input::FieldSource *>* fs_list = acts->getFieldSource();

			double RA  = (*(fs_list->begin()))->getRA().get();
			double Dec = (*(fs_list->begin()))->getDec().get();
			double LSTr = 24.0 - (1.0/15.0)* acos(-tan(0.0174532925*latitude)*tan(0.0174532925*Dec))/0.0174532925 + RA;
			double LSTs = (1.0/15.0)* acos(-tan(0.0174532925*latitude)*tan(0.0174532925*Dec))/0.0174532925 + RA;

			string tg_id_ref = acts->getSchedulingConstrains().getRepresentativeTargetIdRef().get();
			string src_id_ref = "";

			for(list<asch_input::Target *>::iterator tg_it = tg_list->begin(); tg_it != tg_list->end();tg_it++)
			{
				if( tg_id_ref.compare((*tg_it)->getId()) == 0 )
				{
					src_id_ref = (*tg_it)->getSourceIdRef();
					break;
				}
			}

			for(list<asch_input::FieldSource *>::iterator fs_it = fs_list->begin(); fs_it != fs_list->end();fs_it++)
			{
				asch_input::FieldSource * fs = *fs_it;

				if(src_id_ref.compare(fs->getId()) == 0)
				{
						RA  = fs->getRA().get();
						Dec  = fs->getDec().get();
						LSTr = 24.0 - (1.0/15.0)* acos(-tan(0.0174532925*latitude)*tan(0.0174532925*Dec))/0.0174532925 + RA;
						LSTs = (1.0/15.0)* acos(-tan(0.0174532925*latitude)*tan(0.0174532925*Dec))/0.0174532925 + RA;
				}
			}

			//TODO: Add some random sequence block dependencies?
			int seqBlockIndex = -1;
			time_t duration = acts->getSchedulingBlockControl().getMaximumTime() * 60;
			if (duration > 3600) // Maximum duration is 1 TS (restriction)
				duration = 3600;
			double obsFreq = acts->getSchedulingConstrains().getRepresentativeFrequency().present() ? acts->getSchedulingConstrains().getRepresentativeFrequency().get() : 20.0;
			nproj->addSchedBlock(sb_index,
								seqBlockIndex,
								duration,
								obsFreq,
								RA,Dec,LSTr,LSTs);

			sb_counter++;
			sb_index++;
		}

		this->projects.push_back( nproj );

		delete schedblocks;
	}

	cout << "ProjectHandler was successfully initialized" << endl;
	cout << "-------------------------------------------" << endl;
	cout << "Start Date: " << this->startDate << endl;
	cout << "End Date: " << this->endDate << endl;
	cout << "Latitude: " << this->latitude << endl;
	cout << "Longitude: " << this->longitude << endl;
	cout << "Projects: " << project_cant << endl;
	cout << "Scheduling Blocks: " << sb_counter << endl;
	cout << endl;

	delete projects;
	delete reader;
}

list<Project*> ProjectHandler::getProjects(){
	return this->projects;
}

list<Project*> ProjectHandler::getCompletedProjects(){
	list<Project*> completed;

	for(	list<Project *>::iterator pr_it = projects.begin();
			pr_it != projects.end();
			pr_it++)
	{
		if((*pr_it)->getRemainingExec() == 0.0)
		{
			completed.push_back(*pr_it);
		}
	}

	return completed;
}

list<Project*> ProjectHandler::getPendingProjects(){
	list<Project*> uncompleted;

	for(	list<Project *>::iterator pr_it = projects.begin();
			pr_it != projects.end();
			pr_it++)
	{
		if((*pr_it)->getRemainingExec() > 0.0)
		{
			uncompleted.push_back(*pr_it);
		}
	}
	return uncompleted;
}

Project* ProjectHandler::getProject(unsigned int projectIndex){
	for(	list<Project *>::iterator pr_it = projects.begin();
			pr_it != projects.end();
			pr_it++)
	{
		if((*pr_it)->getIndex() == projectIndex)
			return *pr_it;
	}
	return (Project*)NULL;
}

time_t ProjectHandler::getStartDate(){
	return startDate;
}

time_t ProjectHandler::getEndDate(){
	return endDate;
}

double ProjectHandler::getLatitude(){
	return latitude;
}
double ProjectHandler::getLongitude(){
	return longitude;
}
int ProjectHandler::getUT(){
	return UT;
}

