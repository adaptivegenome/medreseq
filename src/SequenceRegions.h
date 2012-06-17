/*
 * SequenceRegions.h
 *
 *  Created on: Jun 14, 2012
 *      Author: sunil
 */

#ifndef SEQUENCEREGIONS_H_
#define SEQUENCEREGIONS_H_

class SequenceRegionInput {

public:
	SequenceRegionInput(std::string, long, long);
	SequenceRegionInput(std::string);
	SequenceRegionInput() {
	}
	;
	std::string getRegionName();
	long getStartIndex();
	long getEndIndex();
	std::string toString();

protected:
	std::string regionName;
	long startIndex;
	long endIndex;

};

class SequenceRegionOutput {

public:
	SequenceRegionOutput(SequenceRegionInput &, std::string, std::string,
			std::string);
	SequenceRegionOutput() {
	}
	;
	SequenceRegionInput getSeqRegInput();
	std::string getPrevSequence();
	std::string getTargetSequence();
	std::string getNextSequence();
	std::string getCompleteSequence();

protected:
	SequenceRegionInput seqRegInput;
	std::string prevSequence;
	std::string targetSequence;
	std::string nextSequence;
};

class SequenceRegions {
public:
	SequenceRegions();
	virtual ~SequenceRegions();
};
#endif /* SEQUENCEREGIONS_H_ */

