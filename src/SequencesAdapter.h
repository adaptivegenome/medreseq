/*********************************************************************
*
* SequencesAdapter.h:  The adapter for taking in the sequence/sequence files
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


#ifndef SEQUENCESADAPTER_H_
#define SEQUENCESADAPTER_H_

#include <string>
#include <map>

#include "SequenceRegions.h"

class SequencesAdapter {

public:

	//The name assigned to the sequence which starts with > symbol.
	const static std::string FASTA_SEQUENCE_NAME_PATTERN;

	//The actual sequence in the fasta format pattern.
	const static std::string FASTA_SEQUENCE_PATTERN;

	/**
	 * This method retrieves the sequences from the file which contains a fasta formatted
	 * list of sequences.
	 */
	std::map<std::string, SequenceRegionOutput> retrieveSequencesFromFile(std::string seqfileLoc);

	/**
	 * This method create a sequences output object from a given string sequence.
	 */
	SequenceRegionOutput retrieveSequenceObj(std::string sequenceName, std::string sequence);

};

#endif /* SEQUENCESADAPTER_H_ */
