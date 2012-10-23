#include "classes.hpp"

using namespace std;

// Helper function to calculate time diff in milliseconds.
static time_t getDiffMillisec(timeval end, timeval start)
{
	return double(end.tv_sec-start.tv_sec) * 1000.0 + double(end.tv_usec-start.tv_usec) / 1000.0 + 0.5;
}


Scheduler::Scheduler() :
	shortTermTime(NULL),
	projectHandler(NULL),
	currentTimeSlot(NULL),
	currentTime(0L),
	nextSBTime(0L),
	nextSBCount(0U),
	executeTime(0L),
	executeCount(0U),
	nextTimeSlotTime(0L),
	nextTimeSlotCount(0U)
{
	//Nothing to do.
}

Scheduler::~Scheduler()
{
	if (shortTermTime != NULL)
		delete shortTermTime;
}

void Scheduler::initializePeriod()
{
	cout << "SCHEDULER: Initializing current period." << endl;

	// Initialize short term time
	shortTermTime = new ShortTermTime();
	shortTermTime->initializePeriod();

	//Get a reference to ProjectHandler instance
	projectHandler = shortTermTime->getProjectHandler();
}

void Scheduler::runSchedule()
{
	cout << "SCHEDULER: Starting scheduling process." << endl;

	currentTime = projectHandler->getStartDate();
//	srand(currentTime);
	while (currentTime < projectHandler->getEndDate()) {
//		//Toggling random time slot availabilities. TODO: Define as optional
//		if (currentTimeSlot != NULL) {
//			int toggleIndex = currentTimeSlot->getIndex() + rand()%5;
//			TimeSlot* toggleSlot = shortTermTime->getTimeSlot(toggleIndex);
//			if (toggleSlot != NULL) {
//				cout << "SCHEDULER: Toggling availability of time slot " << toggleSlot->getIndex() << endl;
//				toggleSlot->setAvailability(!toggleSlot->isAvailable());
//			} else {
//				cerr << "Current time slot: " << currentTimeSlot->getIndex() << endl;
//			}
//		}

		if (!nextTimeSlot()) {
			// No new time slots available
			break;
		}
		while (currentTime < currentTimeSlot->getEndDate()) {
			SchedBlock* schedBlock = nextSB();
			if (schedBlock != NULL) {
				if (!execute(schedBlock))
					// Some sort of error while executing SB
					break;
			} else {
				// No other SB available for current timeSlot
				cout << "SCHEDULER: No more SchedBlocks available for current time slot." << endl;
				break;
			}
		}
		if (currentTime < currentTimeSlot->getEndDate()) {
			waitUntil(currentTimeSlot->getEndDate());
		}
	}
	if (currentTime < (projectHandler->getEndDate() - 1)) {
		cout << "SCHEDULER: Remaining period time:"
			<<  difftime(projectHandler->getEndDate(), currentTime) << endl;
		waitUntil(projectHandler->getEndDate());
	}

	cout << "SCHEDULER: Scheduling process completed." << endl;
}

double Scheduler::calculateTotalThroughput()
{
	double throughput(0.0);
	list<Project*> projects = projectHandler->getCompletedProjects();
	list<Project*>::iterator it;
	for (it=projects.begin(); it!=projects.end(); it++)
		throughput += (*it)->getSciGrade();

	double possibleThroughput = throughput;
	list<Project*> pendingProjects = projectHandler->getPendingProjects();
	list<Project*>::iterator it2;
	for (it2=pendingProjects.begin(); it2!=pendingProjects.end(); it2++)
		possibleThroughput += (*it2)->getSciGrade();

	return throughput / possibleThroughput;
}

double Scheduler::calculateTotalTimeUsage()
{
	time_t execTime(0L);
	time_t availableTime = shortTermTime->getTotalAvailableTime();

	map<time_t,SchedBlock*>::iterator it;
	for (it=executionOrder.begin(); it!=executionOrder.end(); it++)
		execTime += it->second->getExecTime();

	return double(execTime) / double(availableTime);
}

double Scheduler::calculateExecutedSchedBlocks()
{
	unsigned int numSchedBlocks(0U);
	list<Project*> projects = projectHandler->getProjects();
	list<Project*>::iterator it;

	for (it=projects.begin(); it != projects.end(); it++)
		numSchedBlocks += (*it)->getSchedBlocks().size();

	return double(executionOrder.size()) / double(numSchedBlocks);
}

