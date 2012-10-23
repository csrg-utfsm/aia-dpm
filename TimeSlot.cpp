#include "classes.hpp"

using namespace std;

TimeSlot::TimeSlot(
	WeatherHandler* w,
	LongTermTime* ltt,
	ProjectHandler* p,
	unsigned int index,
	time_t start,
	time_t end)
{
	this->slotIndex = index;
	this->startDate = start;
	this->endDate = end;

	// Weather association
	this->weatherHandler = w;
	this->updateWeatherCondition();

	// Long Term Time association
	this->longTermTime = ltt;
	this->longTermTimeSlot = this->longTermTime->getTimeSlot(startDate);
	this->availability = this->longTermTimeSlot->isAvailable(); //Initial availability

	// Initialize execPriority and execSuccessProbability maps
	list<SchedBlock*> sbs = this->longTermTimeSlot->getFeasibleSchedBlocks();
	list<SchedBlock*>::iterator it;
	for (it=sbs.begin(); it!=sbs.end(); it++) {
		// Skip if not visible in this time interval
		if (!(*it)->isVisible(this->startDate, this->endDate))
			continue;
		this->futureFeasibility.insert(make_pair(*it,calculateFutureFeasibility(*it)));
		this->execSuccessProbability.insert(make_pair(*it,calculateSuccessProbability(*it)));
		this->execPriority.insert(make_pair(*it,calculateExecPriority(*it)));
		(*it)->getProject()->registerObserver(this);
	}
}

TimeSlot::~TimeSlot()
{
	// Unregister from observed Projects
	map<SchedBlock*,double>::iterator it;
	for (it=futureFeasibility.begin(); it!=futureFeasibility.end(); it++)
		it->first->getProject()->unregisterObserver(this);
}

bool TimeSlot::isAvailable()
{
	return this->availability;
}

void TimeSlot::setAvailability(bool availability)
{
	// This can never be True if longTermTimeSlot->availability is False
	this->availability = longTermTimeSlot->isAvailable() && availability;
}

map<SchedBlock*,double> TimeSlot::getPriorityList()
{
	return this->execPriority;
}

//TODO: To make this request cheaper (now O(n)), consider a more efficient search algorithm.
//      (same for all other linear searches)
SchedBlock* TimeSlot::getTopPriority()
{
	// Return NULL if TimeSlot not available or priority list empty
	if (!availability || execPriority.empty())
		return (SchedBlock*)NULL;

	// Search for the highest priority in execPriority map.
	// If 2 priorities are equal (conflict), go for the highest SciGrade.
	map<SchedBlock*, double>::iterator maxSb = execPriority.begin();
	for(map<SchedBlock*, double>::iterator it = execPriority.begin(); it != execPriority.end(); ++it) {
		if ( ((*it).second > (*maxSb).second) ||
				((*it).second == (*maxSb).second &&
				(*it).first->getProject()->getSciGrade() > (*maxSb).first->getProject()->getSciGrade()) )
			maxSb = it;
	}

	// Return NULL if top priority is zero
	if ((*maxSb).second == 0.0)
		return (SchedBlock*)NULL;

	cout << "Current top priority: " << (*maxSb).first->getProject()->getIndex()
		<< "," << (*maxSb).first->getIndex()
		<< " " << (*maxSb).second
		<< " (SciGrade: " << (*maxSb).first->getProject()->getSciGrade() << ")"
		<< endl;
	return (*maxSb).first;
}

time_t TimeSlot::getStartDate()
{
	return this->startDate;
}

time_t TimeSlot::getEndDate()
{
	return this->endDate;
}

unsigned int TimeSlot::getIndex()
{
	return this->slotIndex;
}

LongTermTimeSlot* TimeSlot::getLongTermTimeSlot()
{
	return this->longTermTimeSlot;
}

void TimeSlot::updateWeatherCondition()
{
	this->currentWeather = weatherHandler->getWeatherCondition(startDate, endDate);
	this->futureWeather = weatherHandler->getWeatherCondition(endDate+1, endDate+(endDate-startDate));
}

