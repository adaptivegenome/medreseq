/*********************************************************************
 *
 * Primer3Wrapper.cpp:  The facade wrapper around the primer3 library.
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


#include "Primer3Wrapper.h"
#include "libprimer3.h"
#include "thal.h"
#include "read_boulder.h"
#include "print_boulder.h"
#include "SequenceRegions.h"
#include "Utility.h"
#include "ConfigurationLoader.h"

#include <sstream>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

using namespace std;

//TODO: Do memory management for the C code, though the leak seems to grow extremely slowly.

//Assign the static members.
const std::string Primer3Wrapper::PRIMER_COMPOSITE_FILE_EXTENSION = "medreseq";
const std::string Primer3Wrapper::PRIMER_TERSE_FILE_EXTENSION = "primers";
const std::string Primer3Wrapper::PRIMER_ERROR_FILE_NAME = "not-found";

std::string Primer3Wrapper::PRIMER_THERMO_CONFIG_DEFAULT = "essentials/primer3_config/";

//=============================================================================================================
//SinglePrimer class methods.

float SinglePrimer::getAnyTh() const {
	return anyTh;
}

int SinglePrimer::getGcPercent() const {
	return gcPercent;
}

float SinglePrimer::getHairpin() const {
	return hairpin;
}

int SinglePrimer::getLength() const {
	return length;
}

float SinglePrimer::getMeltingTemperature() const {
	return meltingTemperature;
}

std::string SinglePrimer::getSequence() const {
	return sequence;
}

int SinglePrimer::getStartIndex() const {
	return startIndex;
}

void SinglePrimer::setAnyTh(float anyTh) {
	this->anyTh = anyTh;
}

void SinglePrimer::setGcPercent(int gcPercent) {
	this->gcPercent = gcPercent;
}

void SinglePrimer::setHairpin(float hairpin) {
	this->hairpin = hairpin;
}

void SinglePrimer::setLength(int length) {
	this->length = length;
}

void SinglePrimer::setMeltingTemperature(float meltingTemperature) {
	this->meltingTemperature = meltingTemperature;
}

void SinglePrimer::setSequence(std::string sequence) {
	this->sequence = sequence;
}

void SinglePrimer::setStartIndex(int startIndex) {
	this->startIndex = startIndex;
}

void SinglePrimer::setThreeTh(float threeTh) {
	this->threeTh = threeTh;
}

float SinglePrimer::getThreeTh() const {
	return threeTh;
}

//=============================================================================================================
//PrimerOutput class methods.

PrimerOutput::PrimerOutput(SequenceRegionOutput &seqOut, const  p3retval *p3retVal, const seq_args *sargs, string settingsFile) {

	//Fill in the default values for the pair
	this->seqOutput = seqOut;
	this->settingsFile = settingsFile;

	if(p3retVal != NULL) {

		//Fill in the parameters form from the p3retVal structure returned by primer3

		if(!pr_is_empty(&p3retVal->warnings)) {
			this->setWarning(string(p3retVal->warnings.data));
		}
		else {
			this->setWarning(string(""));
		}

		if(pr_is_empty(&p3retVal->glob_err) &&
								pr_is_empty(&p3retVal->per_sequence_err)) {

			//Atleast a single result needs to be returned
			if(p3retVal->best_pairs.num_pairs >= 1) {

				this->productSize = p3retVal->best_pairs.pairs[0].product_size;
				this->pairAnyThCompl = p3retVal->best_pairs.pairs[0].compl_any;
				this->pairThreeThCompl = p3retVal->best_pairs.pairs[0].compl_end;
				this->pairTemplateMisprimingTh = p3retVal->best_pairs.pairs[0].template_mispriming;

				// Pointers for the primer set just printing
				primer_rec *fwd, *rev;

				fwd  = p3retVal->best_pairs.pairs[0].left;
				rev  = p3retVal->best_pairs.pairs[0].right;

				//Values for the left primer.
				this->leftPrimer.setSequence(string(pr_oligo_sequence(sargs, fwd)));
				this->leftPrimer.setMeltingTemperature(fwd->temp);
				this->leftPrimer.setGcPercent(fwd->gc_content);
				this->leftPrimer.setStartIndex(fwd->start);
				this->leftPrimer.setLength((int)fwd->length);
				this->leftPrimer.setAnyTh(fwd->self_any);
				this->leftPrimer.setThreeTh(fwd->self_end);
				this->leftPrimer.setHairpin(fwd->hairpin_th);

				//Values for the right primer
				//Values for the left primer.
				this->rightPrimer.setSequence(string(pr_oligo_rev_c_sequence(sargs, rev)));
				this->rightPrimer.setMeltingTemperature(rev->temp);
				this->rightPrimer.setGcPercent(rev->gc_content);
				this->rightPrimer.setStartIndex(rev->start);
				this->rightPrimer.setLength((int)rev->length);
				this->rightPrimer.setAnyTh(rev->self_any);
				this->rightPrimer.setThreeTh(rev->self_end);
				this->rightPrimer.setHairpin(rev->hairpin_th);

			}
			else {
				this->setGlobalError("No primers were returned.");
			}
		}
		else {
			//There was some error.
			if(!pr_is_empty(&p3retVal->glob_err)) {
				this->setGlobalError(string(p3retVal->glob_err.data));
			}
			else {
				this->setGlobalError(string(""));
			}

			if(!pr_is_empty(&p3retVal->per_sequence_err)) {
				this->setSequenceError(string(p3retVal->per_sequence_err.data));
			}
			else {
				this->setSequenceError(string(""));
			}
		}
	}
	else {
		this->setGlobalError("Result was NULL!");
	}
}

SequenceRegionOutput PrimerOutput::getSeqOutput() {
	return seqOutput;
}

void PrimerOutput::setSeqOutput(SequenceRegionOutput seqOutput) {
	this->seqOutput = seqOutput;
}

SinglePrimer PrimerOutput::getLeftPrimer() const {
	return leftPrimer;
}

void PrimerOutput::setLeftPrimer(SinglePrimer leftPrimer) {
	this->leftPrimer = leftPrimer;
}

SinglePrimer PrimerOutput::getRightPrimer() const {
	return rightPrimer;
}

void PrimerOutput::setRightPrimer(SinglePrimer rightPrimer) {
	this->rightPrimer = rightPrimer;
}

long PrimerOutput::getPairAnyThCompl() const {
	return pairAnyThCompl;
}

void PrimerOutput::setPairAnyThCompl(long pairAnyThCompl) {
	this->pairAnyThCompl = pairAnyThCompl;
}

long PrimerOutput::getPairThreeThCompl() const {
	return pairThreeThCompl;
}

void PrimerOutput::setPairThreeThCompl(long pairThreeThCompl) {
	this->pairThreeThCompl = pairThreeThCompl;
}

int PrimerOutput::getProductSize() const {
	return productSize;
}

void PrimerOutput::setProductSize(int productSize) {
	this->productSize = productSize;
}

long PrimerOutput::getPairTemplateMisprimingTh() const {
	return pairTemplateMisprimingTh;
}

void PrimerOutput::setPairTemplateMisprimingTh(long pairTemplateMisprimingTh) {
	this->pairTemplateMisprimingTh = pairTemplateMisprimingTh;
}

std::string PrimerOutput::getGlobalError() const {
	return globalError;
}

void PrimerOutput::setGlobalError(std::string globalError) {
	this->globalError = globalError;
}

std::string PrimerOutput::getSequenceError() const {
	return sequenceError;
}

void PrimerOutput::setSequenceError(std::string sequenceError) {
	this->sequenceError = sequenceError;
}

std::string PrimerOutput::getWarning() const {
	return warning;
}

void PrimerOutput::setWarning(std::string warning) {
	this->warning = warning;
}

std::string PrimerOutput::getSettingsFile() const {
	return settingsFile;
}

void PrimerOutput::setSettingsFile(std::string settingsFile) {
	this->settingsFile = settingsFile;
}

//=============================================================================================================
//Primer3Settings class methods.


Primer3Settings::Primer3Settings(std::string settingsFile,
									p3_global_settings *globalSettings,
									seq_args *seqSettings, string thermoSettingsLoc) {

	this->settingsFile = settingsFile;
	this->globalSetting = globalSetting;
	this->sequenceSetting = seqSettings;
	this->thermoSettingsLoc = thermoSettingsLoc;
}

Primer3Settings::~Primer3Settings() {

	//Delete the global and sequence settings
	if(globalSetting != NULL) {

		if (globalSetting->thermodynamic_alignment == 1) {
				destroy_thal_structures();
		}

		p3_destroy_global_settings(globalSetting);
		globalSetting = NULL;
	}

	if(sequenceSetting != NULL) {
		destroy_seq_args(sequenceSetting);
		sequenceSetting = NULL;
	}
}

//=============================================================================================================
//Primer3Wrapper class methods.

Primer3Wrapper::Primer3Wrapper() {

}

Primer3Wrapper::~Primer3Wrapper() {

}

bool Primer3Wrapper::fillPrimerParameters(Primer3Settings &primerSettings) {

	bool retVal = false;

	string settingsFile = primerSettings.settingsFile;
	int dump_args = 0;

	read_boulder_record_results read_boulder_record_res = {0,0};

	pr_append_str p3_settings_path;
	pr_append_str fatal_parse_err;
	pr_append_str nonfatal_parse_err;
	pr_append_str warnings;

	init_pr_append_str(&fatal_parse_err);
	init_pr_append_str(&nonfatal_parse_err);
	init_pr_append_str(&warnings);
	init_pr_append_str(&p3_settings_path);

	primerSettings.globalSetting = p3_create_global_settings();

	if (!primerSettings.globalSetting) {
		cerr << "Global settings not created" << endl;
		destroy_pr_append_str_data(&nonfatal_parse_err);
		destroy_pr_append_str_data(&fatal_parse_err);
		destroy_pr_append_str_data(&warnings);
		return retVal;
	}

	primerSettings.globalSetting->dump = dump_args ;

	//Create the sequence settings struct.
	if (!(primerSettings.sequenceSetting = create_seq_arg())) {

		cerr << "Sequence settings not created" << endl;
		p3_destroy_global_settings(primerSettings.globalSetting);
		destroy_seq_args(primerSettings.sequenceSetting);
		destroy_pr_append_str_data(&nonfatal_parse_err);
		destroy_pr_append_str_data(&fatal_parse_err);
		destroy_pr_append_str_data(&warnings);
		return retVal;
	}

	//The path to the external settings file.
	pr_append_external(&p3_settings_path, settingsFile.c_str());

	//Read data from the file which is in boulder io fromat.
	if (!pr_is_empty(&p3_settings_path)) {
		read_p3_file(pr_append_str_chars(&p3_settings_path),
			 settings,
			 0, 0, primerSettings.globalSetting, primerSettings.sequenceSetting, &fatal_parse_err,
			 &nonfatal_parse_err, &warnings, &read_boulder_record_res);
		destroy_pr_append_str_data(&p3_settings_path);

	if (primerSettings.globalSetting->thermodynamic_alignment == 1)
		this->fillThermoPrimerParameters(primerSettings.thermoSettingsLoc);
	}

	//Make custom parameters adjustment to the global settings.
	primerSettings.globalSetting->num_return = 1;

	if (fatal_parse_err.data != NULL) {
		cerr << "Fatal error" << endl;
	}
	else {
		retVal = true;
	}

	destroy_pr_append_str_data(&nonfatal_parse_err);
	destroy_pr_append_str_data(&fatal_parse_err);
	destroy_pr_append_str_data(&warnings);

	return retVal;
}

bool Primer3Wrapper::fillThermoPrimerParameters(std::string themoSettingsPath) {

	bool retVal = false;

	thal_results thalRes;
	//if the path to the parameter files did not change, we don't need to read again
	if (thermodynamic_path_changed == 0)
		return retVal;
	//check that the path to the parameters folder was provided
	if (themoSettingsPath.empty()) {
		cerr << "Thermodynamic Alignment specified but path is empty. " <<
				" No thermodynamic parameters are going to be used." << endl;
		return retVal;
	}
	else {
		/* read in the thermodynamic parameters */
		if (get_thermodynamic_values(themoSettingsPath.c_str(), &thalRes)) {
			cerr << thalRes.msg << endl;
			return retVal;
		}

		retVal = true;
		// mark that the last given path was used for reading the parameters
		thermodynamic_path_changed = 0;
	}

	return retVal;
}

