/*********************************************************************
*
* SequencesAdapter.cpp:  The adapter for taking in the sequence/sequence files
* 						directly from the command line.
*
* Author: Sunil Kamalakar, VBI
* Last modified: 03 July 2012
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

#include "SequencesAdapter.h"
#include "Utility.h"

using namespace std;

//The definition of the static variables.
const std::string SequencesAdapter::FASTA_SEQUENCE_NAME_PATTERN = "^>.+";
const std::string SequencesAdapter::FASTA_SEQUENCE_PATTERN = "^[ATGCatgcN]+\\[[ATGCatgcN]+\\][ATGCatgcN]+$";

map<string, SequenceRegionOutput> SequencesAdapter::retrieveSequencesFromFile(string seqfileLoc) {

	map<string, SequenceRegionOutput> sequencesMap;

	string line1 = "", line2 = "";
	if(Utility::fileExists(seqfileLoc)) {
		ifstream sequencesFile (seqfileLoc.c_str(), ifstream::in);
		if(sequencesFile.is_open()) {
			while(sequencesFile.good()) {
				line1 = "", line2 = "";
				//Get the first line in the name format and second line in sequence format.
				getline(sequencesFile,line1);
				if(!line1.empty() && Utility::regexMatch(line1.c_str(), FASTA_SEQUENCE_NAME_PATTERN.c_str())) {
					getline(sequencesFile, line2);
					if(!line2.empty() && Utility::regexMatch(line2.c_str(), FASTA_SEQUENCE_PATTERN.c_str())) {
						string sequenceName = line1.substr(1);
						sequencesMap.insert(pair<string, SequenceRegionOutput>
												(sequenceName, retrieveSequenceObj(sequenceName, line2)));
					}
				}
			}
			sequencesFile.close();
		}
	}
	else {
		cerr << "The sequence file does not exist: " << seqfileLoc << endl;
	}

	return sequencesMap;
}

SequenceRegionOutput SequencesAdapter::retrieveSequenceObj(string sequenceName, string sequence) {

	SequenceRegionInput seqInput = SequenceRegionInput(sequenceName, 0, 0);
	SequenceRegionOutput seqOut = SequenceRegionOutput(seqInput, string(sequence));

	return seqOut;

}


