#INPUT MAKEFILE
echo=off

# Name of the program
PROGRAM= aia-dpm

INCLUDEDIRS = -I./include
LIBDIRS= -L./lib
LIBS= -linput -lxerces-c -lm

#List of source files
CXXSOURCES = LongTermTimeSlot.cpp LongTermTime.cpp TimeSlot.cpp ShortTermTime.cpp SchedBlock.cpp Project.cpp ProjectHandler.cpp WeatherData.cpp WeatherHandler.cpp Scheduler.cpp
#List of header files
CXXHEADERS = classes.hpp
CXXOBJECTS = $(CXXSOURCES:.cpp=.o)
CXXFLAGS = -c -Wall -O2 $(INCLUDEDIRS)
CXX = g++
LIBFILES = ./lib/libinput.a

LDFLAGS = $(LIBDIRS) $(LIBS)

all: $(LIBFILES) $(PROGRAM)

$(PROGRAM): $(MAKEDEPS) $(CXXOBJECTS) $(CXXHEADERS)
	$(CXX) -o $(PROGRAM) $(CXXOBJECTS) $(LDFLAGS)

clean:
	$(RM) $(CXXOBJECTS)
	$(MAKE) -C ./asch-input clean
	
distclean:
	$(RM) -rf include lib config
	$(RM) $(CXXOBJECTS) $(PROGRAM)
	$(MAKE) -C ./asch-input clean

run:
	@./$(PROGRAM) 

o:
	@./$(PROGRAM) 

lib/libinput.a:
	$(MAKE) -C ./asch-input export