vector<PrimerOutput> Primer3Wrapper::createPrimers(vector<string> settingsFiles,
										map<string, SequenceRegionOutput> seqRegOuts) {

	vector<PrimerOutput> primerOuts;

	//Iterate over all the region inputs
	for(map<string, SequenceRegionOutput>::const_iterator iterator=seqRegOuts.begin();
						iterator!=seqRegOuts.end(); ++iterator) {


		SequenceRegionOutput seqOut = (*iterator).second;
		PrimerOutput primerOut = createPrimers(settingsFiles, seqOut);

		primerOuts.push_back(primerOut);
	}

	return primerOuts;
}

bool Primer3Wrapper::createPrimers(vector<string> settingsFiles,
									map<string, SequenceRegionOutput> seqRegOuts, string outputFileLoc) {

	bool retVal = false;

	//We need to create multiple files based on the settings files.
	int numOfSettingFiles = settingsFiles.size();

	//The names for the files and the file object itself.
	string primerFileName[numOfSettingFiles + 1];
	ofstream primersFile[numOfSettingFiles + 1]; //extra one for the errors
	bool isWrittenToFile[numOfSettingFiles + 1];

	string medseqFileName = outputFileLoc + "." + PRIMER_COMPOSITE_FILE_EXTENSION;
	ofstream medseqFile;
	medseqFile.open(medseqFileName.c_str(), ios::out | ios::trunc);

	for (int i = 0; i < numOfSettingFiles; ++i) {
		string extractedFileName = extractOutputFileName(string(settingsFiles[i]));
		primerFileName[i] = extractedFileName + "." + PRIMER_TERSE_FILE_EXTENSION;
		isWrittenToFile[i] = false;
	}
	primerFileName[numOfSettingFiles] = string(PRIMER_ERROR_FILE_NAME + "." + PRIMER_TERSE_FILE_EXTENSION);
	isWrittenToFile[numOfSettingFiles] = false;

	//Iterate over all the region inputs
	for(map<string, SequenceRegionOutput>::const_iterator iterator=seqRegOuts.begin();
						iterator!=seqRegOuts.end(); ++iterator) {

		SequenceRegionOutput seqOut = (*iterator).second;
		PrimerOutput primerOut = createPrimers(settingsFiles, seqOut);

		int fileIndex = getIndexOfFileToWrite(primerOut, settingsFiles);

		if(fileIndex >= 0 && fileIndex <= numOfSettingFiles) {

			//Lazy creation of file, only if needed.
			if(!isWrittenToFile[fileIndex]) {
				primersFile[fileIndex].open(primerFileName[fileIndex].c_str(), ios::out | ios::trunc);
				isWrittenToFile[fileIndex] = true;
			}

			medseqFile << printPrimer(primerOut);
			primersFile[fileIndex] << printPrimer(primerOut, true);

			retVal = true;
		}
	}

	//Close all the open files.
	if(medseqFile.is_open()) {
		medseqFile.close();
	}

	for (int i = 0; i < numOfSettingFiles; ++i) {
		if(primersFile[i].is_open()) {
			primersFile[i].close();
		}
	}

	//Also close the error file.
	if(primersFile[numOfSettingFiles].is_open()) {
		primersFile[numOfSettingFiles].close();
	}

	return retVal;
}

