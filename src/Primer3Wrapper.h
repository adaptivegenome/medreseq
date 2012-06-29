/*********************************************************************
 *
 * Primer3Wrapper.h:  The facade wrapper around the primer3 library.
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

#ifndef PRIMER3WRAPPER_H_
#define PRIMER3WRAPPER_H_

#include <string>
#include <ostream>

#include "SamtoolsWrapper.h"
#include "libprimer3.h"
#include "SequenceRegions.h"
#include "ConfigurationLoader.h"

/*
 * This class represents the properties of a single primer.
 * It represents a bean which holds the necessary values.
 */
class SinglePrimer {

protected:

	//The start index of the primer in the complete sequence
	int startIndex;

	//The length of the primer
	int length;

	//The melting temparature of the primer.
	float meltingTemperature;

	//Max allowed G and C's percent.
	int gcPercent;

	//Other properties of the primer
	float anyTh;
	float threeTh;
	float hairpin;

	//The acutal primer sequence.
	std::string sequence;

public:
	//Default constructor
	SinglePrimer() {};

	//The getters and setters.
	void setAnyTh(float anyTh);
	void setGcPercent(int gcPercent);
	void setHairpin(float hairpin);
	void setLength(int length);
	void setMeltingTemperature(float meltingTemperature);
	void setSequence(std::string sequence);
	void setStartIndex(int startIndex);
	void setThreeTh(float threeTh);
	float getAnyTh() const;
	int getGcPercent() const;
	float getHairpin() const;
	int getLength() const;
	float getMeltingTemperature() const;
	std::string getSequence() const;
	int getStartIndex() const;
	float getThreeTh() const;

};

/**
 * This class represents the overall output that is provided from primer3
 * for a given pair of primers. It holds information about the whole primer
 * and about the forward and reverse primers.
 */
class PrimerOutput {

protected:

	//The output of sequence regions.
	SequenceRegionOutput seqOutput;

	//The settings file used to generate this primer.
	std::string settingsFile;

	//The left primer
	SinglePrimer leftPrimer;

	//The right primer
	SinglePrimer rightPrimer;

	//Overall product size
	int productSize;

	//Other primer3 parameters
	long pairAnyThCompl;
	long pairThreeThCompl;
	long pairTemplateMisprimingTh;

	//Error and warning messages.
	std::string globalError;
	std::string sequenceError;
	std::string warning;

public:

	//Default constructor
	PrimerOutput() {};

	//The consturctor which takes in the necessary value from different primers and
	//creates the overall primer output object.
	PrimerOutput(SequenceRegionOutput &, const  p3retval*, const seq_args*, std::string);

	//The setters and getters.
	SequenceRegionOutput getSeqOutput();
	void setSeqOutput(SequenceRegionOutput seqOutput);
	SinglePrimer getLeftPrimer() const;
	void setLeftPrimer(SinglePrimer leftPrimer);
	long getPairAnyThCompl() const;
	void setPairAnyThCompl(long pairAnyThCompl);
	long getPairThreeThCompl() const;
	void setPairThreeThCompl(long pairThreeThCompl);
	int getProductSize() const;
	void setProductSize(int productSize);
	SinglePrimer getRightPrimer() const;
	void setRightPrimer(SinglePrimer rightPrimer);
	long getPairTemplateMisprimingTh() const;
	void setPairTemplateMisprimingTh(long pairTemplateMisprimingTh);
	std::string getGlobalError() const;
	void setGlobalError(std::string globalError);
	std::string getSequenceError() const;
	void setSequenceError(std::string sequenceError);
	std::string getWarning() const;
	void setWarning(std::string warning);
	std::string getSettingsFile() const;
	void setSettingsFile(std::string settingsFile);
};

/*
 * This class holds public member variable for the settings.
 * The values are based on the settingsFile used.
 * It just represents a object of all the settings used for the
 * generation of the primers.
 */
class Primer3Settings {

public:
	//The consturctor which requires the settings file, the global level parameters
	//and the sequence level parameters.
	Primer3Settings(std::string settingsFile, p3_global_settings *, seq_args *, std::string);
	~Primer3Settings();

