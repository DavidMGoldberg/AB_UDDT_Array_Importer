//***************************************************************************
//
//  UDDT_Array_Importer.hpp
//
//  Created by David Goldberg
//
//  Description: This is the main 
//
//***************************************************************************/
#include "UDDT_Array_Importer.hpp"

int main(int argc, char *argv[])
{
	std::string 	UddtName,
			ProgramName,
			TagName,
			fileName,
			target,
			specificTarget,
			inputline;

	std::stringstream 	line,
						name,
						type,
						radix,
						RWN;
	std::ifstream ifs;
	std::ofstream ofs;

	unsigned int bitCounter = 0;
	int dimention = 0;
	int memberNum=0;

	std::list<TagData> data;
	std::list<TagData>::iterator it;

	//utf8-BOM encoding. basically just post at beginning of file
	const char utf8Bom[] = u8"\uFEFF";

/******************************************************************
	input arguments will define the program name, uddt name, tag name
*******************************************************************/
//add some error checking here
	ProgramName=argv[1];
	UddtName=argv[2];
	TagName=argv[3];
	fileName=argv[4];


/******************************************************************
		reading from the input.txt file
*******************************************************************/
	ifs.open(fileName.c_str());
	if (ifs.is_open()) cout << "input file opened successfully\n";
	else { std::cerr << "failed to open input file\n";    exit(-1); }

	getline(ifs, inputline);
	name.clear();
	name.str(inputline);
	cout << inputline << endl;
	getline(ifs, inputline);
	type.clear();
	type.str(inputline);
	cout << inputline << endl;
	getline(ifs, inputline);
	radix.clear();
	radix.str(inputline);
	cout << inputline << endl;
	getline(ifs, inputline);
	RWN.clear();
	RWN.str(inputline);
	cout << inputline << endl;

	while(!(line.rdbuf()->in_avail() == 0))
	{
		TagData element;
		name>>element.name;
		type>>element.datatype;
		radix>>element.radix;
		RWN>>element.RW;
		data.push_back(element);
	}

	int integerData;
	std::string stringData;
	float floatData;

	while (!ifs.eof())
	{
		getline(ifs, inputline);
		line.clear();
		line.str(inputline);
		if (line.rdbuf()->in_avail() == 0)break;
		for (it = data.begin(); it != data.end(); it++)
		{
			if (it->radix=="Decimal")
			{
				line >> integerData;
				it->int_data.push_back(integerData);
				cout << it->int_data.back()<<"\t";
			}
			if (it->radix=="Float" || it->radix=="Exponential")
			{
				line>>floatData;
				it->float_data.push_back(floatData);
				cout<<it->float_data.back()<<"\t";
			}
			else
			{
				line>>stringData;
				it->string_data.push_back(stringData);
				cout<<it->float_data.back()<<"\t";
			}

		}
		cout << endl;

	}

	dimention = data.front().int_data.size() + data.front().float_data.size()+data.front().string_data.size();
	cout <<"Dimention of the array:  " << dimention<<endl;
	ifs.close();
	if (ifs.is_open()) { std::cerr << "input file failed to close\n";		exit(-1); }
	else cout << "input file closed successfully" << endl;
/******************************************************************
		Generating Targets, bit numbers, and L5K data format
*******************************************************************/

	target = "ZZZZZZZZZZ"+UddtName.substr(0,10);
	for (it = data.begin(); it != data.end(); it++)
	{
		if (it->datatype == "BOOL") 
		{
			if (bitCounter == 0) 
			{
				specificTarget = target + std::to_string(memberNum);
				memberNum++;
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
		}
		else if (it->datatype == "INT") 
		{
			bitCounter = 0;
		}
		else if (it->datatype == "DINT") 
		{
			bitCounter = 0;
		}
		else if (it->datatype == "REAL") 
		{
			bitCounter = 0;
		}
		else;//error message

		memberNum++;
	}

	cout << "target: " << target << "# \n" <<endl;

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
			cout << "Value=\"";
			//actual data
			if(it->radix=="Decimal")cout<<it->int_data[i];
			else if (it->radix=="Float")cout<<std::fixed<<it->float_data[i];
			else if (it->radix=="Exponential")cout<<std::scientific<<it->float_data[i];
			else	cout<<it->string_data[i];

			cout<< "\"/>" << endl;
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