PrimerOutput Primer3Wrapper::createPrimers(vector<string> settingsFiles, SequenceRegionOutput &seqRegOut) {

	PrimerOutput primerOut;

	//Iterate through the list of settings files in order.
	for(vector<string>::const_iterator settingsIterator=settingsFiles.begin();
			settingsIterator!=settingsFiles.end(); ++settingsIterator) {

		primerOut = createPrimers(*settingsIterator, seqRegOut);

		//If there were no error's then just break out of the inner loop
		if(primerOut.getGlobalError().empty()) {
			break;
		}
	}

	return primerOut;
}

PrimerOutput Primer3Wrapper::createPrimers(string settingsFile, SequenceRegionOutput &seqRegOut) {

	PrimerOutput primerOut;
	p3_global_settings *global_pa = NULL;

	//TODO: We do not need to read the settings file everytime.
	//Optimization required to read it just once.
	//We could just keep a list of all the primer settings and pass them
	//around each time rather than filling the parmeter each time

	Primer3Settings primerSettings = Primer3Settings(settingsFile, global_pa, NULL, PRIMER_THERMO_CONFIG_DEFAULT);

	//Make sure that the path change param is set to 1 now,
	//because we would need to readjust this for each primer.
	thermodynamic_path_changed = 1;
	this->fillPrimerParameters(primerSettings);

	primerSettings.sequenceSetting = create_seq_arg();
	p3retval *retVal = createPrimers(primerSettings, seqRegOut);
	primerOut = PrimerOutput(seqRegOut, retVal, primerSettings.sequenceSetting, primerSettings.settingsFile);

	//Memory deallocation
	destroy_p3retval(retVal);

	return primerOut;
}