	//The settings files used
	std::string settingsFile;

	//The path to the thermodynamic settings location
	std::string thermoSettingsLoc;

	//The global setting for primer3
	p3_global_settings *globalSetting;

	//The sequence level settings for primer3
	seq_args *sequenceSetting;
};

/**
 * The wrapper around the Primer3 which is written in C.
 * It provides the necessary methods to obtain primers for sequence regions.
 */
class Primer3Wrapper {

public:

	//The extension for the comprehensive output for the primers.
	const static std::string PRIMER_COMPOSITE_FILE_EXTENSION;

	//The extension for the precise output for the primers.
	const static std::string PRIMER_TERSE_FILE_EXTENSION;

	//The name of error file for primers
	const static std::string PRIMER_ERROR_FILE_NAME;

	//The link to the primer3 config required to themodynamic allignment.
	static std::string PRIMER_THERMO_CONFIG_DEFAULT;

	//The default constructor and destructor
	Primer3Wrapper();
	~Primer3Wrapper();

	/**
	 * The over-loaded method which accepts a list of settings files and a map of Sequence outputs
	 * and generates a list of primer outputs.
	 * The calling method has to be careful here because the entire output is in memory and could be
	 * dangerous to call this for large number of regions.
	 */
	std::vector<PrimerOutput> createPrimers(std::vector<std::string>,
												std::map<std::string, SequenceRegionOutput> seqRegOuts);

	/**
	 * The equivalent over-loaded method which accepts a list of settings files
	 * and a map of Sequence outputs and dumps the output in files represented by
	 * outputFile. This should be the method of choice for large input sizes.
	 */
	bool createPrimers(std::vector<std::string> settingsFiles,
						std::map<std::string, SequenceRegionOutput> seqRegOuts, std::string outputFile);

	/**
	 * Prints the entire set of primers in a formatted way to stdout.
	 */
	void printPrimer(std::vector<PrimerOutput>);

protected:

	/*
	 * This over-loaded method takes-in a list of settings files and sequence output
	 * to create a primer output. It walks through the settings files in order of
	 * preference.
	 */
	PrimerOutput createPrimers(std::vector<std::string>, SequenceRegionOutput &);

	/**
	 * This over-loaded method creates a primer output for a given settings file
	 * and sequence output. It is possible that the primers are not generated for
	 * the given settings.
	 */
	PrimerOutput createPrimers(std::string, SequenceRegionOutput &);

	/**
	 * The over-loaded method takes the primer settings and sequence output
	 * and creates returns a struct representation given by primer3 library.
	 */
	p3retval* createPrimers(Primer3Settings &, SequenceRegionOutput &);

	/**
	 * This method creates the string representation of the primer-output
	 * The terse parameter represents if the we need a short-hand output.
	 */
	std::string printPrimer(PrimerOutput primerOut, bool isTerse=false);

	/**
	 * The name construction of the output region to be printed.
	 * The current format is ChromosomeNumber-startIndex(F|R)
	 * The boolean parameter determines if it is a Forward or
	 * a Reverse primer.
	 */
	std::string outputPrintRegion(SequenceRegionInput, bool);

	/**
	 * This method is used to fill in the necessary paramers to the
	 * primer settings which can be used to create the primers.
	 */
	bool fillPrimerParameters(Primer3Settings &);

	/**
	 * This method is used to fill in the necessary thermodynamic parameters
	 * to the primer settings which is going to be used for thermodynamic allignment.
	 */
	bool fillThermoPrimerParameters(std::string);

	/**
	 * This method provides the index of the array for which the string match took place.
	 * Returns a -1 on failure.
	 */
	int getIndexOfFileToWrite(PrimerOutput &, std::vector<std::string> &);

	/**
	 * Provides the output name for the primers and medreseq files.
	 * This is based on the settings file used.
	 */
	std::string extractOutputFileName(std::string);

};

#endif /* PRIMER3WRAPPER_H_ */

