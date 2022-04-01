//***************************************************************************
//
//  UDDT_Array_Importer.hpp
//  
//  Created by David Goldberg
//
//	Description: 
//
//***************************************************************************

#ifndef UDDT_ARRAY_IMPORTER_HPP
#define UDDT_ARRAY_IMPORTER_HPP

#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <sstream>
#include <vector>
#include <limits>

using std::cout;
using std::cin;
using std::endl;



typedef struct TagData {
	std::vector<float> 	float_data;
	std::vector<int>	int_data;
	std::vector<std::string> string_data;
	std::string name;
	std::string datatype;
	std::string input_format;
	std::string RW;
	std::string radix;
	std::string target;
	unsigned int bit;
}TagData;

#endif
