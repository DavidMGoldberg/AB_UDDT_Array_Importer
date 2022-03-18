//***************************************************************************
//
//  UDDT_Array_Importer.hpp
//  
//  Created by David Goldberg
//
//	Description: 
//
//***************************************************************************

#pragma once

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
	std::vector<bool>	bool_data;
	std::string name;
	std::string datatype;
	std::string RW;
	std::string radix;
	std::string target;
	unsigned int bit;
};
