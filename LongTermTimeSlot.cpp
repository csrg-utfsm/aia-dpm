#include "classes.hpp"

using namespace std;

LongTermTimeSlot::LongTermTimeSlot(ProjectHandler *p, unsigned int index, time_t startDate, time_t endDate, bool available)
{
	this->projectHandler = p;
	this->longTermSlotIndex = index;
	this->startDate = startDate;
	this->endDate = endDate;
	this->availability = available;

	// Stop here if time slot unavailable.
	if (!this->availability)
		return;

	// Initialize execFeasibility map
	list<Project*> projects = this->projectHandler->getPendingProjects();
	for(list<Project*>::iterator i = projects.begin(); i != projects.end(); ++i) {
		list<SchedBlock*> s = (*i)->getPendingSchedBlocks();
		for(list<SchedBlock*>::iterator j = s.begin(); j != s.end(); ++j) {
			if(LongTermTimeSlot::calculateFeasibility(*j)) {
				execFeasibility.push_back(*j);
				(*j)->registerObserver(this);
			}
		}
	}
}

LongTermTimeSlot::~LongTermTimeSlot()
{
	// Unregister from observed SchedBlocks
	//TODO: This is missing schedBlocks that were executed in the meantime.
	list<SchedBlock*>::iterator it;
	for (it=execFeasibility.begin(); it!=execFeasibility.end(); it++)
		(*it)->unregisterObserver(this);
}

bool LongTermTimeSlot::isAvailable(){
	return this->availability;
}

list<SchedBlock*> LongTermTimeSlot::getFeasibleSchedBlocks(){
	return this->execFeasibility;
}

bool LongTermTimeSlot::getFeasibility(SchedBlock* schedBlock){	
	list<SchedBlock*>::iterator it;
	for(it = execFeasibility.begin(); it != execFeasibility.end(); ++it) {
		if((*it) == schedBlock)
			return true;
	}
	return false;
}

time_t LongTermTimeSlot::getStartDate(){
	return this->startDate;
}

time_t LongTermTimeSlot::getEndDate(){
	return this->endDate;
}

unsigned int LongTermTimeSlot::getIndex(){
	return longTermSlotIndex;
}

bool LongTermTimeSlot::calculateFeasibility(SchedBlock* sb){
	// In this case, it is sufficient if sources are visible at some
	// point of the interval (not the entire one).
	time_t start;
	time_t duration = 60*60; //TODO: Get this from somewhere (considering 1 hour intervals)
	for (start=this->startDate; start+duration-1 <= this->endDate; start+=duration)
		if (sb->isVisible(start, start+duration-1))
			return true;

	return false;
}

unsigned int LongTermTimeSlot::getSBListSize()
{
	return this->execFeasibility.size();
}

void LongTermTimeSlot::notify(Observable* o)
{
	// Notification by SchedBlock
	SchedBlock* schedBlockSubject = dynamic_cast<SchedBlock*>(o);
	if (schedBlockSubject != NULL) {
//		cout << "-- LongTermTimeSlot received SchedBlock notification." << endl;
		// Remove executed SchedBlock from feasibility list
		if (schedBlockSubject->isExecuted()) {
//			schedBlockSubject->unregisterObserver(this);
			execFeasibility.remove(schedBlockSubject);
		}
		return;
	}

	cerr << "ERROR: LongTermTimeSlot received notification from unknown subject." << endl;
}