p3retval* Primer3Wrapper::createPrimers(Primer3Settings &primerSettings,
										SequenceRegionOutput &seqRegOutput) {

	/* Retval will point to the return value from choose_primers(). */
	p3retval *retval = NULL;

	//Set the sequence and target inforamtion.
	p3_set_sa_sequence(primerSettings.sequenceSetting, seqRegOutput.getCompleteSequence().c_str());
	p3_add_to_sa_tar2(primerSettings.sequenceSetting, (int)seqRegOutput.getPrevSequence().length(), (int)seqRegOutput.getTargetSequence().length());

	p3_set_gs_primer_file_flag(primerSettings.globalSetting, 0);
    retval = choose_primers(primerSettings.globalSetting, primerSettings.sequenceSetting);
    if (NULL == retval) {
    	//Let us return a NULL, it is upto the calling method to handle it.
    	return retval;
    }

	//print_boulder(4, primerSettings.globalSetting, primerSettings.sequenceSetting, retval, 0);

	return retval;

}

std::string Primer3Wrapper::outputPrintRegion(SequenceRegionInput region, bool isLeftPrimer) {

	string chromosomeToMatch = "chr";

	string regionPrintStr = region.getRegionName();
	size_t pos = regionPrintStr.find(chromosomeToMatch);
	if(pos != string::npos) {
		regionPrintStr = regionPrintStr.substr(pos + chromosomeToMatch.length());
	}

	stringstream temp;
	temp << region.getStartIndex();
	string startIndexStr = temp.str();
	string primerDirection = isLeftPrimer?"F":"R";
	return string(regionPrintStr + "-" + startIndexStr  + primerDirection);
}

