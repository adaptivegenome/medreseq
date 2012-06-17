/*
 * SamtoolsWrapper.cpp
 *
 *  Created on: Jun 8, 2012
 *      Author: sunil
 */

#include <iostream>
#include <fstream>

#include "SamtoolsWrapper.h"
#include "Sam.h"
#include "SequenceRegions.h"
#include "Utility.h"

using namespace std;

const string SamtoolsWrapper::REGION_INPUT_PATTERN = "^[A-za-z0-9]+:[0-9]+-[0-9]+$";
const int SamtoolsWrapper::SEQUENCE_AROUND_LENGTH = 500;

SamtoolsWrapper::SamtoolsWrapper() {

}

SamtoolsWrapper::~SamtoolsWrapper() {

	if(fai != NULL) {
		free(fai);
	}
}

bool SamtoolsWrapper::loadFastaFile(string fastaFileLoc) {

	//Load the index file.
	this->fai = fai_load(fastaFileLoc.c_str());

	if(fai == NULL) {
		return false;
	}
	else {
		return true;
	}
}

bool SamtoolsWrapper::isValidRegionsFile(string fileLoc) {
	//TODO: Write this piece of code.
	return true;
}

vector<string> SamtoolsWrapper::retrieveRegionsFromFile(string regionsFileLoc) {

	vector<string> regionsVector;

	if(!isValidRegionsFile(regionsFileLoc)) {
		cerr << "The regions-file is not the intended format. Aborting! - " << regionsFileLoc << "\n";
		return regionsVector;
	}

	const char* regionFileLocChar = regionsFileLoc.c_str();

	string line;
	ifstream regionsFile (regionFileLocChar, ifstream::in);
	if(regionsFile.is_open())
	{
		while(regionsFile.good())
		{
			getline(regionsFile,line);
			if(!line.empty() && Utility::regexMatch(line.c_str(), REGION_INPUT_PATTERN.c_str())) {
				regionsVector.push_back(line);
			}
		}
		regionsFile.close();
	}
	else {
		return regionsVector;
	}

	return regionsVector;
}

bool SamtoolsWrapper::validateRegionInput(SequenceRegionInput seqInput) {

	bool retVal = true;

	int startIndex = seqInput.getStartIndex();
	int endIndex = seqInput.getEndIndex();

	if(startIndex > endIndex ||
			startIndex < 0 || endIndex <= 0 ||
			(startIndex - SEQUENCE_AROUND_LENGTH) < 0 ||
			(endIndex + SEQUENCE_AROUND_LENGTH) <= 0) {
		retVal = false;
	}

	return retVal;
}

SequenceRegionOutput SamtoolsWrapper::retrieveSequencesAroundRegion(SequenceRegionInput &region) {

	SequenceRegionOutput seqRegOutput = SequenceRegionOutput(region, "", "", "");
	string prev, target, next;

	//Validate the input so that every thing is in order for the region.
	if(validateRegionInput(region)) {
		//Prev region.
		SequenceRegionInput prevRegion = SequenceRegionInput(region.getRegionName(),
									region.getStartIndex() - SEQUENCE_AROUND_LENGTH,
									region.getStartIndex() - 1);

		prev = this->retrieveSequenceForRegion(prevRegion);

		//Current region
		target = this->retrieveSequenceForRegion(region);

		//Next region
		SequenceRegionInput nextRegion = SequenceRegionInput(region.getRegionName(),
									region.getEndIndex() + 1,
									region.getEndIndex() + SEQUENCE_AROUND_LENGTH);

		next = this->retrieveSequenceForRegion(nextRegion);

		if(!target.empty()) {
			seqRegOutput = SequenceRegionOutput(region, prev, target, next);
		}
	}

	return seqRegOutput;
}

string SamtoolsWrapper::retrieveSequenceForRegion(SequenceRegionInput &region) {

	int readLength = 0;
	cout << "Before fetch" << endl;
	string sequence = fai_fetch(fai, region.toString().c_str(), &readLength);
	cout << "After fetch" << endl;
	return sequence;
}

//Public methods.
map<string, SequenceRegionOutput> SamtoolsWrapper::retrieveSequencesForRegions(string regionfileLoc) {
	map<string, SequenceRegionOutput> regionMap;

	vector<string> regionsVector = this->retrieveRegionsFromFile(regionfileLoc);
	for(vector<string>::const_iterator iterator=regionsVector.begin();
				iterator!=regionsVector.end(); ++iterator) {
		regionMap.insert(pair<string, SequenceRegionOutput>(*iterator, retrieveSequenceForRegion(*iterator)));
	}

	return regionMap;
}

SequenceRegionOutput SamtoolsWrapper::retrieveSequenceForRegion(string region) {

	SequenceRegionOutput regionSeqOut;

	//Create a new sequence region and the string output.
	SequenceRegionInput seqRegion = SequenceRegionInput(region);
	regionSeqOut = this->retrieveSequencesAroundRegion(seqRegion);

	return regionSeqOut;
}
