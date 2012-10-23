#include "WeatherTest.h"
#include "../WeatherReader.hpp"
#include "../DataTypes/WindSpeed.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( WeatherTest );


void WeatherTest::setUp(){
}


void WeatherTest::tearDown(){
}


void WeatherTest::testWindSpeedConstructor(){
	asch_input::WindSpeed *tmpWS = new asch_input::WindSpeed(1.1f, 2.1f, 3.1f, 4.1f);
	CPPUNIT_ASSERT( tmpWS != NULL );
	CPPUNIT_ASSERT( tmpWS->getTime() == 1.1f );
	CPPUNIT_ASSERT( tmpWS->getValue() == 2.1f );
	CPPUNIT_ASSERT( tmpWS->getRms() == 3.1f );
	CPPUNIT_ASSERT( tmpWS->getSlope() == 4.1f );
	CPPUNIT_ASSERT( tmpWS->getSlope() != 5.1f );
	delete tmpWS;
}

void WeatherTest::testWindSpeedLoad(){
	asch_input::WeatherReader *weatherReader = new asch_input::WeatherReader();
	weatherReader->loadWindSpeedHistory();
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0000000, weatherReader->getWindspeedRecords()->front()->getTime(), 0.000005 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( -999.00000, weatherReader->getWindspeedRecords()->front()->getValue(), 0.000005 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( -999.00000, weatherReader->getWindspeedRecords()->front()->getRms(), 0.000005 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( -999.00000, weatherReader->getWindspeedRecords()->front()->getSlope(), 0.000005 );
	
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 365.25000, weatherReader->getWindspeedRecords()->back()->getTime(), 0.000005 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( -999.00000, weatherReader->getWindspeedRecords()->back()->getValue(), 0.000005 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( -999.00000, weatherReader->getWindspeedRecords()->back()->getRms(), 0.000005 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( -999.00000, weatherReader->getWindspeedRecords()->back()->getSlope(), 0.000005 );
	delete weatherReader;
}

void WeatherTest::testTemperatureLoad(){
	asch_input::WeatherReader *weatherReader = new asch_input::WeatherReader();
	weatherReader->loadTemperatureHistory();
	delete weatherReader;
}

void WeatherTest::testOpacityLoad(){
	asch_input::WeatherReader *weatherReader = new asch_input::WeatherReader();
	weatherReader->loadOpacityHistory();
	delete weatherReader;
}


