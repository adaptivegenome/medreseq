/*
 * Primer3Wrapper.cpp
 *
 *  Created on: Jun 11, 2012
 *      Author: sunil
 */

#include "Primer3Wrapper.h"
#include "thal.h"
#include "format_output.h"
#include "libprimer3.h"
#include "read_boulder.h"
#include "print_boulder.h"
#include "SequenceRegions.h"
#include "Utility.h"

#include <sstream>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

using namespace std;

//TODO: Do memory management for the C code.

static void read_thermodynamic_parameters();

float SinglePrimer::getAnyTh() const {
	return any_th;
}

int SinglePrimer::getGcPercent() const {
	return gc_percent;
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
	any_th = anyTh;
}

void SinglePrimer::setGcPercent(int gcPercent) {
	gc_percent = gcPercent;
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
	three_th = threeTh;
}

float SinglePrimer::getThreeTh() const {
	return three_th;
}

PrimerOutput::PrimerOutput(SequenceRegionOutput &seqOut, const  p3retval *p3retVal, const seq_args *sargs) {

	//Fill in the default values for the pair
	this->seqOutput = seqOut;

	if(p3retVal != NULL) {

		if(!pr_is_empty(&p3retVal->warnings)) {
			this->setWarning(string(p3retVal->warnings.data));
		}
		else {
			this->setWarning(string(""));
		}

		if(pr_is_empty(&p3retVal->glob_err) &&
								pr_is_empty(&p3retVal->per_sequence_err)) {

			this->productSize = p3retVal->best_pairs.pairs[0].product_size;
			this->pairAnyThCompl = p3retVal->best_pairs.pairs[0].compl_any;
			this->pairThreeThCompl = p3retVal->best_pairs.pairs[0].compl_end;
			this->pairTemplateMisprimingTh = p3retVal->best_pairs.pairs[0].template_mispriming;

			 // Pointers for the primer set just printing
			 primer_rec *fwd, *rev, *intl;

			 fwd  = p3retVal->best_pairs.pairs[0].left;
			 rev  = p3retVal->best_pairs.pairs[0].right;
			 intl = p3retVal->best_pairs.pairs[0].intl;

			//Values for the left primer.
			this->leftPrimer.setSequence(string(pr_oligo_sequence(sargs, p3retVal->best_pairs.pairs[0].left)));
			this->leftPrimer.setMeltingTemperature(p3retVal->best_pairs.pairs[0].left->temp);
			this->leftPrimer.setGcPercent(p3retVal->best_pairs.pairs[0].left->gc_content);
			this->leftPrimer.setStartIndex(p3retVal->best_pairs.pairs[0].left->start);
			this->leftPrimer.setLength((int)p3retVal->best_pairs.pairs[0].left->length);
			this->leftPrimer.setAnyTh(p3retVal->best_pairs.pairs[0].left->self_any);
			this->leftPrimer.setThreeTh(p3retVal->best_pairs.pairs[0].left->self_end);
			this->leftPrimer.setHairpin(p3retVal->best_pairs.pairs[0].left->hairpin_th);

			//Values for the right primer
			//Values for the left primer.
			this->rightPrimer.setSequence(string(pr_oligo_rev_c_sequence(sargs, p3retVal->best_pairs.pairs[0].right)));
			this->rightPrimer.setMeltingTemperature(p3retVal->best_pairs.pairs[0].right->temp);
			this->rightPrimer.setGcPercent(p3retVal->best_pairs.pairs[0].right->gc_content);
			this->rightPrimer.setStartIndex(p3retVal->best_pairs.pairs[0].right->start);
			this->rightPrimer.setLength((int)p3retVal->best_pairs.pairs[0].right->length);
			this->rightPrimer.setAnyTh(p3retVal->best_pairs.pairs[0].right->self_any);
			this->rightPrimer.setThreeTh(p3retVal->best_pairs.pairs[0].right->self_end);
			this->rightPrimer.setHairpin(p3retVal->best_pairs.pairs[0].right->hairpin_th);
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
		this->setGlobalError("Result was NULL!!!");
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

Primer3Settings::Primer3Settings(std::string settingsFile,
									p3_global_settings *globalSettings, seq_args *seqSettings) {

	this->settingsFile = settingsFile;
	this->globalSetting = globalSetting;
	this->sequenceSetting = seqSettings;
}

Primer3Wrapper::Primer3Wrapper() {
	// TODO Auto-generated constructor stub

}

Primer3Wrapper::~Primer3Wrapper() {
	// TODO Auto-generated destructor stub
}

bool Primer3Wrapper::fillInParameters(Primer3Settings &primerSettings) {

	//TODO: We do not need to read the settings file everytime.
	//Optimization required to read it just once.

	cout << "Reading settings file" << endl;

	bool retVal = false;

	string settingsFile = primerSettings.settingsFile;
	int io_version = 4;
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
		cout << "Global settings not created" << endl;
		return retVal;
	}

	primerSettings.globalSetting->dump = dump_args ;

	/* Settings files have to be read in just below, and
	 the functions need a temporary sarg */
	if (!(primerSettings.sequenceSetting = create_seq_arg())) {
		p3_destroy_global_settings(primerSettings.globalSetting);
		cout << "Global settings not created" << endl;
		return retVal;
	}

	pr_append_external(&p3_settings_path, settingsFile.c_str());
//	cout << seqRegOutput.getSeqRegInput().toString() << "=" << seqRegOutput.getCompleteSequence() << endl;
//	cout << "Settings: " << p3_settings_path.data << " + " << p3_settings_path.storage_size << endl;

	/* Read data from the settings file until a "=" line occurs.  Assign
	     parameter values for primer picking to pa and sa. */
	if (!pr_is_empty(&p3_settings_path)) {
		read_p3_file(pr_append_str_chars(&p3_settings_path),
			 settings,
			 0, 0, primerSettings.globalSetting, primerSettings.sequenceSetting, &fatal_parse_err,
			 &nonfatal_parse_err, &warnings, &read_boulder_record_res);
		destroy_pr_append_str_data(&p3_settings_path);

	if (primerSettings.globalSetting->thermodynamic_alignment == 1)
		read_thermodynamic_parameters();
	}

	//Make custom parameters adjustment to the global settings
	primerSettings.globalSetting->num_return = 1;

	if (fatal_parse_err.data != NULL) {

		cout << "Fatal error" << endl;
		  /* To avoid being distracted when looking for leaks: */
		  if (primerSettings.globalSetting->thermodynamic_alignment == 1)
		    destroy_thal_structures();
		  p3_destroy_global_settings(primerSettings.globalSetting);
		  primerSettings.globalSetting = NULL;
		  destroy_seq_args(primerSettings.sequenceSetting);
		  destroy_pr_append_str_data(&nonfatal_parse_err);
		  destroy_pr_append_str_data(&fatal_parse_err);
		  destroy_pr_append_str_data(&warnings);
		  destroy_dpal_thal_arg_holder();
	}
	else {
		retVal = true;
	}

	return retVal;
}

vector<PrimerOutput> Primer3Wrapper::createPrimers(vector<string> settingsFiles, vector<SequenceRegionOutput> seqRegOuts) {

	vector<PrimerOutput> primerOuts;

	string firstSettingsFile = settingsFiles.front();
	PrimerOutput primerOut;

	p3_global_settings *global_pa;
	seq_args *sarg;

	Primer3Settings primerSettings = Primer3Settings(firstSettingsFile, global_pa, NULL);
	this->fillInParameters(primerSettings);

	for(vector<SequenceRegionOutput>::const_iterator iterator=seqRegOuts.begin();
						iterator!=seqRegOuts.end(); ++iterator) {

		SequenceRegionOutput seqOut = *iterator;

		cout << "Before primer out" << endl;
		primerSettings.sequenceSetting = create_seq_arg();
		p3retval retVal = createPrimers(primerSettings, seqOut);
		cout << "After primer out" << endl;
		primerOut = PrimerOutput(seqOut, &retVal, primerSettings.sequenceSetting);
		cout << "After primer out" << endl;
		destroy_seq_args(primerSettings.sequenceSetting);
		cout << "After primer out" << endl;
		primerOuts.push_back(primerOut);
	}

	return primerOuts;
}


p3retval Primer3Wrapper::createPrimers(Primer3Settings &primerSettings,
										SequenceRegionOutput &seqRegOutput) {

	/* Retval will point to the return value from choose_primers(). */
	p3retval *retval = NULL;

	p3_set_sa_sequence(primerSettings.sequenceSetting, seqRegOutput.getCompleteSequence().c_str());
	p3_add_to_sa_tar2(primerSettings.sequenceSetting, (int)seqRegOutput.getPrevSequence().length(), (int)seqRegOutput.getTargetSequence().length());

    retval = choose_primers(primerSettings.globalSetting, primerSettings.sequenceSetting);
    if (NULL == retval) {
    	//Let us return a NULL, it is upto the calling method to handle it.
    	return *retval;
    }

	print_boulder(4, primerSettings.globalSetting, primerSettings.sequenceSetting, retval, 0);

	return *retval;

}

/* Reads the thermodynamic parameters if the thermodynamic alignment
   tag was set to 1 */
static void
read_thermodynamic_parameters() {

	//TODO: Implement this method.

}

std::string Primer3Wrapper::outputPrintRegion(SequenceRegionInput region, bool isLeftPrimer) {

	std::stringstream temp1, temp2;
	temp1 << Utility::digitFromString(region.getRegionName());
	string regionPrintStr = temp1.str();
	if(regionPrintStr.empty() || regionPrintStr == "0") {
		regionPrintStr = region.getRegionName();
	}
	temp2 << region.getStartIndex();
	string startIndexStr = temp2.str();
	string primerDirection = isLeftPrimer?"F":"R";
	return string(regionPrintStr + "-" + startIndexStr  + primerDirection + "=");

}

void Primer3Wrapper::printPrimer(vector<PrimerOutput> primerOuts) {

	for(vector<PrimerOutput>::const_iterator iterator=primerOuts.begin();
		iterator!=primerOuts.end(); ++iterator) {
		//cout << "For: " << iterator << endl;
		PrimerOutput pOut = *iterator;

		cout << pOut.getSeqOutput().getSeqRegInput().toString() << "=" <<
							pOut.getSeqOutput().getCompleteSequence() << endl;

		if(!pOut.getGlobalError().empty()) {
			cout << "ERROR: " << pOut.getGlobalError() << endl;
		}
		else if(!pOut.getSequenceError().empty()) {
			cout << "ERROR: " << pOut.getSequenceError() << endl;
		}
		else {

			if(!pOut.getWarning().empty()) {
				cout << "WARNING: " << pOut.getWarning() << endl;
			}

			cout << "Target:[" << pOut.getSeqOutput().getPrevSequence().length() << "," <<
					pOut.getSeqOutput().getTargetSequence().length() << "]" << endl <<
					this->outputPrintRegion(pOut.getSeqOutput().getSeqRegInput(), true) <<
					pOut.getLeftPrimer().getSequence() <<
					"| Start: " << pOut.getLeftPrimer().getStartIndex()  + 1 << " "
					"| Length: " << pOut.getLeftPrimer().getLength() << " "
					"| Temperature: " << pOut.getLeftPrimer().getMeltingTemperature() << " "
					"| GC%: " << pOut.getLeftPrimer().getGcPercent() << endl <<
					this->outputPrintRegion(pOut.getSeqOutput().getSeqRegInput(), false) <<
					pOut.getRightPrimer().getSequence() <<
					"| Start: " << pOut.getRightPrimer().getStartIndex() + 1 << " "
					"| Length: " << pOut.getRightPrimer().getLength() << " "
					"| Temperature: " << pOut.getRightPrimer().getMeltingTemperature() << " "
					"| GC%: " << pOut.getRightPrimer().getGcPercent() << endl <<
					"Overall Product size: " << pOut.getProductSize() << endl << endl;
		}
	}
}
