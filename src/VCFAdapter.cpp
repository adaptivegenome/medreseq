/*********************************************************************
 *
 * VCFAdapter.cpp:  The adapter for VCF inputs.
 *
 * Author: Sunil Kamalakar, VBI
 * Last modified: 22 June 2012
 *
 *********************************************************************
 *
 * This file is released under the Virginia Tech Non-Commercial
 * Purpose License. A copy of this license has been provided in
 * the Medical Re-sequencing root directory.
 *
 *********************************************************************/
#include <fstream>
#include <sstream>
#include <ostream>
#include <iostream>
#include <vector>
#include <cstdlib>

#include "VCFAdapter.h"
#include "Utility.h"
#include "SequenceRegions.h"
#include "SamtoolsWrapper.h"


using namespace std;

//Initialize the static members
const std::string VCFAdapter::REGION_CHROMOSOME_PREFIX = "chr";
const int VCFAdapter::REGION_INDEX_PADDING = 5;
const std::string VCFAdapter::VCF_FILE_EXTENTION = ".vcf";
const std::string VCFAdapter::VCF_LINE_COMMENT_PATTERN = "\\s*#.*";
const std::string VCFAdapter::VCF_TEMP_REGIONS_FILE = "temp-vcf.regions";

bool VCFAdapter::isVCFFile(string fileLoc) {

	bool retVal = false;

	//TODO:we can also support ignore case for the extension
	if(fileLoc.find(VCF_FILE_EXTENTION) != string::npos) {
		retVal = true;
	}

	return retVal;
}

bool VCFAdapter::isValidVCFFile(string vcfFileLoc) {

	//TODO: Do the validation for the VCF file
	return true;
}

int VCFAdapter::findMaxReftoAltLen(string &reference, string &alternate) {

	unsigned int refLen = reference.length();
	unsigned int maxAltLen  = 0;

	vector<string> regionArr = Utility::split(alternate, ",");
	for(vector<string>::const_iterator iterator=regionArr.begin();
								iterator!=regionArr.end(); ++iterator) {
		string altStr(*iterator);
		Utility::trimString(altStr);
		maxAltLen = (maxAltLen > altStr.length())?maxAltLen:altStr.length();
	}

	int maxLen = (refLen > maxAltLen)?refLen:maxAltLen;

	return maxLen;
}

string VCFAdapter::convertVCFToRegion(string &chromosome, string &startIndexStr,
												string &reference, string &alternate) {

	string regionStr = "";
	long startIndex = 0;
	long endIndex = 0;
	int diffInRtoALen = 0;

	//If the name does not contain chr, add it
	if(chromosome.find(REGION_CHROMOSOME_PREFIX) == string::npos) {
		chromosome = REGION_CHROMOSOME_PREFIX + chromosome;
	}

	startIndex = atol(startIndexStr.c_str()) - REGION_INDEX_PADDING;
	endIndex = atol(startIndexStr.c_str()) + REGION_INDEX_PADDING;

	diffInRtoALen = findMaxReftoAltLen(reference, alternate);
	endIndex += diffInRtoALen;

	regionStr = SequenceRegionInput::convertToRegionFormat(chromosome, startIndex, endIndex);

	return regionStr;

}

string VCFAdapter::extractRegion(std::string &vcfLine) {

	//Split the string based on the \t delimiter
	//Interestingly if we give only \t without the white space it did not work.
	vector<string> regionArr = Utility::split(vcfLine, " \t");

	string chromosome = regionArr[CHROMOSOMES];
	string postition = regionArr[POSITION];
	string ref = regionArr[REF];
	string alt = regionArr[ALT];

	return convertVCFToRegion(chromosome, postition, ref, alt);
}

vector<string> VCFAdapter::extractRegions(string vcfFileLoc) {

	//Read the VCF file and extract the region info.
	vector<string> regionsVector;

	if(!isValidVCFFile(vcfFileLoc)) {
		cerr << "The vcf-file is not the intended format." << vcfFileLoc << "\n";
		return regionsVector;
	}

	string line = "";
	ifstream vcfFile (vcfFileLoc.c_str(), ifstream::in);
	if(vcfFile.is_open())
	{
		while(vcfFile.good()) {
			getline(vcfFile,line);
			if(!line.empty() && !Utility::regexMatch(line.c_str(), VCF_LINE_COMMENT_PATTERN.c_str())) {

				//Now extract the information for each line and add it to the vector
				string regionStr = extractRegion(line);

				if(Utility::regexMatch(regionStr.c_str(), SamtoolsWrapper::REGION_INPUT_PATTERN.c_str())) {
					regionsVector.push_back(regionStr);
					//cout << line << endl << regionStr << endl << endl;
				}
			}
		}
		vcfFile.close();
	}
	else {
		return regionsVector;
	}

	return regionsVector;
}

bool VCFAdapter::extractRegionsToFile(string vcfFileLoc, string regionsFileLoc) {

	bool retVal = false;

	if(!isValidVCFFile(vcfFileLoc)) {
		cerr << "The vcf-file is not the intended format." << vcfFileLoc << "\n";
		return retVal;
	}

	string line = "";
	ifstream vcfFile (vcfFileLoc.c_str(), ifstream::in);

	ofstream regionOutFile;
	regionOutFile.open(regionsFileLoc.c_str(), ios::out | ios::trunc);

	if(vcfFile.is_open() && regionOutFile.is_open()) {
		while(vcfFile.good()) {
			getline(vcfFile,line);
			if(!line.empty() && !Utility::regexMatch(line.c_str(), VCF_LINE_COMMENT_PATTERN.c_str())) {

				//Now extract the information for each line.
				string regionStr = extractRegion(line);

				if(Utility::regexMatch(regionStr.c_str(), SamtoolsWrapper::REGION_INPUT_PATTERN.c_str())) {
					regionOutFile << regionStr << endl;
					//cout << line << endl << regionStr << endl << endl;
					retVal = true;
				}
			}
		}
		vcfFile.close();
		regionOutFile.close();
	}
	else {
		return retVal;
	}

	return retVal;
}

