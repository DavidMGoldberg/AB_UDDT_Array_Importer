//***************************************************************************
//
//  UDDT_Array_Importer.hpp
//  
//  Created by David Goldberg
//
//	
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


template <typename T>
struct TagData {
	std::vector<T> data;
	std::string name;
	std::string datatype;
	std::string RW;
	std::string radix;
	std::string target;
	unsigned int bit;
};
