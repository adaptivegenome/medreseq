/*********************************************************************
 *
 * SamtoolsWrapper.h:  The facade wrapper around the samtools library.
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
 * the samtools to extract sequence information from region inputs using the samtools API.
 */
class SamtoolsWrapper {

public:

	//The value which represents the number of bases we will pick
	//to the left and the right of the target sequence.
	static int SEQUENCE_AROUND_LENGTH;

	//The pattern which recognizes if a region is in the correct format.
	//This acts as a regex match.
	const static std::string REGION_INPUT_PATTERN;

public:
	/* The constructor and destructor */
	SamtoolsWrapper();
	~SamtoolsWrapper();

	/*
	 * This method sets the fasta index file to do the subsequent operation
	 * on the index file. It also loads the index file if not already loaded.
	 *
	 * It is necessary to call this before we can perform any sequence
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

	/**
	 * This method walks through the entire regions file and infers if the file contents
	 * are valid or not.
	 */
	bool isValidRegionsFile(std::string fileLoc);

	/**
	 * This method retrieves the regions from a given file and produces a array of
	 * regions. Each line of the input is compared with the REGION_INPUT_PATTERN
	 * regular expression based on which it is added to the array.
	 */
	std::vector<std::string> retrieveRegionsFromFile(std::string regionsFileLoc);

	/**
	 * This method recognizes if a given region input object is a valid region or not.
	 */
	bool validateRegionInput(SequenceRegionInput seqInput);

	/**
	 * Retrieves the sequences around the target regions by padding the target with
	 * SEQUENCE_AROUND_LENGTH of sequences on both sides of the target.
	 */
	SequenceRegionOutput retrieveSequencesAroundRegion(SequenceRegionInput &region);

	/**
	 * Retrieves the actual sequences for a given region input.
	 * It uses the samtools faidx to do so.
	 */
	std::string retrieveSequenceForRegion(SequenceRegionInput &region);
};

#endif /* SAMTOOLSWRAPPER_H_ */