void Primer3Wrapper::printPrimer(vector<PrimerOutput> primerOuts) {

	for(vector<PrimerOutput>::const_iterator iterator=primerOuts.begin();
		iterator!=primerOuts.end(); ++iterator) {
		cout << printPrimer((PrimerOutput)(*iterator));
	}
}

string Primer3Wrapper::extractOutputFileName(string fileName) {

	string retVal("");

	string exactFileName = fileName;
	if(fileName.find(ConfigurationLoader::ESSENTIALS_FOLDER_NAME) != string::npos) {
		exactFileName = Utility::split(fileName, ConfigurationLoader::FOLDER_DELIMITER)[1];
	}

	if(exactFileName.find(".") != string::npos) {
		retVal = Utility::split(exactFileName, ".")[0];
	}

	return retVal;
}

int Primer3Wrapper::getIndexOfFileToWrite(PrimerOutput &primerOut, vector<string> &textVector) {

	int retVal = -1;

	if(primerOut.getGlobalError().empty() && primerOut.getSequenceError().empty()) {
		string text = primerOut.getSettingsFile();
		for (unsigned int i = 0; i < textVector.size() ; ++i) {
			if(text == textVector[i]) {
				retVal = i;
				break;
			}
		}
	}
	else {
		//There was an error, so we need to give the index to error file.
		retVal = textVector.size();
	}

	return retVal;
}

