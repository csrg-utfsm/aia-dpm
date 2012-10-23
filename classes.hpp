#ifndef ALG_CLASSES_H
#define ALG_CLASSES_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <map>
#include <ctime>
#include <cmath>
#include <sys/time.h>
#include <sys/stat.h>
#include <XmlReader.hpp>
#include <WeatherReader.hpp>

using namespace std;

class LongTermTimeSlot;
class LongTermTime;
class ShortTermTime;
class TimeSlot;
class WeatherData;
class WeatherHandler;
class Scheduler;
class ProjectHandler;
class Project;
class SchedBlock;
class Observer;
class Observable;

/* Structure to pass weather information refering to a condition within
 * a time frame among classes.
 */
typedef struct weatherCondition {
	time_t startTime;
	time_t endTime;
	double temperature;
	double humidity;
	double pwv;
} weatherCondition;

/* Structure to pass atmospheric data associated to a PWV and a Frequency
 * among classes.
 */
typedef struct atmosphericData {
	double pwv;
	double frequency;
	double opacity;
	double brightnessTemperature;
} atmosphericData;

/* Definition of available weather condition parameters.
 */
enum weatherParameter {TEMPERATURE, HUMIDITY};

/* Definition of invalid data value. This is used for invalid weather
 * conditions and atmospheric parameters, as well as for invalid coordinates.
 */
#define INVALID_DATA -999.0


/*
 * Observer class needs to be inherited by any class to be notified by an
 * Observable class.
 */
class Observer
{
  public:
	virtual ~Observer() {};

	// Notify this class of a change in an Observed class
	virtual void notify(Observable*) = 0;
};

/*
 * Observable needs to be inherited by any class that needs to notify Observer
 * classes of internal changes.
 */
class Observable
{
  public:
	virtual ~Observable() {};

	// Register a new Observer of this class, while avoiding duplications
	void registerObserver(Observer* o) {
		observers.remove(o);
		observers.push_back(o);
	}

	// Unregister an Observer of this class
	void unregisterObserver(Observer* o) {
		observers.remove(o);
	}

  protected:
	list<Observer*> observers;

	// Notify all Observers of this class
	void notifyObservers() {
		list<Observer*>::iterator it;
		for (it = observers.begin(); it != observers.end(); ++it)
			(*it)->notify(this);
	}
};


/* Long-term time slot representation, as discretization of the static long term
 * planning. It associates feasible SchedBlock instances.
 *
 * OBSERVER: LongTermTimeSlot observes SchedBlock for changes in status
 * attribute, to update its feasibility map by removing the executed SB.
 */
class LongTermTimeSlot : public Observer
{
  public:
	/*constructor*/
	// Parameters:
	// * ProjectHandler reference
	// * Slot index
	// * Start date
	// * End date
	LongTermTimeSlot(ProjectHandler*, unsigned int, time_t, time_t, bool);

	/*destructor*/
	~LongTermTimeSlot();

	/*interface methods*/
	// Return availability of this time slot to execute time-slots; if
	// unavailable no SBs can be scheduled to be executed during this time slot;
	// availability is only set during initialization and cannot be changed
	// afterwards
	bool isAvailable();

	// Return all feasible SBs for this time slot; this list
	// could evetually be empty, if the time slot is unavailable for
	// observations, or if no SBs are feasible in this time slot (should
	// hopefully never happen)
	list<SchedBlock*> getFeasibleSchedBlocks();

	// Caculate feasibility of given SchedBlock, considering only static parameters.
	bool getFeasibility(SchedBlock*);

	// Return the time slot's start date
	time_t getStartDate();

	// Return the time slot's end date
	time_t getEndDate();

	// Return slot index
	unsigned int getIndex();

	// Return size of feasibility list
	unsigned int getSBListSize();

	// Update callback method
	virtual void notify(Observable*);

