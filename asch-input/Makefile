#INPUT MAKEFILE
echo=off
CC=c++
CFLAGS=-c -Wall

all: input

#############XML INPUT DAO###############
#Macros
XSD=config/config.xsd config/executive.xsd config/observatory.xsd config/obsunit.xsd
XSD-HEAD=$(XSD:config/%=xsd-head/%)
XSD-HEAD-H=$(XSD-HEAD:.xsd=.hxx)
XSD-HEAD-C=$(XSD-HEAD:.xsd=.cxx)
XSD-HEAD-O=$(XSD-HEAD:.xsd=.o)
DAO=XmlInputDao.hpp Dao.hpp ExecutiveDao.hpp ObservatoryDao.hpp ObsProjectDao.hpp
DATA-TYPES-H=DataTypes/*

#GENERATED OBJECTS TO BE INCLUDED
XML_OBJ=$(XSD-HEAD-O) XmlReader.o 

xsd-headers: $(XSD-HEAD-H)

xml-clean:
	rm -f *.o 
	rm -rf xsd-head/

#XML Reader Generation

xsd-head/%.hxx xsd-head/%.cxx: config/%.xsd 
	mkdir -p xsd-head
	xsdcxx cxx-tree --output-dir ./$(dir $@) ./$<

xsd-head/%.o: xsd-head/%.cxx xsd-head/%.hxx
	$(CC) -I.../libxsd $(CFLAGS) $< -o $@

#Object Dependencies
XmlInputDao.o: XmlReader.cpp XmlReader.hpp $(DAO) $(XSD-HEAD-H) $(DATA-TYPES-H)
	$(CC) -I.../libxsd $(CFLAGS) $<

#############WEATHER DATA DAO###############
#GENERATED OBJECTS TO BE INCLUDED
WDATA_OBJ=WeatherReader.o util/Tokenizer.o

wdata-clean:
	rm -f util/*.o
	
##########STATIC LIBRARY GENERATION#########
input: $(XML_OBJ) $(WDATA_OBJ)
	ar -cvq lib$@.a $^

	
##############EXPORTING ROUTINES############

export: input
	mkdir -p ../include
	cp ./*.hpp ../include
	mkdir -p ../include/DataTypes
	cp ./DataTypes/*.hpp ../include/DataTypes
	mkdir -p ../include/xsd-head
	cp ./xsd-head/*.hxx ../include/xsd-head
	mkdir -p ../lib
	cp ./libinput.a ../lib/libinput.a 
	mkdir -p ../config
	cp ./config/* ../config/
	

##############CLEANING ROUTINES#############

clean: xml-clean wdata-clean
	rm -f libinput.a
