#include "classes.hpp"

using namespace std;

Project::Project(ProjectHandler* projectHandler, double projectIndex, double sciGrade):
	projectHandler(projectHandler),
	projectIndex(projectIndex),
	sciGrade(sciGrade),
	remainingExec(1.0),
	numSchedBlocks(0U)
{
}

Project::~Project(){
	while (!schedBlocks.empty()) {
		SchedBlock* oldSchedBlock = schedBlocks.front();
		schedBlocks.pop_front();
		delete oldSchedBlock;
	}
}

unsigned int Project::getIndex(){
	return this->projectIndex;
}

double Project::getSciGrade(){
	return this->sciGrade;
}

double Project::getRemainingExec(){
	return this->remainingExec;
}

list<SchedBlock*> Project::getSchedBlocks(){
	return this->schedBlocks;
}

ProjectHandler* Project::getProjectHandler(){
	return this->projectHandler;
}

list<SchedBlock*> Project::getPendingSchedBlocks(){	
	list<SchedBlock*> p;
	
	for(	list<SchedBlock *>::iterator it = schedBlocks.begin();
			it != schedBlocks.end();
			it++)
	{
		if(!(*it)->isExecuted())
		{
			p.push_back(*it);
		}
	}
	
	return p;
}

SchedBlock* Project::getSchedBlock(unsigned int schedBlockIndex){
	list<SchedBlock*> p;
	
	for(	list<SchedBlock *>::iterator it = schedBlocks.begin();
			it != schedBlocks.end();
			it++)
	{
		if(!(*it)->getIndex() == schedBlockIndex)
		{
			return *it;
		}
	}
	
	return (SchedBlock*)NULL;
}

void Project::addSchedBlock(
		unsigned int schedBlockIndex,
		int seqBlockIndex,
		time_t execTime,
		double obsFrequency,
		double RA, double Dec, double LTSr, double LTSs)
{
	SchedBlock* newSchedBlock = new SchedBlock(
		this,
		schedBlockIndex,seqBlockIndex,execTime,obsFrequency,
		RA,Dec,LTSr,LTSs);
	newSchedBlock->registerObserver(this);
	schedBlocks.push_back(newSchedBlock);
	numSchedBlocks++;
}


void Project::notify(Observable* o)
{
	// Notification by SchedBlock
	SchedBlock* schedBlockSubject = dynamic_cast<SchedBlock*>(o);
	if (schedBlockSubject != NULL) {
//		cout << "-- Project received SchedBlock notification." << endl;
		double newRemainingExec = double(getPendingSchedBlocks().size()) / double(numSchedBlocks);
		if (newRemainingExec != this->remainingExec) {
			this->remainingExec = newRemainingExec;
			this->notifyObservers();
		}
		return;
	}

	cerr << "ERROR: Project received notification from unknown subject." << endl;
}