  private:
	unsigned int longTermSlotIndex;
	time_t startDate;
	time_t endDate;
	bool availability;
	ProjectHandler* projectHandler;
	list<SchedBlock*> execFeasibility;

	// Calculate the feasibility for the given SB
	bool calculateFeasibility(SchedBlock*);
};

/* Coordinator class for LongTermTimeSlot instances. It handles feasible
 * subgroups of SchedBlock instances, in order to reduce the search space.
 */
class LongTermTime
{
  public:
	/*constructor*/
	LongTermTime();

	/*destructor*/
	~LongTermTime();

	/*interface methods*/
	// Initialize all time slots for the current period
	// Parameters:
	// * ProjectHandler reference
	// * LT time slots duration
	void initialize(ProjectHandler*, time_t);

	// Return time slot which contains the given start time.
	LongTermTimeSlot* getTimeSlot(time_t);

	// Return SchedBlock feasibility proportion of future LongTermTimeSlots.
	// This will never return zero.
	// Parameters:
	// * SchedBlock reference
	// * LongTermTimeSlot index after which start to count
	double getFutureFeasibilities(SchedBlock*, unsigned int);

	// Return average SB list size hold by LongTermTimeSlot instances
	unsigned int getAvgeSBListSize();

  private:
	unsigned int numSlots;
	time_t slotDuration;
	vector<LongTermTimeSlot*> timeSlots;
	unsigned int avgeSBListSize;
};

/* Coordinator class for TimeSlot instances, which are in a limited length list
 * (next X instances). This class also creates all main handler class instances.
 */
class ShortTermTime : public Observable
{
  public:
	/*constructor*/
	ShortTermTime();

	/*destructor*/
	~ShortTermTime();

	/*interface methods*/
	// Initialize a new period, by calling all private initialize* methods
	void initializePeriod();

	// Move foreward to the next time slot; this includes incrementing the
	// currentSlotIndex attribute and shifting the timeSlots list, which is
	// always of length numSlots. Also return the new current timeSlot.
	TimeSlot* nextTimeSlot();

	// Return time slot corresponding to the given index.
	// If no reference to this index is currently hold, return NULL.
	TimeSlot* getTimeSlot(unsigned int);

	// Return the total time that has been available for observations until now;
	// this is sum of duration of all short-term time slots that have been
	// available until now
	time_t getTotalAvailableTime();

	// Return a reference to projectHandler.
	ProjectHandler* getProjectHandler();

	// Return average SB list size hold by TimeSlot instances
	unsigned int getAvgeShortTermSBListSize();

	// Return average SB list size hold by LongTermTimeSlot instances
	unsigned int getAvgeLongTermSBListSize();

  private:
	ProjectHandler* projectHandler;
	LongTermTime* longTermTime;
	WeatherHandler* weatherHandler;
	unsigned int numSlots;
	time_t slotDuration;
	int currentSlotIndex;  // Initialized as -1, before moving to first slot
	time_t totalAvailableTime;
	list<TimeSlot*> timeSlots;
    unsigned int sbListSize;
    unsigned int sbListNum;

	// Instantiate and execute initialization for ProjectHandler
	void initializeProjectHandler();

	// Instantiate and execute initialization for LongTermTime
	void initializeLongTermTime();

	// Instantiate and execute initialization for WeatherHandler
	void initializeWeatherHandler();

	// Initialization of timeSlots list
	void initializeShortTermTime();

	// Add a new time slot to timeSlots list
	void addTimeSlot();
};

/* Short-term time slot representation, as discretization of the dynamic short
 * term planning. It associates execution success probability (according to
 * weather condition) and execution priority with feasible SchedBlocks. The
 * corresponding weather condition will be initialized when created, and further
 * updated through command executed by ShortTermTime whenever this one moves to
 * the next TimeSlot (once an hour).
 *
 * OBSERVER: TimeSlot observes Project for changes in completion attribute, to
 * update its priority map by updating other SBs belonging to that Project.
 *
 * OBSERVER: TimeSlot observes ShortTermTime for changes in currentSlotIndex
 * attribute, to update its currentWeather (app. once an hour).
 */
