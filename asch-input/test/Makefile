CXXFLAGS=-Wall
CXX_LDFLAGS='cppunit'
LDFLAGS=-lcppunit

all: WeatherSuite

WeatherSuite: WeatherSuite.cpp WeatherTest.o ../WeatherReader.o ../util/Tokenizer.o

clean:
	rm -rf *.o WeatherSuite

