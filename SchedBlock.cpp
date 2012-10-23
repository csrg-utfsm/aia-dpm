#include "classes.hpp"

using namespace std;

SchedBlock::SchedBlock(
		Project* project,
		unsigned int schedBlockIndex,
		int seqBlockIndex,
		unsigned int execTime,
		double obsFrequency,
		double RA,
		double Dec,
		double LSTr,
		double LSTs
	):
		project(project),
		schedBlockIndex(schedBlockIndex),
		seqBlockIndex(seqBlockIndex),
		execTime(execTime),
		obsFrequency(obsFrequency),
		status(false),
		RA(RA),
		Dec(Dec),
		LSTr(LSTr),
		LSTs(LSTs)
{
}

SchedBlock::~SchedBlock(){
}

unsigned int SchedBlock::getIndex(){
	return schedBlockIndex;
}

Project* SchedBlock::getProject(){
	return project;
}

time_t SchedBlock::getExecTime(){
	return execTime;
}

double SchedBlock::getObservationFrequency(){
	return obsFrequency;
}

bool SchedBlock::isAvailable(){
	if(seqBlockIndex == -1) return true;
	
	SchedBlock * sq_blk = this->project->getSchedBlock(seqBlockIndex);
	
	if(sq_blk == NULL) return true;
	
	if(sq_blk->isExecuted()) return true;
	
	return false;
}

bool SchedBlock::isExecuted(){
	return status;
}

void SchedBlock::setExecuted(){
	if (!status) {
		this->status = true;
		this->notifyObservers();
	}
}

double SchedBlock::getRA() {
	return RA;
}

double SchedBlock::getDec() {
	return Dec;
}

bool RA_Dec_to_valid(time_t unixTime, double RA, double Dec, double LAT, double LONG, double UT)
{
	/* This function can be optimized */
	const double GtoR = 0.0174532925;
	
	double JD = (unixTime / 86400.0) + 2440587.5;
	double LST = 100.46 + 0.985647 * JD + LONG + 15*UT;
	double HR_ANG = (LST - RA) < 0 ? LST - RA + 360: LST - RA;
	
	double ALT = asin( 
		sin( LAT * GtoR) * sin( Dec * GtoR) + 
		cos( LAT * GtoR) * cos( Dec * GtoR) * cos( HR_ANG * GtoR));
	
	if(ALT/GtoR > 10)
		return true;
	return false;
}

bool LST_to_valid(time_t unixTime, double LSTr, double LSTs, double LAT, double LONG, double UT)
{
	/* This function can be optimized */
	double JD = (unixTime / 86400.0) + 2440587.5;
	double LST = 100.46 + 0.985647 * JD + LONG + 15*UT;
	LST = LST - floor(LST);
	if(LSTr < LST && LST < LSTs )
		return true;
	return false;
}

bool SchedBlock::isVisible(time_t start, time_t end){
	ProjectHandler *ph = this->project->getProjectHandler();
	double latitude = ph->getLatitude();
	double longitude = ph->getLongitude();
	double UT = ph->getUT();
	
/*	
	return (LST_to_valid(start,LSTr,LSTs,latitude,longitude,UT) &&
//			LST_to_valid((start+end)/2,LSTr,LSTs,latitude,longitude,UT) &&
			LST_to_valid(end,LSTr,LSTs,latitude,longitude,UT));
	*/
			
	return (RA_Dec_to_valid(start,RA,Dec,latitude,longitude,UT) &&
//			RA_Dec_to_valid((start+end)/2,RA,Dec,latitude,longitude,UT) &&
			RA_Dec_to_valid(end,RA,Dec,latitude,longitude,UT));

}

