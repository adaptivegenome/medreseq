/*********************************************************************
*
* main.cpp: entry point to the Medical Resequencing command line executable.
*
* Author: Sunil Kamalakar, VBI
* Last modified: 20 June 2012
*
*********************************************************************
*
* This file is released under the Virginia Tech Non-Commercial
* Purpose License. A copy of this license has been provided in
* the project root directory.
*
*********************************************************************/

#include <iostream>
#include <fstream>
#include <regex.h>

#include "SamtoolsWrapper.h"
#include "libprimer3.h"
#include "Primer3Wrapper.h"
#include "SequenceRegions.h"
#include "Utility.h"
#include "ConfigurationLoader.h"
#include "VCFAdapter.h"

using namespace std;

const static string USAGE_STR = "Usage is ./MedReseq fasta-file sequence/sequence-file/vcf-file";
const static string MAIN_CONFIG_FILE = "essentials/medreseq.config";
const static string OUTPUT_FILE_NAME_DEFAULT = "output";


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

	//Command line params
	string fastaFile = argv[1];
	string regionsInput = argv[2];

	//Check if the fasta file exists.
	if(Utility::fileExists(fastaFile)) {

		bool regionsFileExists = Utility::fileExists(regionsInput);
		//Now check if the second argument is a file or a direct region input.
		if(!regionsFileExists) {
			if(isRegionInput(regionsInput)) {
				cmdParamIsFile = false;
			}
			else {
				cerr << "Sequence file either as a list of regions or a VCF file does not exist.\n"
							"Or Sequence should be in format regionName:startIndex-endIndex. Aborting." << endl;
				return EXIT_FAILURE;
			}
		}
	}
	else {
		cerr << "Fasta-file does not exist. Aborting." << endl;
		return EXIT_FAILURE;
	}

	//Obtain all the configurations and apply them.
	//TODO: Have a way for the config file to be passed in as a parameter to the binary.
	ConfigurationHolder configHolder = ConfigurationLoader::obtainConfigurationSettings(MAIN_CONFIG_FILE);
	ConfigurationLoader::applyConfigurationSetting(configHolder);

	//If there were any errors in the configuration just print them out
	if(configHolder.isError()) {
		cerr << configHolder.getErrorMsg() << endl;
	}

	cout << "Welcome to Medical Re-sequencing!" << endl;

	//The map of sequencename->sequence object.
	map<string, SequenceRegionOutput> sequencesMap;

	//The wrapper objects for both primer3 and samtools.
	SamtoolsWrapper samtoolsWrapper;
	Primer3Wrapper primer3Wrapper;

	//Load the fasta file.
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

	cout << "Creating primers for " << sequencesMap.size() << " regions" << endl;

	//Now that we have the sequences, we can get the primers generated.
	vector<string> settingsFiles = configHolder.getSettingsFiles();
	primer3Wrapper.createPrimers(settingsFiles, sequencesMap, OUTPUT_FILE_NAME_DEFAULT);

	cout << "The comprehensive output file is: " <<
				OUTPUT_FILE_NAME_DEFAULT  << "." << Primer3Wrapper::PRIMER_COMPOSITE_FILE_EXTENSION << endl;
	cout << "The concise primers output files are: " <<
					"*"  << "." << Primer3Wrapper::PRIMER_TERSE_FILE_EXTENSION << endl;

	return EXIT_SUCCESS;
}

