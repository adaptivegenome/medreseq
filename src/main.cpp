/*********************************************************************
*
* main.cpp: entry point to the Medical Resequencing command line executable.
*
* Author: Sunil Kamalakar, VBI
* Last modified: 03 July 2012
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
#include "SequencesAdapter.h"

using namespace std;

const static string MEDRESEQ_CMD_USAGE_STR_DEFAULT = "Usage: ./MedReseq [OPTION] -f FILE -r region-input\n";
const static string MEDRESEQ_CMD_USAGE_STR_FULL = "Generate primers for the given input\n"
								"Example: ./MedReseq --fasta ucsc.hg19.fasta --region chr4:148375054-148375079 -o compOutput\n"
								"\t-f, --fasta \tfasta-file\n "
								"\t-r, --region\tregion OR regions-file OR vcf-file(.vcf)\n "
								"\t-s, --sequence\tsequence OR sequences-file(instead of -r)\n"
								"\t-o, --output\toptional output-file name(defaults to output, and .medreseq will be appended)\n"
								"\t-v, --verbose\toptional flag for verbose output";

const static string MAIN_CONFIG_FILE = "essentials/medreseq.config";
const static string OUTPUT_FILE_NAME_DEFAULT = "output";


bool isRegionInput(string input) {
	return Utility::regexMatch(input.c_str(), SamtoolsWrapper::REGION_INPUT_PATTERN.c_str());
}

bool isSequenceInput(string input) {
	return Utility::regexMatch(input.c_str(), SequencesAdapter::FASTA_SEQUENCE_PATTERN.c_str());
}

/*
 * The all important main method which starts the application.
 * It makes use of SamtoolsWrapper and Primer3Wrapper for performing the
 * computation of primers.
 */
int main(int argc, char **argv) {

	//Parameter which indicates if the second command line argument is a file or sequence
	bool cmdParamIsFile = true;

	//Parameter to indicate if the input provided is a sequence or a region
	bool isRegionInputVal = true;

	//Command line params
	string fastaFile = "";
	string sequenceOrRegionInput = "";
	string outputName = OUTPUT_FILE_NAME_DEFAULT;

	//Validate the command line arguments
	if(!(argc == 5 || argc == 7 || argc == 6 || argc == 8) ||
		argc <= 2 || (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))) {
		cerr << MEDRESEQ_CMD_USAGE_STR_DEFAULT;
		cerr << MEDRESEQ_CMD_USAGE_STR_FULL << endl;
		return EXIT_FAILURE;
	}

	//Parse the command line parameters and exit if format is not correct.
	 for (int i = 1; i < argc; i++) {
		if (i + 1 != argc) {
			if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--fasta")) {
				fastaFile = argv[++i];
				if(!Utility::fileExists(fastaFile)) {
					cerr << "Fasta-file does not exist. Aborting." << endl;
					return EXIT_FAILURE;
				}
			}
			else if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--region")) {
				sequenceOrRegionInput = argv[++i];
				bool regionsFileExists = Utility::fileExists(sequenceOrRegionInput);
				//Now check if the second argument is a file or a direct region input.
				if(!regionsFileExists) {
					if(isRegionInput(sequenceOrRegionInput)) {
						cmdParamIsFile = false;
					}
					else {
						cerr << "Regions file either as a list of regions or a VCF file does not exist.\n"
								"Or region should be in format regionName:startIndex-endIndex. Aborting." << endl;
						cerr << MEDRESEQ_CMD_USAGE_STR_DEFAULT;
						return EXIT_FAILURE;
					}
				}
			}
			else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--sequence")) {
				sequenceOrRegionInput = argv[++i];
				isRegionInputVal = false;
				bool sequenceFileExists = Utility::fileExists(sequenceOrRegionInput);
				//Now check if the second argument is a file or a direct region input.
				if(!sequenceFileExists) {
					if(isSequenceInput(sequenceOrRegionInput)) {
						cmdParamIsFile = false;
					}
					else {
						cerr << "Sequence file, "
								"or input sequence in format prev[target]next should be provided. Aborting." << endl;
						cerr << MEDRESEQ_CMD_USAGE_STR_DEFAULT;
						return EXIT_FAILURE;
					}
				}
			}
			else if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output")) {
				outputName = argv[++i];
			}
			else {
				if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) {
					Primer3Wrapper::IS_VERBOSE = true;
				}
				else {
					cerr << "Not enough or Invalid arguments." << endl;
					cerr << MEDRESEQ_CMD_USAGE_STR_DEFAULT;
					cerr << MEDRESEQ_CMD_USAGE_STR_FULL << endl;
					return EXIT_FAILURE;
				}
			}
		}
	 }

	//Obtain all the configurations and apply them.
	ConfigurationHolder configHolder = ConfigurationLoader::obtainConfigurationSettings(MAIN_CONFIG_FILE);
	ConfigurationLoader::applyConfigurationSetting(configHolder);

	//If there were any errors in the configuration just print them out
	if(configHolder.isError()) {
		cerr << configHolder.getErrorMsg() << endl;
	}

	//The start of the actual processing.
	cout << "Welcome to Medical Re-sequencing!" << endl;

	//The map of sequencename->sequence object.
	map<string, SequenceRegionOutput> sequencesMap;

	//The wrapper objects for both primer3 and samtools.
	SamtoolsWrapper samtoolsWrapper;
	Primer3Wrapper primer3Wrapper;

	//Load the fasta file.
	if(!samtoolsWrapper.loadFastaFile(fastaFile)) {
		cerr << "Error in loading the fasta index file" << fastaFile << endl;
		return EXIT_FAILURE;
	}

	//Now, based on the second input call the appropriate method.
	if(isRegionInputVal) {
		if(!cmdParamIsFile) {
			sequencesMap.insert(pair<string, SequenceRegionOutput>
									(sequenceOrRegionInput, samtoolsWrapper.retrieveSequenceForRegion(sequenceOrRegionInput)));
		}
		else {
			sequencesMap = samtoolsWrapper.retrieveSequencesForRegions(sequenceOrRegionInput);
		}
	}
	else {
		SequencesAdapter sequenceAdapter;
		if(!cmdParamIsFile) {
			sequencesMap.insert(pair<string, SequenceRegionOutput>
										("UNKNOWN", sequenceAdapter.retrieveSequenceObj("UNKNOWN", sequenceOrRegionInput)));
		}
		else {
			sequencesMap = sequenceAdapter.retrieveSequencesFromFile(sequenceOrRegionInput);
		}
	}

	cout << "Creating primers for " << sequencesMap.size() << " regions" << endl;

	//Now that we have the sequences, we can get the primers generated.
	vector<string> settingsFiles = configHolder.getSettingsFiles();
	primer3Wrapper.createPrimers(settingsFiles, sequencesMap, outputName);

	cout << "The comprehensive output file is: " <<
				outputName  << "." << Primer3Wrapper::PRIMER_COMPOSITE_FILE_EXTENSION << endl;
	cout << "The concise primers output files are: " <<
					outputName << "-*"  << "." << Primer3Wrapper::PRIMER_TERSE_FILE_EXTENSION << endl;

	return EXIT_SUCCESS;
}

