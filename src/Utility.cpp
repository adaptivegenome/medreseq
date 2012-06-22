/*********************************************************************
*
* Utility.h:  The utility class for the project.
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

#include <regex.h>
#include <fstream>
#include <vector>

#include "Utility.h"

using namespace std;

bool Utility::fileExists(std::string fileName) {
	bool retVal = true;

	std::ifstream file (fileName.c_str(), std::ifstream::in);
	if(file.fail()) {
		file.close();
		retVal = false;
	}

	file.close();
	return retVal;
}

bool Utility::regexMatch(const char *text, const char *pattern)
{
	bool retVal = false;
	int	status = -1;
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

int Utility::digitFromString(std::string str) {

	std::string data;

	for (unsigned int i = 0; i < str.length(); i++)
	{
		if (std::isdigit(str[i]))
			data += str[i];
	}

	//If there is no digit that exists, it returns a 0 value.
	return atoi(data.c_str());
}

void Utility::trimString(std::string &str) {

	str.erase(0, str.find_first_not_of(' '));
	str.erase(str.find_last_not_of(' ') + 1);
}

vector<string> Utility::split(std::string text, std::string delimiter) {

	 std::vector<std::string> retVal;

	 //We need to extract the required parameters.
	 char* textCharPtr = new char[text.length()];
	 std::strcpy(textCharPtr, text.c_str());
	 char* textIterator = NULL;
	 textIterator = std::strtok(textCharPtr, delimiter.c_str());

	 std::string val = "";
	 while(textIterator != NULL) {
		 val = std::string(textIterator);
		 retVal.push_back(val);
		 textIterator = std::strtok(NULL, delimiter.c_str());
	}

	delete textCharPtr;

	return retVal;
}
