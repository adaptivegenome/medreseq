/*
 * main.cpp : the main program which starts the medical re-validation.
 *
 *  Created on: Jun 8, 2012
 *      Author: Sunil Kamalakar
 */

#include <iostream>
#include <fstream>
#include <regex.h>

#include "SamtoolsWrapper.h"
#include "libprimer3.h"
#include "Primer3Wrapper.h"
#include "SequenceRegions.h"
#include "Utility.h"

using namespace std;

const static string USAGE_STR = "Usage is ./MedReseq fasta-file sequence/sequence-file";
const static string PRIMER_SETTINGS_FILE = "primer3-2.3.4/primer3web_v3_0_0_default_settings.txt";

bool isRegionInput(string input) {
	return Utility::regexMatch(input.c_str(), SamtoolsWrapper::REGION_INPUT_PATTERN.c_str());
}

/*
 * The all important main method which starts the application.
 * It makes use of SamtoolsWrapper and Primer3Wrapper for performing the
 * computation of primers.
 */
int main(int argc, char **argv) {

	//Parameter which indicates if the second command line argument is a file or sequence
	bool cmdParamIsFile = true;

	//Validate the command line arguments.
	if(argc != 3) {
		cerr <<  USAGE_STR << endl;
		return EXIT_FAILURE;
	}

	//Check if the fasta file exists.
	if(Utility::fileExists(argv[1])) {

		bool regionsFileExists = Utility::fileExists(argv[2]);
		//Now check if the second argument is a file or a direct region input.
		if(!regionsFileExists) {
			if(isRegionInput(argv[2])) {
				cmdParamIsFile = false;
			}
			else {
				cerr << "Sequence file does not exist.\n"
							"Or Sequence should be in format regionName:startIndex-endIndex. Aborting." << endl;
				return EXIT_FAILURE;
			}
		}
	}
	else {
		cerr << "Fasta-file does not exist. Aborting." << endl;
		return EXIT_FAILURE;
	}

	//The map of sequencename->sequence object.
	map<string, SequenceRegionOutput> sequencesMap;

	SamtoolsWrapper samtoolsWrapper;
	Primer3Wrapper primer3Wrapper;

	if(!samtoolsWrapper.loadFastaFile(argv[1])) {
		cerr << "Error in loading the fasta index file" << argv[1] << endl;
		return EXIT_FAILURE;
	}

	//Now, based on the second input call the appropriate method.
	if(!cmdParamIsFile) {
		sequencesMap.insert(pair<string, SequenceRegionOutput>
								(argv[2], samtoolsWrapper.retrieveSequenceForRegion(argv[2])));
	}
	else {
		sequencesMap = samtoolsWrapper.retrieveSequencesForRegions(argv[2]);
	}

	vector<PrimerOutput> primerOuts;
	vector<SequenceRegionOutput> seqOuts;
	vector<string> settingsFiles;
	settingsFiles.push_back(PRIMER_SETTINGS_FILE);

	//Extract all the sequence outputs and put it in an vector.
	for( map<string, SequenceRegionOutput>::iterator ii=sequencesMap.begin();
				ii!=sequencesMap.end(); ++ii) {

		SequenceRegionOutput seqOut = (*ii).second;
		seqOuts.push_back(seqOut);
	}

	primerOuts = primer3Wrapper.createPrimers(settingsFiles, seqOuts);

	primer3Wrapper.printPrimer(primerOuts);

	return EXIT_SUCCESS;
}