class TimeSlot : public Observer
{
  public:
	/*constructor*/
	// Parameters:
	// * WeatherHandler reference
	// * LongTermTime reference
	// * ProjectHandler reference
	// * Time slot index
	// * Start date
	// * End date
	TimeSlot(WeatherHandler*, LongTermTime*, ProjectHandler*, unsigned int, time_t, time_t);

	/*destructor*/
	~TimeSlot();

	/*interface methds*/
	// Return current availability
	bool isAvailable();

	// Set current availability to given value; if unavailable, no SBs can be
	// executed in this time slot; short-term time slot availability can never
	// conflict with corresponding long-term time slot availability
	void setAvailability(bool);

	// Return current map of SBs v/s execution priorities
	map<SchedBlock*, double> getPriorityList();

	// Return SB with the highest priority; this could evetually be NULL if the
	// current time slot is not available for observations, or if no SB is
	// available (should hopefully never happen)
	SchedBlock* getTopPriority();

	// Return the time slot's start date
	time_t getStartDate();

	// Return the time slot's end date
	time_t getEndDate();

	// Return slot index
	unsigned int getIndex();

	// Return associated LT time slot
	LongTermTimeSlot* getLongTermTimeSlot();

	// Return size of priority map
	unsigned int getSBListSize();

	// Update callback method
	virtual void notify(Observable*);

  private:
	unsigned int slotIndex;
	time_t startDate;
	time_t endDate;
	bool availability;
	weatherCondition currentWeather;
	weatherCondition futureWeather;
	WeatherHandler* weatherHandler;
	LongTermTimeSlot* longTermTimeSlot;
	LongTermTime* longTermTime;
	map<SchedBlock*, double> futureFeasibility;
	map<SchedBlock*, double> execSuccessProbability;
	map<SchedBlock*, double> execPriority;

	// Update the currentWeather attribute by asking weatherHandler
	void updateWeatherCondition();

	// Calculate the execution success probability for the given SB under the
	// currentWeather condition
	double calculateSuccessProbability(SchedBlock*);

	// Calculate future feasibility for the given SB, i.e. for how many future
	// long-term time slots this SB is feasible. This needs to be calculated
	// only once.
	double calculateFutureFeasibility(SchedBlock*);

	// Calculate the execution priority for the given SB, considering all
	// relevant factors (for now, equally weighted)
	double calculateExecPriority(SchedBlock*);
};

/* Represents one static weather parameter for a certain time period, according
 * to the resolution window. This can eventually be a mean of various static
 * values from the XML data.
 */
class WeatherData
{
  public:
	/*constructor*/
	WeatherData(short, time_t, double, double, double);

	/*destructor*/
	~WeatherData();

	/*public attributes*/
	short type;  // refers to enum weatherParameter
	time_t time;
	double value;
	double rms;
	double slope;
};

/* Handles instances of WeatherData. The static data is provided by the input
 * data library, while the dynamically changing conditions (return values) are
 * generated based on the static data, plus a random distribution over time.
 */
class WeatherHandler
{
  public:
	/*constructor*/
	WeatherHandler();

	/*destructor*/
	~WeatherHandler();

	/*interface methods*/
	// Create all WeatherData objects, based on the XML input
	void initialize(string, time_t, time_t);

	// Get weatherCondition for the given time interval, eventually summarizing
	// data to return a mean for the period; if time interval overlaps with
	// WeatherData time separations, pick up the closest one
	weatherCondition getWeatherCondition(time_t, time_t);

	// Get atmospheric parameters according to given frequency and PWV values.
	atmosphericData getFrequencyParameters(double, double);

	// Calculate system temperature (Tsys) according to the given sky coordinates
	// and atmosphericData.
	double getSystemTemperature(double, double, atmosphericData&);