string Primer3Wrapper::printPrimer(PrimerOutput primerOut, bool isTerse/*=false*/) {

	stringstream retVal("");

	if(!isTerse) {
		retVal << ">" << primerOut.getSeqOutput().getSeqRegInput().toString() << endl <<
								primerOut.getSeqOutput().getCompleteSequence(true) << endl;

			if(!primerOut.getGlobalError().empty()) {
				retVal << "ERROR: " << primerOut.getGlobalError() << endl << endl;
			}
			else if(!primerOut.getSequenceError().empty()) {
				retVal << "ERROR: " << primerOut.getSequenceError() << endl << endl;
			}
			else {

				if(!primerOut.getWarning().empty()) {
					retVal << "WARNING: " << primerOut.getWarning() << endl;
				}

				retVal << "Target: [" << primerOut.getSeqOutput().getPrevSequence().length() << "," <<
						primerOut.getSeqOutput().getTargetSequence().length() << "]" << endl <<
						"Settings file : " << primerOut.getSettingsFile() << endl <<
						this->outputPrintRegion(primerOut.getSeqOutput().getSeqRegInput(), true) << "=" <<
						primerOut.getLeftPrimer().getSequence() <<
						"| Start: " << primerOut.getLeftPrimer().getStartIndex()  + 1 << " " <<
						"| Length: " << primerOut.getLeftPrimer().getLength() << " " <<
						"| Temperature: " << (int)primerOut.getLeftPrimer().getMeltingTemperature() << " " <<
						"| GC%: " << primerOut.getLeftPrimer().getGcPercent() << endl <<
						this->outputPrintRegion(primerOut.getSeqOutput().getSeqRegInput(), false) << "=" <<
						primerOut.getRightPrimer().getSequence() <<
						"| Start: " << primerOut.getRightPrimer().getStartIndex() + 1 << " " <<
						"| Length: " << primerOut.getRightPrimer().getLength() << " " <<
						"| Temperature: " << (int)primerOut.getRightPrimer().getMeltingTemperature() << " " <<
						"| GC%: " << primerOut.getRightPrimer().getGcPercent() << endl <<
						"Overall Product size: " << primerOut.getProductSize() << endl << endl;
			}
	}
	else {

		if(!primerOut.getGlobalError().empty()) {
			retVal << this->outputPrintRegion(primerOut.getSeqOutput().getSeqRegInput(), true) << ", " <<
								  "ERROR: " << primerOut.getGlobalError() << endl <<
								  this->outputPrintRegion(primerOut.getSeqOutput().getSeqRegInput(), false) << ", " <<
								  "ERROR: " << primerOut.getGlobalError() << endl;
		}
		else if(!primerOut.getSequenceError().empty()) {
			retVal << this->outputPrintRegion(primerOut.getSeqOutput().getSeqRegInput(), true) << ", " <<
					  "ERROR: " << primerOut.getSequenceError() << endl <<
					  this->outputPrintRegion(primerOut.getSeqOutput().getSeqRegInput(), false) << ", " <<
					  "ERROR: " <<	primerOut.getSequenceError() << endl;
		}
		else {
			if(!primerOut.getWarning().empty()) {
				retVal << "WARNING: " << primerOut.getWarning() << endl;
			}

			retVal << this->outputPrintRegion(primerOut.getSeqOutput().getSeqRegInput(), true) << ", " <<
					primerOut.getLeftPrimer().getSequence() << endl <<
					this->outputPrintRegion(primerOut.getSeqOutput().getSeqRegInput(), false) << ", " <<
					primerOut.getRightPrimer().getSequence() << endl;
		}

	}


	return retVal.str();
}
//=============================================================================================================

