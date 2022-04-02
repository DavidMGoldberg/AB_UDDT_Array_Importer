#
#
#
#

#Compiler Variables
CXX = g++
CXXFLAGS = -Wall -Werror -pedantic -std=c++11

#rules
UDDT_Array_Importer: UDDT_Array_Importer.o
	$(CXX) $(CXXFLAGS) -o UDDT_Array_Importer UDDT_Array_Importer.o

UDDT_Array_Importer.o: UDDT_Array_Importer.cpp UDDT_Array_Importer.hpp

clean:
	rm -f *.o UDDT_Array_Importer
