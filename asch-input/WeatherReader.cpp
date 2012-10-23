#include "WeatherReader.hpp"
#include "util/Tokenizer.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>

using namespace asch_input;

WeatherReader::WeatherReader(std::string isource){
	std::string WeatherDirectory;
	char cwd[PATH_MAX];
	
	this->temperatureRecords = NULL;
	this->humidityRecords = NULL;
	this->opacityRecords = NULL;
	this->windspeedRecords = NULL;
	this->atmDataRecords = NULL;
	
	/*Get Weather Folder*/

	if (getcwd (cwd, PATH_MAX) == 0)
	{
	  // Buffer too small?
	}

	try{
		xml_schema::properties config_xsd;
		config_xsd.no_namespace_schema_location ("file://" + std::string (cwd) +"/config/config.xsd");
//		config_xsd.schema_location ("http://www.w3.org/XML/1998/namespace", "file://" + std::string (cwd) + "config/xml.xsd");
		std::auto_ptr<Configuration> configRoot(Configuration_(isource + "aprc-config.xml",0,config_xsd));
		
		WeatherDirectory = (isource + configRoot->WeatherDirectory() + "/");
	}
	catch (const xml_schema::exception& e)
	{
		std::cerr << e << std::endl;
	}
	
	this->temperatureFile = WeatherDirectory + "Temp_average_15min_all.dat";
	this->opacityFile = WeatherDirectory + "Opacity_average_15min_all.dat";
	this->windspeedFile = WeatherDirectory + "WindSpeed_average_15min_all.dat";

	this->pwvToAtmDataFiles.insert(make_pair(0.4722, WeatherDirectory + "SKY.SPE0001.trim"));
	this->pwvToAtmDataFiles.insert(make_pair(0.6580, WeatherDirectory + "SKY.SPE0002.trim"));
	this->pwvToAtmDataFiles.insert(make_pair(0.9134, WeatherDirectory + "SKY.SPE0003.trim"));
	this->pwvToAtmDataFiles.insert(make_pair(1.2620, WeatherDirectory + "SKY.SPE0004.trim"));
	this->pwvToAtmDataFiles.insert(make_pair(1.7960, WeatherDirectory + "SKY.SPE0005.trim"));
	this->pwvToAtmDataFiles.insert(make_pair(2.7480, WeatherDirectory + "SKY.SPE0006.trim"));
	this->pwvToAtmDataFiles.insert(make_pair(5.1860, WeatherDirectory + "SKY.SPE0007.trim"));
}

WeatherReader::~WeatherReader(){
		std::cout << "LOG: ~WeatherReader()" << std::endl;
		if( temperatureRecords != NULL ){
				while( !temperatureRecords->empty() ){
						Temperature* tmp = temperatureRecords->front();
						delete tmp;
						temperatureRecords->pop_front();
				}
				delete temperatureRecords;
		}
		if( humidityRecords != NULL ){
				while( !humidityRecords->empty() ){
						Humidity* tmp = humidityRecords->front();
						delete tmp;
						humidityRecords->pop_front();
				}
				delete humidityRecords;
		}
		if( opacityRecords != NULL ){
				while( !opacityRecords->empty() ){
						Opacity* tmp = opacityRecords->front();
						delete tmp;
						opacityRecords->pop_front();
				}
				delete opacityRecords;
		}
		if( windspeedRecords != NULL ){
				while( !windspeedRecords->empty()){
						WindSpeed* tmp = windspeedRecords->front();
						delete tmp;
						windspeedRecords->pop_front();
				}
				delete windspeedRecords;
		}
		if( atmDataRecords != NULL ){
			std::map< double, std::list<asch_input::AtmData*> >::iterator it;
			for (it=atmDataRecords->begin(); it!=atmDataRecords->end(); it++) {
				std::list<asch_input::AtmData*> atmDataList = it->second;
				while (!atmDataList.empty()) {
					asch_input::AtmData* oldData = atmDataList.front();
					delete oldData;
					atmDataList.pop_front();
				}
			}
			delete atmDataRecords;
		}
}


void WeatherReader::loadWindSpeedHistory(){
	std::cout << "LOG: loadWindSpeedHistory()" << std::endl;
	this->windspeedRecords = new std::list<WindSpeed*>();
	std::list<std::string> *tokens;
	Tokenizer *tokenizer = new Tokenizer( windspeedFile, " " );
	tokens = tokenizer->getNextLineOfTokens();
	while( tokens != NULL ){
		double time = boost::lexical_cast<double>(tokens->front());
		tokens->pop_front();
		double value = boost::lexical_cast<double>(tokens->front());
		tokens->pop_front();
		double rms = boost::lexical_cast<double>(tokens->front());
		tokens->pop_front();
		double slope = boost::lexical_cast<double>(tokens->front());
		tokens->pop_front();
		WindSpeed *newWS = new WindSpeed(time, value, rms, slope);
		windspeedRecords->push_back(newWS);
		delete tokens;
		tokens = tokenizer->getNextLineOfTokens();
	};
	
	delete tokenizer;
}