  private:
	time_t resolution;
	list<WeatherData*> temperatureRecords;
	list<WeatherData*> humidityRecords;
	map< double, list<asch_input::AtmData*> >* atmDataRecords;

	// Calculate an estimated PWV value for given weather condition (humidity+temperature).
	void estimatePwv(weatherCondition&);

	// Calculate interpolated opacity and atmospheric brightness temperature for
	// the given atmospheric data (pwv+frequency).
	void interpolateOpacityAndTemperature(atmosphericData&);

	// Return enclosing PWV values, considering the input data associated to
	// discrete PWV values.
	double* getEnclosingPwvInterval(double);

	// Return enclosing atmosphericData structures for the given PWV and frequency
	// values and considering the input data associated to discrete PWV and frequency
	// values.
	atmosphericData* getEnclosingIntervalForPwvAndFreq(double, double);
};

/* Master scheduler of SchedBlock instances. It has to take the "execute" SBs
 * (mark as observed) assigned by the ShortTermTime. This class is also in
 * charge of the "time simulation", thus informs ShortTermTime when it should
 * move to a new time slot. The Scheduler can set a TimeSlot as
 * available/unavailable and calculate at any moment the current value of the
 * objective functions.
 */
class Scheduler
{
  public:
	/*constructor*/
	Scheduler();

	/*destructor*/
	~Scheduler();

	// Create a ShortTermTime object and initialize the period
	void initializePeriod();

	/*interface methods*/
	// Execute the scheduling process by moving through time slots and executing SBs
	void runSchedule();

	// Calculate current scientific throughput percentage of the absolute schedule,
	// i.e. the sum of scientific grade of all completed projects versus the sum of
	// all available projects.
	double calculateTotalThroughput();

	// Calculate the total time usage percentege, i.e. how much of the
	// totalAvailableTime of the period has been dedicated to SBs execution,
	// based on their duration. Should return a number in range [0,1].
	double calculateTotalTimeUsage();

	// Calculate the total number of schedBlocks as a proportion of
	// total available schedBlocks.
	double calculateExecutedSchedBlocks();

	// Return absolute execution order.
	map<time_t,SchedBlock*> getExecutionOrder();

	// Return list of incomplete projects.
	list<Project*> getPendingProjects();

	// Return average next SB processing time
	double getAvgeNextSBTime();

	// Return average SB execution processing time
	double getAvgeExecuteTime();

	// Return average next time slot processing time
	double getAvgeNextTimeSlotTime();

	// Return average SB list size hold by TimeSlot instances
	unsigned int getAvgeShortTermSBListSize();

	// Return average SB list size hold by LongTermTimeSlot instances
	unsigned int getAvgeLongTermSBListSize();

  private:
	ShortTermTime* shortTermTime;
	ProjectHandler* projectHandler;
	TimeSlot* currentTimeSlot;
	time_t currentTime;
	map<time_t,SchedBlock*> executionOrder;

	time_t nextSBTime;
	unsigned int nextSBCount;
	time_t executeTime;
	unsigned int executeCount;
	time_t nextTimeSlotTime;
	unsigned int nextTimeSlotCount;

	// Request next ShortTermTime SB to execute (top priority)
	SchedBlock* nextSB();

	// Execute the given SB, by marking it as observed, adding it to the
	// observationOrder list and waiting for it to finish (adding "duration" to
	// currentTime); all SBs are considered to be successfully executed
	// Return false if some error during execution.
	bool execute(SchedBlock*);

	// Move to next ShortTermTime time slot.
	// Return false if no further time slot available.
	bool nextTimeSlot();

	// Set the availability of the TimeSlot corresponding to the given index
	// (1=set available, 0=set unavailable)
	void setTimeSlotAvailability(unsigned int, bool);

	// Wait until the given time. This includes increasing currentTime and
	// sleeping a corresponding amount of time.
	void waitUntil(time_t);
};

/* Handles all projects and provides access functionality. The construction of
 * the Project collection is based on data provided by the input data library.
 */
