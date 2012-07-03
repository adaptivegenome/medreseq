/*********************************************************************
 *
 * Utility.h:  The utility class for the project.
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

#ifndef UTILITY_H_
#define UTILITY_H_

#include <vector>
#include <string>

/*
 * The utility class which defines utility based static methods.
 */
class Utility {

public:

	/**
	 * Utility method to check if a file exist or not.
	 * If the file exists then it returns true, if not returns false.
	 */
	static bool fileExists(std::string);

	/*
	 * Match text against the extended regular expression in
	 * pattern. Returns true for match, false for no match.
	 */
	static bool regexMatch(const char *, const char *);

	/*
	 * This method extracts all the digits from a given string value.
	 */
	static int digitFromString(std::string);

	/**
	 * This method trim the string of white spaces o both sides.
	 */
	static void trimString(std::string &);

	/**
	 * This method is used to split a text based on a pattern.
	 * It returns a vector of strings which contains the split values.
	 */
	static std::vector<std::string> split(std::string, std::string);

};

#endif /* UTILITY_H_ */
