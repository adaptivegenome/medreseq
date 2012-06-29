/*********************************************************************
 *
 * SamtoolsWrapper.h:  The facade wrapper around the samtools library.
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

#include <iostream>
#include <fstream>

#include "SamtoolsWrapper.h"
#include "sam.h"
#include "SequenceRegions.h"
#include "Utility.h"
#include "VCFAdapter.h"
#include "ConfigurationLoader.h"

using namespace std;

//The initialization of the static variables
const string SamtoolsWrapper::REGION_INPUT_PATTERN = "^[A-za-z0-9]+:[0-9]+-[0-9]+$";
int SamtoolsWrapper::SEQUENCE_AROUND_LENGTH = ConfigurationLoader::SEQUENCE_AROUND_LENGTH_DEFAULT;

//The empty consturctor
SamtoolsWrapper::SamtoolsWrapper() {

}

//Destructor
SamtoolsWrapper::~SamtoolsWrapper() {

	//Delete the fai index
	if(fai != NULL) {
		free(fai);
	}

	//Delete the temporary regions file if it exists
	if(Utility::fileExists(VCFAdapter::VCF_TEMP_REGIONS_FILE)) {
		remove(VCFAdapter::VCF_TEMP_REGIONS_FILE.c_str());
	}
}

bool SamtoolsWrapper::loadFastaFile(string fastaFileLoc) {

	//Load the index file. This creates the fai file if it does not exist,
	//which is time consuming.
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

//TODO: The entire regions file is loaded into memory. If the regions file is too large
//this could lead to memory contraints. We should use a buffering scheme if we feel
//that we may need to load a very large file into memory.
vector<string> SamtoolsWrapper::retrieveRegionsFromFile(string fileLoc) {

	vector<string> regionsVector;

	//Here we plug in the VCF adapter if the file has extention VCF.
	if (VCFAdapter::isVCFFile(fileLoc)) {
		VCFAdapter vcfAdapter;
		regionsVector = vcfAdapter.extractRegions(fileLoc);
		return regionsVector;
	}
	else { //it is not a VCF file but a direct regions input file.
		if(!isValidRegionsFile(fileLoc)) {
			cerr << "The regions-file is not the intended format. Aborting! - " << fileLoc << "\n";
			return regionsVector;
		}

		const char* regionFileLocChar = fileLoc.c_str();
		string line = "";
		ifstream regionsFile (regionFileLocChar, ifstream::in);
		if(regionsFile.is_open()) {
			while(regionsFile.good()) {
				getline(regionsFile,line);
				if(!line.empty() && Utility::regexMatch(line.c_str(), REGION_INPUT_PATTERN.c_str())) {
					regionsVector.push_back(line);
				}
			}
			regionsFile.close();
		}
	}

	return regionsVector;
}

bool SamtoolsWrapper::validateRegionInput(SequenceRegionInput seqInput) {

	bool retVal = true;

	int startIndex = seqInput.getStartIndex();
	int endIndex = seqInput.getEndIndex();

	//Makes sure that we have the correct values for start and end indicies.
	//TODO: What if the sequences start before the length of the sequence around length,
	//in that case we do not create primers for those regions. Fix it if required.
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

		//Only create the required seq-out object if target has sequences in it.
		if(!target.empty()) {
			seqRegOutput = SequenceRegionOutput(region, prev, target, next);
		}
	}

	return seqRegOutput;
}

string SamtoolsWrapper::retrieveSequenceForRegion(SequenceRegionInput &region) {

	int readLength = 0;

	//Calling the samtools api for fetching the sequences in the given region.
	char *sequence = fai_fetch(fai, region.toString().c_str(), &readLength);

	string sequenceStr("");

	if(sequence != NULL) {
		sequenceStr = sequence;
		//The memory allocated by samtools needs to be deleted.
		free(sequence);
	}

	return sequenceStr;
}

//Public methods.
map<string, SequenceRegionOutput> SamtoolsWrapper::retrieveSequencesForRegions(string regionfileLoc) {
	map<string, SequenceRegionOutput> regionMap;

	//Obtain the regions first
	vector<string> regionsVector = this->retrieveRegionsFromFile(regionfileLoc);

	//For each region, add it and the sequences around obtained to the map.
	for(vector<string>::const_iterator iterator=regionsVector.begin();
				iterator!=regionsVector.end(); ++iterator) {
		SequenceRegionInput seqRegion = *iterator;
		regionMap.insert(pair<string, SequenceRegionOutput>(*iterator, retrieveSequencesAroundRegion(seqRegion)));
	}

	return regionMap;
}

SequenceRegionOutput SamtoolsWrapper::retrieveSequenceForRegion(string region) {

	SequenceRegionOutput regionSeqOut;

	//Create a new sequence region and the seq-output since there is only one
	//region to worry about.
	SequenceRegionInput seqRegion = SequenceRegionInput(region);
	regionSeqOut = this->retrieveSequencesAroundRegion(seqRegion);

	return regionSeqOut;
}
