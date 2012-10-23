#include "classes.hpp"

using namespace std;


ShortTermTime::ShortTermTime():
	projectHandler(NULL),
	longTermTime(NULL),
	weatherHandler(NULL),
    sbListSize(0U),
    sbListNum(0U)
{
	//Nothing to do here
}

ShortTermTime::~ShortTermTime(){
	if (longTermTime != NULL)
		delete longTermTime;

	if (weatherHandler != NULL)
		delete weatherHandler;

	while (!timeSlots.empty()) {
		TimeSlot* oldTimeSlot = timeSlots.front();
		timeSlots.pop_front();
		delete oldTimeSlot;
	}

	if (projectHandler != NULL)
		delete projectHandler;
}

void ShortTermTime::initializePeriod(){
	// Initialize everything else.
	initializeProjectHandler();
	initializeWeatherHandler();
	initializeLongTermTime();
	initializeShortTermTime();
}

TimeSlot* ShortTermTime::nextTimeSlot(){
	//Shift timeSlots list.
	this->currentSlotIndex++;
	if (this->currentSlotIndex > 0) {
		// Don't pop if period is just starting
		addTimeSlot();
//		cout << "Removing ST time slot " << timeSlots.front()->getIndex() << endl;
		TimeSlot* oldTimeSlot = timeSlots.front();
		this->unregisterObserver(oldTimeSlot);
		timeSlots.pop_front();
		delete oldTimeSlot;
		//TODO: Also unregister observed SBs from front LongTermTimeSlot
		// to avoid useless future calculations?
	}
	//If list is empty, period has ended.
	if (timeSlots.empty())
		return (TimeSlot*)NULL;

	// Increase totalAvailableTime if TimeSlot available for executions
	if(timeSlots.front()->isAvailable())
		this->totalAvailableTime += this->slotDuration;
	else
		cout << "Unavailable time slot: " << timeSlots.front()->getStartDate() << endl;

	// Notify observers
	this->notifyObservers();

	// Save list length if >0
	unsigned int listSize = timeSlots.front()->getSBListSize();
	if (listSize > 0) {
		sbListSize += listSize;
		sbListNum++;
		cout << "Short term SB list size: " << listSize << "/" << timeSlots.front()->getLongTermTimeSlot()->getSBListSize() << endl;
	}

	// Return first element of timeSlots, which is now the current slot
	return timeSlots.front();
}

TimeSlot* ShortTermTime::getTimeSlot(unsigned int ts){
	list<TimeSlot*>::iterator it;
	for(it = timeSlots.begin(); it != timeSlots.end(); it++)
		if ((*it)->getIndex() == ts)
			return (*it);
	// Index not found. Return NULL.
	cerr << "WARNING: Time slot index not found: "<< ts << endl;
	return (TimeSlot*)NULL;
}

time_t ShortTermTime::getTotalAvailableTime(){
	return this->totalAvailableTime;
}

ProjectHandler* ShortTermTime::getProjectHandler(){
	return this->projectHandler;
}

unsigned int ShortTermTime::getAvgeShortTermSBListSize()
{
	return double(sbListSize) / double(sbListNum) + 0.5;
}

unsigned int ShortTermTime::getAvgeLongTermSBListSize()
{
	return longTermTime->getAvgeSBListSize();
}

void ShortTermTime::initializeProjectHandler(){
	this->projectHandler = new ProjectHandler();

	string dataSource = "../input/"; //TODO: Get this from somewhere
	this->projectHandler->initialize(dataSource);
}

void ShortTermTime::initializeLongTermTime(){
	this->longTermTime = new LongTermTime();

	time_t slotDuration = 24*60*60; //TODO Get this from somewhere (considering 24 hours)
	this->longTermTime->initialize(this->projectHandler, slotDuration);
}

void ShortTermTime::initializeWeatherHandler(){
	this->weatherHandler = new WeatherHandler();

	string dataSource = "../input/"; //TODO: Get this from somewhere
	time_t resolution = 60*60; //TODO: Get this from somewhere (considering 1 hour)
	this->weatherHandler->initialize(dataSource, projectHandler->getStartDate(), resolution);
}

void ShortTermTime::initializeShortTermTime(){
	this->slotDuration = 60*60; //TODO: Get this from somewhere (considering 1 hour)
	this->numSlots = 5; //TODO: Get this from somewhere
	cout << "Will initialize " << numSlots << " short term time slots of duration " << slotDuration << "[sec]" << endl;

	//Initialize timeSlots list with first numSlots instances
	for (unsigned int i=0; i<numSlots; i++)
		addTimeSlot();

	// Set initial slot index and available time
	// considering execution has not yet started
	this->currentSlotIndex = -1;
	this->totalAvailableTime = 0L;
}

void ShortTermTime::addTimeSlot(){
	unsigned int index(0U);
	time_t startDate(0L);
	time_t endDate(0L);

	// Look for last created timeSlot as start point
	if (timeSlots.back() != *(timeSlots.end())) {
		startDate = timeSlots.back()->getEndDate() + 1;
		index = timeSlots.back()->getIndex() + 1;
	} else {
		// If first time slot
		startDate = projectHandler->getStartDate();
	}
	endDate = startDate + slotDuration - 1;

	// Skip if endDate would be after period end date
	if (endDate > projectHandler->getEndDate())
		return;

//	cout << "Adding new ST time slot: " << index << " " << startDate << " " << endDate << endl;

	// Create new TimeSlot instance
	TimeSlot* newTimeSlot = new TimeSlot(weatherHandler, longTermTime, projectHandler, index, startDate, endDate);

	// Register to observe for changes
	this->registerObserver(newTimeSlot);

	// Insert as last element of timeSlots
	timeSlots.push_back(newTimeSlot);
}