map<time_t,SchedBlock*> Scheduler::getExecutionOrder()
{
	return executionOrder;
}

list<Project*> Scheduler::getPendingProjects()
{
	return projectHandler->getPendingProjects();
}

double Scheduler::getAvgeNextSBTime()
{
	return double(nextSBTime) / double(nextSBCount);
}

double Scheduler::getAvgeExecuteTime()
{
	return double(executeTime) / double(executeCount);
}

double Scheduler::getAvgeNextTimeSlotTime()
{
	return double(nextTimeSlotTime) / double(nextTimeSlotCount);
}

unsigned int Scheduler::getAvgeShortTermSBListSize()
{
	return shortTermTime->getAvgeShortTermSBListSize();
}

unsigned int Scheduler::getAvgeLongTermSBListSize()
{
	return shortTermTime->getAvgeLongTermSBListSize();
}

/*Private methods*/

SchedBlock* Scheduler::nextSB()
{
	timeval start, end;
	gettimeofday(&start, NULL);

	SchedBlock* topPriority = currentTimeSlot->getTopPriority();

	gettimeofday(&end, NULL);
	time_t diff = getDiffMillisec(end, start);
	nextSBTime += diff;
	nextSBCount++;

	return topPriority;
}

bool Scheduler::execute(SchedBlock* schedBlock)
{
	if (schedBlock->isExecuted()) {
		cerr << "ERROR: SchedBlock " << schedBlock->getProject()->getIndex()
			<< "," << schedBlock->getIndex() << " was already executed. Skipping to next TimeSlot."
			<< endl;
		return false;
	}
	cout << ">> Executing SchedBlock: " << schedBlock->getProject()->getIndex()
		<< "," << schedBlock->getIndex()
		<< " " << double(schedBlock->getExecTime())/60.0 << "[min] "
		<< currentTime << endl;

	timeval start, end;
	gettimeofday(&start, NULL);

	schedBlock->setExecuted();
	executionOrder.insert(make_pair(currentTime,schedBlock));
	waitUntil(currentTime + schedBlock->getExecTime());

	gettimeofday(&end, NULL);
	time_t diff = getDiffMillisec(end, start);
	executeTime += diff;
	executeCount++;

	return true;
}

bool Scheduler::nextTimeSlot()
{
	cout << "SCHEDULER: Moving to next time slot." << endl;

	timeval start, end;
	gettimeofday(&start, NULL);

	currentTimeSlot = shortTermTime->nextTimeSlot();

	// If currentTimeSlot is NULL, period has ended.
	if (currentTimeSlot == NULL) {
		cout << "SCHEDULER: No more time slots available." << endl;
		return false;
	}

	gettimeofday(&end, NULL);
	time_t diff = getDiffMillisec(end, start);
	nextTimeSlotTime += diff;
	nextTimeSlotCount++;

	cout << "SCHEDULER: Current time slot: " << currentTimeSlot->getIndex()
	<< " (" << currentTimeSlot->getLongTermTimeSlot()->getIndex() << ") "
	<< currentTime << endl;

	return true;
}

void Scheduler::setTimeSlotAvailability(unsigned int slotIndex, bool availability)
{
	TimeSlot* timeSlot = shortTermTime->getTimeSlot(slotIndex);
	if (timeSlot == NULL or timeSlot == currentTimeSlot)
		//Skip if empty reference or time slot currently under execution
		return;

	if (timeSlot->isAvailable() != availability) {
		cout << "SCHEDULER: Setting availability of time slot " << slotIndex
		<< " to " << availability << endl;
		timeSlot->setAvailability(availability);
	}
}

void Scheduler::waitUntil(time_t time)
{
	//long diff = long(difftime(time, currentTime));
//	cout << "SCHEDULER: Sleeping for " << diff << endl;
	//usleep(diff); //Sleep one micro-second for each real second
	currentTime = time;
}


/* MAIN */

