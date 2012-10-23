#include "classes.hpp"

using namespace std;

LongTermTime::LongTermTime(){
	//Nothing to do here
}

LongTermTime::~LongTermTime(){
	while (!timeSlots.empty()) {
		LongTermTimeSlot* oldTimeSlot = timeSlots.back();
		timeSlots.pop_back();
		delete oldTimeSlot;
	}
}

void LongTermTime::initialize(ProjectHandler* projHandler, time_t duration)
{
	this->slotDuration = duration;
	this->numSlots = ceil(double(difftime(projHandler->getEndDate(), projHandler->getStartDate())) / double(slotDuration));
	cout << "Will create " << numSlots << " long term time slots of duration " << slotDuration  << "[sec]"<< endl;

	//TODO: Unavailable TS should be taken from somewhere else
	// (setting February 2012 as unavailable as defined for current test data set).
	char buffer[80], buffer2[80];
	time_t startSkip = 1328065200;
	time_t endSkip = 1330570799;
	strftime(buffer,80,"%Y-%m-%dT%H:%M:%S",localtime(&startSkip));
	strftime(buffer2,80,"%Y-%m-%dT%H:%M:%S",localtime(&endSkip));
	cout << "Setting unavailable period: " << buffer << " to " << buffer2 << endl;

	time_t endDate = projHandler->getStartDate() - 1;
	unsigned int sbListSize = 0;
	unsigned int sbListCount = 0;
	unsigned int index;
	for (index=0U; index<numSlots; index++) {
		time_t startDate = endDate + 1;
		endDate = startDate + slotDuration - 1;
		bool available = true;
		// Setting unavailable time slots
		if ((startDate >= startSkip && startDate <= endSkip) || (endDate>= startSkip && endDate <= endSkip))
			available = false;
//		cout << "Adding new LT time slot: " << index << " " << startDate << " " << endDate << endl;
		LongTermTimeSlot* newTimeSlot =
			new LongTermTimeSlot(projHandler, index, startDate, endDate, available);
		// Save list length if >0
		unsigned int listSize = newTimeSlot->getSBListSize();
		if (listSize > 0) {
			sbListSize += listSize;
			sbListCount++;
			cout << "Long term SB list size: " << listSize << endl;
		}
		// Insert into feasibility list
		this->timeSlots.push_back(newTimeSlot);
	}
	avgeSBListSize = double(sbListSize) / double(sbListCount) + 0.5;
}

LongTermTimeSlot* LongTermTime::getTimeSlot(time_t time){
	vector<LongTermTimeSlot*>::iterator it;
	for(it = timeSlots.begin(); it != timeSlots.end(); ++it) {
		if((*it)->getStartDate() <= time && (*it)->getEndDate() >= time)
			return *it;
	}

	// Index not found. Return NULL pointer.
	cerr << "WARNING: No LongTermTimeSlot found for time " << time << endl;
	return (LongTermTimeSlot*)NULL;
}

double LongTermTime::getFutureFeasibilities(SchedBlock* schedBlock, unsigned int longTermIndex){
	unsigned int count = 0U;
	unsigned int totalCount = 0U;
	vector<LongTermTimeSlot*>::iterator it = timeSlots.begin() + longTermIndex;
	for (it++; it < timeSlots.end(); it++) {
		if((*it)->getFeasibility(schedBlock))
			count++;
		totalCount++;
	}
	// If no more TS available, return 1
	// (this is irrelevant as all SBs will return the same at this time)
	if (totalCount == 0U)
		return 1.0;
	// If not feasible in more TSs return a number smaller than 1 / totalCount
	if (count == 0U)
		return 1.0 / double(totalCount+1);

	return double(count) / double(totalCount);
}

unsigned int LongTermTime::getAvgeSBListSize(){
	return avgeSBListSize;
}

