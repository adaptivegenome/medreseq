/*
 * Utility.h
 *
 *  Created on: Jun 14, 2012
 *      Author: sunil
 */

#ifndef UTILITY_H_
#define UTILITY_H_

#include <regex.h>
#include <fstream>

/*
 * The utility class which defines static methods.
 */
class Utility {

public:

	/**
	 * Utility method to check if a file exist or not.
	 * If the file exists then it returns true, if not returns false.
	 */
	static bool fileExists(std::string fileName) {
		bool retVal = true;

		std::ifstream file (fileName.c_str(), std::ifstream::in);
		if(file.fail()) {
			retVal = false;
		}

		return retVal;
	}

	/*
	 * Match text against the extended regular expression in
	 * pattern. Returns true for match, false for no match.
	 */
	static bool regexMatch(const char *text, const char *pattern)
	{
		bool retVal = false;
	    int	status;
	    regex_t regex;

	    if (regcomp(&regex, pattern, REG_EXTENDED|REG_NOSUB) != 0) {
	        return retVal;
	    }
	    status = regexec(&regex, text, (size_t) 0, NULL, 0);

	    regfree(&regex);

	    if (status == 0) {
	        retVal = true;
	    }

	    return retVal;
	}

	static int digitFromString(std::string str) {

		std::string data;

		for (unsigned int i = 0; i < str.length(); i++)
		{
			if (std::isdigit(str[i]))
				data += str[i];
		}

		return atoi(data.c_str());
	}

};

#endif /* UTILITY_H_ */
