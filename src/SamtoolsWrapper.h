/*
 * SamtoolsWrapper.h
 *
 *  Created on: Jun 8, 2012
 *      Author: Sunil Kamalakar
 */

#ifndef SAMTOOLSWRAPPER_H_
#define SAMTOOLSWRAPPER_H_

#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "sam.h"
#include "faidx.h"
#include "SequenceRegions.h"

/**
 * Samtools library is written in C, and this class acts as a wrapper around
 * the samtools to extract information using samtool api.
 */
class SamtoolsWrapper {

public:

	//The value which represents the number of bases we will pick
	//to the left and the right of the target sequence.
	const static int SEQUENCE_AROUND_LENGTH;
	const static std::string REGION_INPUT_PATTERN;

	/* The constructor and destructor */
	SamtoolsWrapper();
	~SamtoolsWrapper();

	/*
	 * This method sets the fasta index file to do the subsequent operation
	 * on the index file. It also loads the index file if not already loaded.
	 *
	 * It is necessary to perform this before we can perform any sequence
	 * retrieval from the fasta file.
	 *
	 * Returns a false value on failure.
	 */
	bool loadFastaFile(std::string fastaFileLoc);

	/*
	 * This method returns a key value pair of regions -> sequence
	 * for a given file which contains information about the regions.
	 */
	std::map<std::string, SequenceRegionOutput> retrieveSequencesForRegions(std::string regionfileLoc);

	/* This method provides a string containing the sequence for
	 * regions which is passed as parameter.
	 */
	SequenceRegionOutput retrieveSequenceForRegion(std::string region);

protected:

	/* The structure that corresponds to a particular fasta index file */
	faidx_t *fai;

	//The method used to retrieve the regions of interest.
	bool isValidRegionsFile(std::string fileLoc);
	bool validateRegionInput(SequenceRegionInput seqInput);
	std::vector<std::string> retrieveRegionsFromFile(std::string regionsFileLoc);
	SequenceRegionOutput retrieveSequencesAroundRegion(SequenceRegionInput &region);
	std::string retrieveSequenceForRegion(SequenceRegionInput &region);
};

#endif /* SAMTOOLSWRAPPER_H_ */
