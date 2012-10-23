#ifndef __OBSUNIT_HPP__
#define __OBSUNIT_HPP__

#include <list>
#include <string>
#include "Optional.hpp"

using namespace std;


namespace asch_input{

class ObsProject{
	private:
	long id;
	string principalInvestigator;
	float scientificScore;
	int scientificRank;

	public:
	ObsProject(long i, std::string pi, float ss, int sr):
		id(i),
		principalInvestigator(pi),
		scientificScore(ss),
		scientificRank(sr){};

	long getId(){return this->id;}
	string getPrincipalInvestigator(){return this->principalInvestigator;}
	float getScientificScore(){return this->scientificScore;}
	int getScientificRank(){return this->scientificRank;}

};

class AssemblyRequirement
{
	private:
	Optional<string> name;

	public:
	AssemblyRequirement(Optional<string> n):name(n){}

	Optional<string> getName(){return this->name;}

};

class EquipmentRequirement
{
	private:
	list<AssemblyRequirement *>* assemblyRequirement;

	public:
	EquipmentRequirement(list<AssemblyRequirement *>* ar):
		assemblyRequirement(ar){}

	list<AssemblyRequirement *>* getAssemblyRequirement(){return this->assemblyRequirement;}
};

class HardwareRequirements
{
	private:
	list<EquipmentRequirement *>* equipmentRequirement;

	public:
	HardwareRequirements(list<EquipmentRequirement *>* er):
		equipmentRequirement(er){}

	list<EquipmentRequirement *>* getEquipmentRequirement(){return this->equipmentRequirement;}

};



class SpectralSpec
{
};

class OpticalCameraSpec
{
};


class InstrumentSpec
{
	public:
	enum ObservingModeType{OpticalPointing,SingleFieldInterferometry,TotalPower,Holography};

	private:
	Optional<ObservingModeType> observingMode;
	Optional<SpectralSpec> spectralSpec;
	Optional<OpticalCameraSpec> opticalCameraSpec;
	Optional<HardwareRequirements> hardwareRequirements;

	public:
	InstrumentSpec(Optional<ObservingModeType> om,
		Optional<SpectralSpec> ss,
		Optional<OpticalCameraSpec> ocs,
		Optional<HardwareRequirements> hr):
		observingMode(om),
		spectralSpec(ss),
		opticalCameraSpec(ocs),
		hardwareRequirements(hr){}

	Optional<ObservingModeType> getObservingMode(){return this->observingMode;}
	Optional<SpectralSpec> getSpectralSpec(){return this->spectralSpec;}
	Optional<OpticalCameraSpec> getOpticalCameraSpec(){return this->opticalCameraSpec;}
	Optional<HardwareRequirements> getHardwareRequirements(){return this->hardwareRequirements;}

};

class Target
{
	private:
	string id;
	string sourceIdRef;
	string instrumentSpecIdRef;
	string obsParametersIdRef;

	public:
	Target(string i, string sir, string isir, string opir):
	id(i),
	sourceIdRef(sir),
	instrumentSpecIdRef(isir),
	obsParametersIdRef(opir){}

	string getId(){return this->id;}
	string getSourceIdRef(){return this->sourceIdRef;}
	string getInstrumentSpecIdRef(){return this->instrumentSpecIdRef;}
	string getObsParametersIdRef(){return this->obsParametersIdRef;}
};

class FieldSource{
	private:
	string id;
	Optional<string> name;
	Optional<double> RA;
	Optional<double> Dec;
	Optional<string> ephemeris;
	Optional<double> pmRA;
	Optional<double> pmDec;

	public:
	FieldSource(string i,Optional<string> n,Optional<double> r,Optional<double> d,
		Optional<string> e,Optional<double> pra,Optional<double> pd):
		id(i),
		name(n),
		RA(r),
		Dec(d),
		ephemeris(e),
		pmRA(pra),
		pmDec(pd){}

	string getId(){return this->id;}
	Optional<string> getName(){return this->name;}
	Optional<double> getRA(){return this->RA;}
	Optional<double> getDec(){return this->Dec;}
	Optional<string> getEphemeris(){return this->ephemeris;}
	Optional<double> getPmRA(){return this->pmRA;}
	Optional<double> getPmDec(){return this->pmDec;}

};

class WeatherConstrains{
	private:
	Optional<double> maxWindVelocity;
	Optional<double> maxOpacity;
	Optional<double> minPhaseStability;
	Optional<double> maxSeeing;

	public:
	WeatherConstrains(Optional<double> mwv, Optional<double> mo,
		Optional<double> mps, Optional<double> ms):
		maxWindVelocity(mwv),
		maxOpacity(mo),
		minPhaseStability(mps),
		maxSeeing(ms){}

