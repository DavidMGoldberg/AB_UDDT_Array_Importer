//***************************************************************************
//
//  UDDT_Array_Importer.hpp
//  
//  Created by David Goldberg
//
//	Description: 
//
//***************************************************************************
#include "CSV_to_l5x.h"

int main()
{
	
	std::string 	UddtName, 
					ProgramName,
					TagName, 
					varId, 
					configLine,
					inputline, 
					name, 
					type,
					RW, 
					radix, target, specificTarget, L5Kformat;

	std::stringstream line;
	std::ifstream ifs;
	std::ofstream ofs;
	float dataElement;
	
	unsigned int bitCounter = 0;
	int dimention = 0;
	int memberNum=0;

	std::list<TagData<float>> data;
	std::list<TagData<float>>::iterator it;

	//utf8-BOM encoding. basically just post at beginning of file
	const char utf8Bom[] = u8"\uFEFF";

/******************************************************************
	reading from the config.ini file. This file will contain settings for
	the operation of the consol  application (right now there are only 
	two things.)
*******************************************************************/
	ifs.open("config.ini");
	if (ifs.is_open()) cout << "config file opened successfully\n";
	else { std::cerr << "failed to open config file\n";    exit(-1); }
	while (!ifs.eof())
	{
		ifs >> varId;
		if (varId == "Program")
		{
			ifs >> ProgramName;
			cout << varId << "\t" << ProgramName << endl;
		}
		else if (varId == "UDDT")
		{
			ifs >> UddtName;
			cout << varId << "\t" << UddtName<<endl;
		}
		else if (varId == "TAG")
		{
			ifs >> TagName;
			cout << varId << "\t" << TagName << endl;
		}
		else if (varId == "EOF") break;

		else
		{std::cerr << "error in reading config file\n"; exit(-1);}
	}
	ifs.close();
	if (ifs.is_open()) { std::cerr << "config file failed to close\n";		exit(-1); }
	else cout << "config file closed successfully" << endl ;



/******************************************************************
	reading from the format.txt file
*******************************************************************/
	ifs.open("format.txt");
	if (ifs.is_open()) cout << "format file opened successfully\n";
	else { std::cerr << "failed to open format file\n";    exit(-1); }
	while (!ifs.eof())
	{
		line.clear();
		getline(ifs, configLine);
		//test to see if its reading correctly
//		cout << configLine << endl;
		line.str(configLine);
		line >> name >> type >> RW >> radix;
		if (name == "EOF") break;
		


		//load the format of the data on the list
		TagData<float> element;
		element.name = name;
		element.datatype = type;
		element.RW = RW;
		element.radix = radix;
		data.push_back(element);
		
	}
	ifs.close();
	if (ifs.is_open()) { std::cerr << "format file failed to close\n";		exit(-1); }
	else cout << "format file closed successfully" << endl;

/******************************************************************
		Generating Targets, bit numbers, and L5K data format
*******************************************************************/
	L5Kformat = "[";
	target = "ZZZZZZZZZZ"+UddtName.substr(0,10);
	for (it = data.begin(); it != data.end(); it++)
	{
		if (it->datatype == "BOOL") 
		{
			if (bitCounter == 0) 
			{
				specificTarget = target + std::to_string(memberNum);
				memberNum++;
				L5Kformat += "0,";
				it->target = specificTarget;
				it->bit = 0;
				bitCounter++;
			}
			else if (bitCounter > 0 && bitCounter < 7) 
			{
				it->target = specificTarget;
				it->bit = bitCounter;
				bitCounter++;
			}
			else if (bitCounter == 7) 
			{
				bitCounter = 0;
				it->target = specificTarget;
				it->bit = 7;
			}
			else;//error message

		}
		else if (it->datatype == "SINT") 
		{
			bitCounter=0;
			L5Kformat += "0,";
		}
		else if (it->datatype == "INT") 
		{
			bitCounter = 0;
			L5Kformat += "0,";
		}
		else if (it->datatype == "DINT") 
		{
			bitCounter = 0;
			L5Kformat += "0,";
		}
		else if (it->datatype == "REAL") 
		{
			bitCounter = 0;
			L5Kformat += "0.00000000e+000,";
		}
		else;//error message

		memberNum++;
	}
	L5Kformat.pop_back();
	L5Kformat += "]";
	cout << "target: " << target << "# \n" << "L5K: " << L5Kformat << endl;
/******************************************************************
		reading from the input.txt file
*******************************************************************/
	ifs.open("input.txt");
	if (ifs.is_open()) cout << "input file opened successfully\n";
	else { std::cerr << "failed to open input file\n";    exit(-1); }


	//must test first line of input file to make sure it agrees with the data configuration
	getline(ifs, inputline);
	//cout << inputline << endl;
	line.clear();
	line.str(inputline);
	for (it = data.begin(); it != data.end(); it++)
	{
		line >> name;
		if (it->name == name) 
		{
			cout << it->name << "\t" << it->datatype << "\t" << it->RW << "\t" << it->radix << "\t\tName Match\t";
			if (it->datatype == "BOOL")cout << it->target << "\t" << it->bit << endl;
			else cout << endl;
		}
		else {
			std::cerr << "ERROR: " << it->name << " does not match the name " << name << ": incongruity between input.txt and format.txt files";
			exit(-1);
		}
	}
	while (!ifs.eof())
	{
		getline(ifs, inputline);
		line.clear();
		line.str(inputline);
		if (line.rdbuf()->in_avail() == 0)break;
		for (it = data.begin(); it != data.end(); it++) 
		{
			line >> dataElement;
			it->data.push_back(dataElement);
			cout << it->data.back()<<"\t";
		}
		cout << endl;

	}

	dimention = data.front().data.size();
	cout <<"Dimention of the array:  " << dimention<<endl;
	ifs.close();
	if (ifs.is_open()) { std::cerr << "input file failed to close\n";		exit(-1); }
	else cout << "input file closed successfully" << endl;

/******************************************************************
		writing the target.L5X file
*******************************************************************/
	cout << "\n\nWriting to .L5X file\n";
	ofs.open("target.L5X", std::ofstream::out | std::ofstream::trunc);
	std::streambuf* coutbuf = cout.rdbuf();
	if (!ofs.is_open()) { std::cerr << "unable to open output file\n"; exit(-1); }
	cout.rdbuf(ofs.rdbuf());
	//writing the top of the xml file
	cout << utf8Bom <<"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"<<endl;
	cout << "<RSLogix5000Content SchemaRevision=\"1.0\" SoftwareRevision=\"31.00\" TargetName=\"" << ProgramName << "\" TargetType=\"Program\" ContainsContext=\"true\" Owner=\"fai\" ExportDate=\"Mon Jan 17 13:58:27 2022\" ExportOptions=\"References NoRawData L5KData DecoratedData Context Dependencies ForceProtectedEncoding AllProjDocTrans\">" << endl;
	cout << "<Controller Use=\"Context\" Name=\"dataImportExportTest\">"<< endl<<"<DataTypes Use=\"Context\">\n"<<"<DataType Name=\""<<UddtName<< "\" Family=\"NoFamily\" Class=\"User\">"<<"\n<Members>\n";
	//data members
	//here we will define the UDDT in the output file
	for (it = data.begin(); it != data.end(); it++)
	{
		if(it->datatype=="BOOL")
		{
			if (it->bit == 0){cout << "<Member Name=\"" << it->target << "\" DataType=\"SINT\" Dimension=\"0\" Radix=\"Decimal\" Hidden=\"true\" ExternalAccess=\"Read/Write\"/>" << endl;}
			cout << "<Member Name=\"" << it->name << "\" DataType=\"BIT\" Dimension=\"0\" Radix=\"Decimal\" Hidden=\"false\" Target=\"" << it->target << "\" BitNumber=\"" << it->bit << "\" ExternalAccess=\"";
			if (it->RW == "W")cout << "Read/Write\"/>" << endl;
			else if (it->RW == "R")cout << "Read Only\"/>" << endl;
			else if (it->RW == "N")cout << "None\"/>" << endl;
			else { std::cerr << "Problem with RW parameter"; exit(-1); }
		}
		else
		{
			cout << "<Member Name=\"" << it->name << "\" DataType=\"" << it->datatype << "\" Dimension=\"0\" Radix=\"" << it->radix << "\" Hidden=\"false\" ExternalAccess=\"";
			if (it->RW == "W")cout << "Read/Write\"/>" << endl;
			else if (it->RW == "R")cout << "Read Only\"/>" << endl;
			else if (it->RW == "N")cout << "None\"/>" << endl;
			else { std::cerr << "Problem with RW parameter"; exit(-1); }
		}
	}
	cout << "</Members>\n</DataType>\n</DataTypes>\n";

	//Here we will define our dummy program
	cout << "<Programs Use=\"Context\">" << endl;
	cout << "<Program Use=\"Target\" Name=\"" << ProgramName << "\" TestEdits=\"false\" UseAsFolder=\"false\">" << "\n<Tags>\n";
	cout << "<Tag Name=\"" << TagName << "\" TagType=\"Base\" DataType=\"" << UddtName << "\" Dimensions=\"" << dimention << "\" Constant=\"true\" ExternalAccess=\"None\">" << endl;
	//l5k
	cout << "<Data Format=\"L5K\">" << endl<<"<![CDATA[["<<L5Kformat;
	//we are gonna reuse bitcounter here because why use 8 extra bytes
	bitCounter = 1;
	for (int i = 1; i < dimention; i++)
	{
		bitCounter++;
		if (bitCounter == 5) {
			cout << "\n\t\t\t\t\t"; bitCounter = 0;
		}
		cout << "," << L5Kformat;
	}

	cout << "]]]>" << endl << "</Data>" << endl;

	//decorated data
	cout << "<Data Format=\"Decorated\">\n" << "<Array DataType=\""<<UddtName<<"\" Dimensions=\""<<dimention<<"\">" << endl;
	for (int i = 0; i < dimention; i++)
	{
		cout << "<Element Index=\"["<<i<<"]\">" << endl;
		cout << "<Structure DataType=\"" << UddtName << "\">" << endl;

		for (it = data.begin(); it != data.end(); it++)
		{
			cout << "<DataValueMember Name=\"" << it->name << "\" DataType=\"" << it->datatype << "\" ";
			if (it->datatype != "BOOL")cout << "Radix=\"" << it->radix << "\" ";
			cout << "Value=\"" << it->data[i] << "\"/>" << endl;
		}

		cout << "</Structure>\n</Element>" << endl;
	}
	cout<< "</Array>\n</Data>" << endl;

	//tail
	cout << "</Tag>\n</Tags>\n<Routines/>\n</Program>\n</Programs>\n</Controller>\n</RSLogix5000Content>\n";
	ofs.close();

	//switch back to original Cout buffer
	cout.rdbuf(coutbuf);
	//pause exiting the console app so people can look at the output
	cout << "finished Writing\n\n";
	cout << "press enter to exit ";
	cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');

	return 0;
}
