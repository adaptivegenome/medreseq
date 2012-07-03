/*********************************************************************
 *
 * VCFAdapter.h:  The adapter for VCF inputs.
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

#ifndef VCFEXTRACTOR_H_
#define VCFEXTRACTOR_H_

/**
 * The enumerator elements which corresponds to the index of the named parameter.
 * in the tab delimited VCF file.
 */
enum VCFLineParameters {
	CHROMOSOMES = 0,
	POSITION = 1,
	REF = 3,
	ALT = 4
};

/**
 * This class is responsible for extracting the necessary region information
 * from a VCF file.
 */
class VCFAdapter {

protected:

	//The prefix that exist for any chrosome in the region name.
	const static std::string REGION_CHROMOSOME_PREFIX;

	//The padding we will use for covering the target region.
	const static int REGION_INDEX_PADDING;

	//The pattern which is used to distinguish a VCF comment.
	const static std::string VCF_LINE_COMMENT_PATTERN;

	//The extension which is always present for any VCF file.
	const static std::string VCF_FILE_EXTENTION;

	//The extension which is always present for any VCF file.
	const static std::string VCF_LINE_TARGET_PATTERN;


public:

	//The temporary regions file created for VCF inputs.
	const static std::string VCF_TEMP_REGIONS_FILE;

protected:
	/*
	 * The validator which tells us if a given file is in the required
	 * VCF format of tab delimited parameters file.
	 */
	bool isValidVCFFile(std::string);

	/*
	 * This method finds the max length of the REF and ALT sequences.
	 */
	int findMaxReftoAltLen(std::string &, std::string &);

	/**
	 * This method takes in the required parameters from the VCF content and
	 * creates a string based region output from it.
	 */
	std::string convertVCFToRegion(std::string &, std::string &, std::string &, std::string &);

	/**
	 * This method extracts the contents from a given VCF line.
	 */
	std::string extractRegion(std::string &);

public:

	/*
	 * This static method tells if a given file is VCF file or not based on
	 * the extension of the file.
	 */
	bool static isVCFFile(std::string);

	/*
	 * This method is used to extract the regions from a given VCF file
	 * into a vector of regions.
	 */
	std::vector<std::string> extractRegions(std::string);

	/**
	 * This method is used to extract the regions from a given VCF file
	 * to given output file.
	 */
	bool extractRegionsToFile(std::string vcfFileLoc, std::string regionsFileLoc);
};


#endif /* VCFEXTRACTOR_H_ */
