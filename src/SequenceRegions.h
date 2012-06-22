/*********************************************************************
*
* SequenceRegions.h:  The regions wrapper which represents the output and
* 					  input structure for each region.
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


#ifndef SEQUENCEREGIONS_H_
#define SEQUENCEREGIONS_H_

/**
 * This class represents the object representation of the region input.
 */
class SequenceRegionInput {

protected:
	//The name of the regions. E.g. chr10
	std::string regionName;

	//The start index of the target region.
	long startIndex;

	//The end index of the target region.
	long endIndex;


public:
	//Constructor where each individual parameter is passed separately.
	SequenceRegionInput(std::string, long, long);

	//Constructor where the entire region string is passed.
	SequenceRegionInput(std::string);

	//Default constructor
	SequenceRegionInput() {};

	/**
	 * The static overloaded methods which provides the string format representaion
	 * of the regions from a given input. The string representation would look like
	 * chrName:startIndex-endIndex
	 */
	static std::string convertToRegionFormat(std::string, long, long);
	static std::string convertToRegionFormat(std::string, std::string, std::string);

	//The string representation of the region input object.
	std::string toString();

	//The getters.
	std::string getRegionName();
	long getStartIndex();
	long getEndIndex();
};

/**
 * This class represents the object representation of the region output.
 */
class SequenceRegionOutput {

protected:
	//The reference to the input for which this output exists.
	SequenceRegionInput seqRegInput;

	//The previous set of sequences for padding.
	std::string prevSequence;

	//The important target sequence.
	std::string targetSequence;

	//The next set of sequences for padding.
	std::string nextSequence;

public:
	//Constructors
	SequenceRegionOutput(SequenceRegionInput &, std::string, std::string, std::string);
	SequenceRegionOutput() {};

	//The getters and setters.
	SequenceRegionInput getSeqRegInput();
	std::string getPrevSequence();
	std::string getTargetSequence();
	std::string getNextSequence();

	//The method returns a concatenation of the complete sequence which incudes the
	//prev - target - next sequences. If the boolean target parameter is true, then
	//the target region will be enclosed by a square brackets. prev-[target]-next.
	std::string getCompleteSequence(bool isTargetEnclosed=false);
};
#endif /* SEQUENCEREGIONS_H_ */

