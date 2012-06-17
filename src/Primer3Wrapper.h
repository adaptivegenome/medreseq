/*
 * Primer3Wrapper.h
 *
 *  Created on: Jun 11, 2012
 *      Author: sunil
 */

#ifndef PRIMER3WRAPPER_H_
#define PRIMER3WRAPPER_H_

#include <string>

#include "SamtoolsWrapper.h"
#include "libprimer3.h"
#include "SequenceRegions.h"

/*
 * This class represents the properties of a single primer.
 * It represents a bean which holds the necessary values.
 */
class SinglePrimer {

public:
	SinglePrimer() {};
	SinglePrimer(p3retval) {};
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

protected:
	int startIndex;
	int length;
	float meltingTemperature;
	int gc_percent;
	float any_th;
	float three_th;
	float hairpin;
	std::string sequence;

};

/**
 * This class represents the overall output that is provided from primer3
 * for a given pair of primers. It holds information about the whole primer
 * and about the forward and reverse primers.
 */
class PrimerOutput {

public:
	PrimerOutput() {};
	PrimerOutput(SequenceRegionOutput &, const  p3retval*, const seq_args*);
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



protected:
	SequenceRegionOutput seqOutput;
	SinglePrimer leftPrimer;
	SinglePrimer rightPrimer;
	int productSize;
	long pairAnyThCompl;
	long pairThreeThCompl;
	long pairTemplateMisprimingTh;

	std::string globalError;
	std::string sequenceError;
	std::string warning;
};

/*
 * This class holds public member variable for the settings.
 * The values are based on the settingsFile used.
 */
class Primer3Settings {

public:
	Primer3Settings(std::string settingsFile, p3_global_settings *, seq_args *);
	std::string settingsFile;
	p3_global_settings *globalSetting;
	seq_args *sequenceSetting;
};

/**
 * The wrapper around the Primer3 which is written in C.
 * It provides the necessary methods to obtain primers for sequence regions.
 */
class Primer3Wrapper {

public:

	Primer3Wrapper();
	virtual ~Primer3Wrapper();

	std::vector<PrimerOutput> createPrimers(std::vector<std::string>, std::vector<SequenceRegionOutput> seqRegOuts);
	void printPrimer(std::vector<PrimerOutput>);
	std::string outputPrintRegion(SequenceRegionInput, bool);

protected:
	p3retval createPrimers(Primer3Settings &, SequenceRegionOutput &);
	bool fillInParameters(Primer3Settings &);
};

#endif /* PRIMER3WRAPPER_H_ */