	Optional<double> getMaxWindVelocity(){return this->maxWindVelocity;}
	Optional<double> getMaxOpacity(){return this->maxOpacity;}
	Optional<double> getMinPhaseStability(){return this->minPhaseStability;}
	Optional<double> getMaxSeeing(){return this->maxSeeing;}



};

class Preconditions
{
	private:
	Optional<double> maxAllowedHA;
	Optional<double> minAllowedHA;

	public:
	Preconditions(Optional<double> maaha, Optional<double> miaha):
		maxAllowedHA(maaha),
		minAllowedHA(miaha){}

	Optional<double> getMaxAllowedHA(){return this->maxAllowedHA;}
	Optional<double> getMinAllowedHA(){return this->minAllowedHA;}


};

class SchedulingConstrains
{
	private:
	Optional<double> maxAngularResolution;
	Optional<double> representativeFrequency;
	Optional<string> representativeTargetIdRef;

	public:
	SchedulingConstrains(Optional<double> mar, Optional<double> rf, Optional<string> rtir):
		maxAngularResolution(mar),
		representativeFrequency(rf),
		representativeTargetIdRef(rtir){}

	Optional<double> getMaxAngularResolution(){return this->maxAngularResolution;}
	Optional<double> getRepresentativeFrequency(){return this->representativeFrequency;}
	Optional<string> getRepresentativeTargetIdRef(){return this->representativeTargetIdRef;}

};

class SchedulingBlockControl
{
	public:
	enum ArrayType
	{
		ACA,SEVEN_M,TP_ARRAY,TWELVE_M
	};

	private:
	double maximumTime;
	double estimatedExecutionTime;
	ArrayType arrayRequested;
	bool indefiniteRepeat;

	public:

	SchedulingBlockControl(double mt, double eet, ArrayType ar, bool ir):
		maximumTime(mt),
		estimatedExecutionTime(eet),
		arrayRequested(ar),
		indefiniteRepeat(ir){}

	double getMaximumTime(){return this->maximumTime;}
	double getEstimatedExecutionTime(){return this->estimatedExecutionTime;}
	ArrayType getArrayRequested(){return this->arrayRequested;}
	bool getIndefiniteRepeat(){return this->indefiniteRepeat;}


};


class ScienceParameters
{
	private:
	Optional<double> duration;
	Optional<double> representativeBandwidth;
	Optional<double> representativeFrequency;
	Optional<double> sensitivityGoal;

	public:
	ScienceParameters(Optional<double> d, Optional<double> rb,
		Optional<double> rf, Optional<double> sg):
		duration(d),representativeBandwidth(rb),
		representativeFrequency(rf),sensitivityGoal(sg){}

	Optional<double> getDuration(){return this->duration;}
	Optional<double> getRepresentativeBandwidth(){return this->representativeBandwidth;}
	Optional<double> getRepresentativeFrequency(){return this->representativeFrequency;}
	Optional<double> getSensitivityGoal(){return this->sensitivityGoal;}


};

class ObsParameters
{
	private:
	string id;
	ScienceParameters scienceParameters;

	public:
	ObsParameters(string i, ScienceParameters sp):id(i),
		scienceParameters(sp){}

	string getId(){return this->id;}
	ScienceParameters getScienceParameters(){return this->scienceParameters;}

};

class SchedBlock{
	private:

	long projectId;
	WeatherConstrains weatherConstrains;
	SchedulingConstrains schedulingConstrains;
	Preconditions preconditions;
	SchedulingBlockControl schedulingBlockControl;
	ObsParameters obsParameters;
	list<InstrumentSpec *>* instrumentSpec;
	list<Target *>* target;
	list<FieldSource *>* fieldSource;

	public:

	SchedBlock(long pid, WeatherConstrains wc, SchedulingConstrains sc,
		Preconditions p, SchedulingBlockControl sbc, ObsParameters op,
		list<InstrumentSpec *>* is, list<Target *>* t,
		list<FieldSource *>* fs):
			weatherConstrains(wc),
			schedulingConstrains(sc),
			preconditions(p),
			schedulingBlockControl(sbc),
			obsParameters(op),
			instrumentSpec(is),
			target(t),
			fieldSource(fs){}

	WeatherConstrains getWeatherConstrains(){return this->weatherConstrains;}
	SchedulingConstrains getSchedulingConstrains(){return this->schedulingConstrains;}
	Preconditions getPreconditions(){return this->preconditions;}
	SchedulingBlockControl getSchedulingBlockControl(){return this->schedulingBlockControl;}
	ObsParameters getObsParameters(){return this->obsParameters;}
	list<InstrumentSpec *>* getInstrumentSpec(){return this->instrumentSpec;}
	list<Target *>* getTarget(){return this->target;}
	list<FieldSource *>* getFieldSource(){return this->fieldSource;}

};
}



#endif
