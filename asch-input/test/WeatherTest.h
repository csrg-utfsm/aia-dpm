#ifndef WEATHERTEST_H
#define WEATHERTEST_H

#include <cppunit/extensions/HelperMacros.h>

class WeatherTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( WeatherTest );
	CPPUNIT_TEST( testWindSpeedConstructor );
	CPPUNIT_TEST( testWindSpeedLoad );
	CPPUNIT_TEST( testTemperatureLoad );
	CPPUNIT_TEST( testOpacityLoad );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	void testWindSpeedConstructor();
	void testWindSpeedLoad();
	void testTemperatureLoad();
	void testOpacityLoad();
};

#endif  // WEATHERTEST_H