class ProjectHandler
{
  public:
	/*constructor*/
	ProjectHandler();

	/*destructor*/
	~ProjectHandler();

	/*interface methods*/
	// Create all Project objects, based on the XML input
	void initialize(string);

	// Return all projects
	list<Project*> getProjects();

	// Return all completed projects (completion less than 1)
	list<Project*> getCompletedProjects();

	// Return all not completed projects
	list<Project*> getPendingProjects();

	// Return project corresponding to the given index
	Project* getProject(unsigned int);

	// Return the period's start date
	time_t getStartDate();

	// Return the period's end date
	time_t getEndDate();

	// Return the observation place's latitude
	double getLatitude();

	// Return the observation place's longitude
	double getLongitude();

	// Return the observation place's time diference
	int getUT();

  private:
	time_t startDate;
	time_t endDate;
	double latitude;
	double longitude;
	int UT;
	list<Project*> projects;
};

/* Representation of a project, associated to its scheduling blocks.
 *
 * OBSERVER: Project observes SchedBlock for changes in the status attribute, to
 * update its completion.
 */
class Project : public Observer, public Observable
{
  public:
	/*constructor*/
	Project(ProjectHandler* projectHandler, double projectIndex, double sciGrade);

	/*destructor*/
	~Project();

	/*interface methods*/
	// Return project index
	unsigned int getIndex();

	// Return project scientific grade
	double getSciGrade();

	// Return project's remaining SBs percentage, based on the number of pending
	// SBs in the total amount of SBs
	double getRemainingExec();

	// Return all SBs belonging to this project
	list<SchedBlock*> getSchedBlocks();

	// Return all not completed SBs belonging to this project
	list<SchedBlock*> getPendingSchedBlocks();

	// Return SB corresponding to the given index
	SchedBlock* getSchedBlock(unsigned int);

	// Return the reference to the Project Handler
	ProjectHandler* getProjectHandler();

	// Adds an SB to the project
	void addSchedBlock(
		unsigned int schedBlockIndex,
		int seqBlockIndex,
		time_t execTime,
		double obsFrequency,
		double RA,
		double Dec,
		double LSTr,
		double LSTs
);

	// Update callback method
	virtual void notify(Observable*);

  private:
	ProjectHandler *projectHandler;
	unsigned int projectIndex;
	double sciGrade;
	double remainingExec;
	list<SchedBlock*> schedBlocks;
	unsigned int numSchedBlocks;
};

/* Scheduling block representation, associated to the corresponding Project
 * instance. Also, associations with another SchedBlock index (sequence blocks)
 * are possible.
 */
class SchedBlock : public Observable
{
  public:
	/*constructor*/
	SchedBlock(
		Project* project,
		unsigned int schedBlockIndex,
		int seqBlockIndex,
		unsigned int execTime,
		double obsFrequency,
		double RA,
		double Dec,
		double LSTr,
		double LSTs
		);

	/*destructor*/
	~SchedBlock();

	/*interface methods*/
	// Return SB index
	unsigned int getIndex();

	// Return parent project
	Project* getProject();

	// Return execution time
	time_t getExecTime();

	// Get weather condition requirements
	double getObservationFrequency();

	// Return availability to be executed; if already executed, or if an
	// unsatisfied sequence block dependency exists, this will be false
	bool isAvailable();

	// Return current execution status
	bool isExecuted();

	// Set this SB as executed
	void setExecuted();

	// Check if the coordinates are visible during a period of time
	bool isVisible(time_t start, time_t end);

	// Return reference source RA coordinate
	double getRA();

	// Return reference source Dec coordinate
	double getDec();

  private:
	Project* project;
	unsigned int schedBlockIndex;
	int seqBlockIndex;
	time_t execTime;
	double obsFrequency;
	bool status;  // 0=pending, 1=executed
	double RA;
	double Dec;
	double LSTr;
	double LSTs;
};

#endif