double TimeSlot::calculateSuccessProbability(SchedBlock* sb)
{
	//Calculate atmospheric data and Tsys
	atmosphericData data = weatherHandler->getFrequencyParameters(sb->getObservationFrequency(), currentWeather.pwv);
	double tsys = weatherHandler->getSystemTemperature(sb->getRA(), sb->getDec(), data);

	//Calculate projected atmospheric data and Tsys
	atmosphericData pdata = weatherHandler->getFrequencyParameters(sb->getObservationFrequency(), futureWeather.pwv);
	double ptsys = weatherHandler->getSystemTemperature(sb->getRA(), sb->getDec(), pdata);

	// Calculate Tsys variation
	double tsysVariation = abs((ptsys - tsys) / tsys);

//	cout << "** " << data.pwv << " " << data.frequency << " " << data.opacity << " " << data.temperature << endl;
//	cout << "** " << pdata.pwv << " " << pdata.frequency << " " << pdata.opacity << " " << pdata.temperature << endl;
//	cout << "** DELTA Tsys: " << tsysVariation << " (" << tsys << " / " << ptsys << ")" << endl;

	// Calculate execution probability according to Tsys variation (stability)
	// if tsysVariation >= 0.15 -> 0.0
	// if tsysVariation  = 0.0  -> 1.0
	// else -> intermediate value
	double probability = 0.0;
	if (tsysVariation < 0.15)
		probability = (0.15 - tsysVariation) / 0.15;

//	cout << "** PROBABILITY: " << probability << endl;
	return probability;
}

double TimeSlot::calculateFutureFeasibility(SchedBlock* sb)
{
	return this->longTermTime->getFutureFeasibilities(sb, this->longTermTimeSlot->getIndex());
}

double TimeSlot::calculateExecPriority(SchedBlock* schedBlock)
{
	// Get exec success probability
	map<SchedBlock*,double>::iterator it;
	double probability(0.0);
	it = execSuccessProbability.find(schedBlock);
	if (it != execSuccessProbability.end())
		probability = it->second;

	// Get future feasibility
	double feasibility(0.0);
	it = futureFeasibility.find(schedBlock);
	if (it != futureFeasibility.end())
		feasibility = it->second;

	// Calculate priority
	double priority = double(
			schedBlock->getProject()->getSciGrade()
			* schedBlock->isAvailable()
			* probability
			/ schedBlock->getProject()->getRemainingExec()
			/ feasibility
		);
//	cout << ">> Priority: "
//		<< schedBlock->getProject()->getSciGrade() << " "
//		<< schedBlock->isAvailable() << " "
//		<< probability << " "
//		<< schedBlock->getProject()->getRemainingExec() << " "
//		<< feasibility << " "
//		<< endl;

	return priority;
}

unsigned int TimeSlot::getSBListSize()
{
	return this->execPriority.size();
}

void TimeSlot::notify(Observable* o)
{
	// Notification by ShortTermTime
	ShortTermTime* timeSlotSubject = dynamic_cast<ShortTermTime*>(o);
	if (timeSlotSubject != NULL) {
//		cout << "-- TimeSlot received ShortTermTime notification." << endl;
		// Update weather condition
		weatherCondition oldWeather = currentWeather;
		this->updateWeatherCondition();

		// Re-calculate success probabilities (only necessary if weather changed)
		if (currentWeather.temperature != oldWeather.temperature
				&& currentWeather.humidity != oldWeather.humidity
				&& currentWeather.pwv != oldWeather.pwv) {
			map<SchedBlock*,double>::iterator it;
			for (it=execPriority.begin(); it!=execPriority.end(); it++) {
				execSuccessProbability[it->first] = calculateSuccessProbability(it->first);
				execPriority[it->first] = calculateExecPriority(it->first);
			}
		}
		return;
	}

	// Notification by Project
	Project* projectSubject = dynamic_cast<Project*>(o);
	if (projectSubject != NULL) {
//		cout << "-- TimeSlot received Project notification." << endl;
		// Update SBs belonging to projectSubject in execPriority and execSuccessPriority lists
		list<SchedBlock*> updatedSchedBlocks = projectSubject->getSchedBlocks();
		list<SchedBlock*>::iterator it;
		for (it=updatedSchedBlocks.begin(); it!=updatedSchedBlocks.end(); it++) {
			map<SchedBlock*,double>::iterator espIt = execSuccessProbability.find(*it);
			map<SchedBlock*,double>::iterator epIt = execPriority.find(*it);

			// Skip to next SB if not in maps
			if (espIt == execSuccessProbability.end() || epIt == execPriority.end())
				continue;

			// Remove from maps if executed
			if ((*it)->isExecuted()) {
				execPriority.erase(epIt);
				execSuccessProbability.erase(espIt);
				continue;
			}

			// Update priority
			execPriority[*it] = calculateExecPriority(*it);
		}
		return;
	}

	cerr << "ERROR: TimeSlot received notification from unknown subject." << endl;
}