int main(int argc, const char* argv[])
{
	// Getting input parameter
	if (argc > 2) {
		cerr << "Usage: ./aia-dpm [output_file]" << endl;
		return 1;
	}
	string filename;
	if (argc == 2)
		filename = argv[1];
	else
		filename = "output.txt";

	Scheduler* scheduler = NULL;
	try {
		// Initializing and executing the observation period
		timeval start, end;

		gettimeofday(&start, NULL);
		scheduler = new Scheduler();
		scheduler->initializePeriod();
		gettimeofday(&end, NULL);
		time_t initTime = getDiffMillisec(end, start);

		gettimeofday(&start, NULL);
		scheduler->runSchedule();
		gettimeofday(&end, NULL);
		time_t execTime = getDiffMillisec(end, start);

		// Getting execution data
		gettimeofday(&start, NULL);
		double throughput = scheduler->calculateTotalThroughput();
		double timeUsage = scheduler->calculateTotalTimeUsage();
		double totalSbs = scheduler->calculateExecutedSchedBlocks();
		double avgeNextSBTime = scheduler->getAvgeNextSBTime();
		double avgeExecuteTime = scheduler->getAvgeExecuteTime();
		double avgeNextTimeSlotTime = scheduler->getAvgeNextTimeSlotTime();
		unsigned int avgeShortTermSBListSize = scheduler->getAvgeShortTermSBListSize();
		unsigned int avgeLongTermSBListSize = scheduler->getAvgeLongTermSBListSize();
		map<time_t,SchedBlock*> executionOrder = scheduler->getExecutionOrder();
		gettimeofday(&end, NULL);
		time_t finalTime = getDiffMillisec(end, start);

		// Prepare output file
		ofstream outputFile;
		outputFile.open(filename.c_str(), ios::out);
		if (!outputFile.is_open()) {
			cerr << "Error opening output file " << filename << endl;
			if (scheduler != NULL)
				delete scheduler;
			return 1;
		}

		// Print statistics
		outputFile << "-------------------------------------------------------" << endl;
		outputFile << "Total scientific throughput rate: " << throughput << endl;
		outputFile << "Total time usage rate: " << timeUsage << endl;
		outputFile << "Total SBs execution rate: " << totalSbs << endl;
		outputFile << endl;
		outputFile << "Average initial short term SB list size: " << avgeShortTermSBListSize << endl;
		outputFile << "Average initial long term SB list size: " << avgeLongTermSBListSize << endl;
		outputFile << endl;
		outputFile << "Average next SB processing time: " << avgeNextSBTime << "[msec]" << endl;
		outputFile << "Average execute SB processing time: " << avgeExecuteTime << "[msec]" << endl;
		outputFile << "Average next time slot processing time: " << avgeNextTimeSlotTime << "[msec]" << endl;
		outputFile << endl;
		outputFile << "Initialization time: " << initTime/1000.0 << "[sec]" << endl;
		outputFile << "Period execution time: " << execTime/1000.0 << "[sec]" << endl;
		outputFile << "Finalization time: " << finalTime/1000.0 << "[sec]" << endl;
		outputFile << "-------------------------------------------------------" << endl;

		// Print execution order
		char buffer[80];
		map<time_t,SchedBlock*>::iterator it;
		for (it=executionOrder.begin(); it!=executionOrder.end(); it++) {
			//outputFile << it->first << " "
			strftime(buffer,80,"%Y-%m-%dT%H:%M:%S ",localtime(&(it->first)));
			outputFile << buffer
				<< it->second->getProject()->getIndex() << ","
				<< it->second->getIndex() << endl;
		}
		outputFile << "-------------------------------------------------------" << endl;

		// Print pending SBs
		list<Project*> pendingProj = scheduler->getPendingProjects();
		list<Project*>::iterator itProj;
		for (itProj=pendingProj.begin(); itProj!=pendingProj.end(); itProj++) {
			list<SchedBlock*> pendingSB = (*itProj)->getPendingSchedBlocks();
			list<SchedBlock*>::iterator itSB;
			for (itSB=pendingSB.begin(); itSB!=pendingSB.end(); itSB++)
				outputFile << (*itProj)->getIndex() << "," << (*itSB)->getIndex()
					<< " (" << (*itProj)->getSciGrade() << ")" << endl;
		}
		outputFile << "-------------------------------------------------------" << endl;

		// Cleanup
		outputFile.close();
		delete scheduler;
	} catch (...) {
		cerr << "ERROR: Something went wrong! Aborting." << endl;
		if (scheduler != NULL)
			delete scheduler;
		return 1;
	}
	return 0;
}