void WeatherReader::loadOpacityHistory(){
	std::cout << "LOG: loadOpacityHistory()" << std::endl;
	this->opacityRecords = new std::list<Opacity*>();
	std::list<std::string> *tokens;
	Tokenizer *tokenizer = new Tokenizer( opacityFile, " " );
	tokens = tokenizer->getNextLineOfTokens();
	while( tokens != NULL ){
		double time = boost::lexical_cast<double>(tokens->front());
		tokens->pop_front();
		double value = boost::lexical_cast<double>(tokens->front());
		tokens->pop_front();
		double rms = boost::lexical_cast<double>(tokens->front());
		tokens->pop_front();
		double slope = boost::lexical_cast<double>(tokens->front());
		tokens->pop_front();
		Opacity *newO = new Opacity( time, value, rms, slope );
		opacityRecords->push_back(newO);
		delete tokens;
		tokens = tokenizer->getNextLineOfTokens();
	};
	
	delete tokenizer;
}

void WeatherReader::loadTemperatureHistory(){
	std::cout << "LOG: loadTemperatureHistory()" << std::endl;
	this->temperatureRecords = new std::list<Temperature*>();
	std::list<std::string> *tokens;
	Tokenizer *tokenizer = new Tokenizer( temperatureFile, " " );
	tokens = tokenizer->getNextLineOfTokens();
	while( tokens != NULL ){
		double time = boost::lexical_cast<double>(tokens->front());
		tokens->pop_front();
		double value = boost::lexical_cast<double>(tokens->front());
		tokens->pop_front();
		double rms = boost::lexical_cast<double>(tokens->front());
		tokens->pop_front();
		double slope = boost::lexical_cast<double>(tokens->front());
		tokens->pop_front();
		Temperature *newT = new Temperature( time, value, rms, slope );
		temperatureRecords->push_back(newT);
		delete tokens;
		tokens = tokenizer->getNextLineOfTokens();
	};
	
	delete tokenizer;
}
void WeatherReader::loadHumidityHistory(){
	// As there are is no humidity data available, values are generated randomly
	// for each opacity record.
	std::cout << "LOG: loadHumidityHistory()" << std::endl;
	this->humidityRecords = new std::list<Humidity*>();
	std::list<std::string> *tokens;
	Tokenizer *tokenizer = new Tokenizer( opacityFile, " " );
	tokens = tokenizer->getNextLineOfTokens();
	while( tokens != NULL ) {
		double time = boost::lexical_cast<double>(tokens->front());
		tokens->pop_front();
		tokens->pop_front();
		tokens->pop_front();
		tokens->pop_front();
		delete tokens;
		tokens = tokenizer->getNextLineOfTokens();

		srand(time);
		double value = 0.5 + double(rand()%100)/100.0 * 4.0;
		Humidity *newH = new Humidity( time, value, 0.0, 0.0 );
		humidityRecords->push_back(newH);
	}
	delete tokenizer;
}

void WeatherReader::loadAtmData(){
	std::cout << "LOG: loadAtmData()" << std::endl;

	this->atmDataRecords = new std::map< double, std::list<AtmData*> >();
	std::map<double,std::string>::iterator fileIter;
	for (fileIter=pwvToAtmDataFiles.begin(); fileIter!=pwvToAtmDataFiles.end(); fileIter++) {
		std::string line;
		std::string filename = fileIter->second;
		std::ifstream file;
		std::cout << "LOG: Opening file " << filename << std::endl;
		file.open(filename.c_str());
		if (!file.is_open()) {
			std::cerr << "ERROR: Cannot open input file " << filename << std::endl;
			return;
		}
		double pwv = fileIter->first;
		std::list<AtmData*> atmDataList;
		while (!file.eof()) {
			getline(file, line);
			if (file.eof())
				break;
			std::vector<std::string> params;
			boost::split(params, line, boost::is_any_of(" "));
			if (params.size() != 3) {
				std::cerr << "ERROR: Wrong format while parsing input file " << filename << " : " << line << std::endl;
				continue;
			}
			double frequency = atof(params[0].c_str());
			double opacity = atof(params[1].c_str());
			double temperature = atof(params[2].c_str());
			atmDataList.push_back(new AtmData(pwv,frequency,opacity,temperature));
		}
		file.close();
		this->atmDataRecords->insert(make_pair(pwv,atmDataList));
	}
}


std::list<Temperature*>* WeatherReader::getTemperatureRecords(){
	return this->temperatureRecords;
}

std::list<Humidity*>* WeatherReader::getHumidityRecords(){
	return this->humidityRecords;
}

std::list<Opacity*>* WeatherReader::getOpacityRecords(){
	return this->opacityRecords;
}

std::list<WindSpeed*>* WeatherReader::getWindspeedRecords(){
	return this->windspeedRecords;
}

std::map< double, std::list<AtmData*> >* WeatherReader::getAtmDataRecords(){
	return this->atmDataRecords;
}

