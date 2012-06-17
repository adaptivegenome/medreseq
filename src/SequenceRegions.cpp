/*
 * SequenceRegions.cpp
 *
 *  Created on: Jun 14, 2012
 *      Author: sunil
 */

#include <iostream>
#include <sstream>>

#include "SequenceRegions.h"

using namespace std;

SequenceRegionInput::SequenceRegionInput(string region) {

	//We need to extract the required parameters.
	char* regionCharPtr = new char[region.length()];
	strcpy(regionCharPtr, region.c_str());
	char* regionName = strtok(regionCharPtr, ":");
	long startIndex, endIndex;

	if(regionName != NULL) {
		char *rightStr = strtok(NULL, ":");
		if(rightStr != NULL) {
			startIndex = atoi(strtok(rightStr, "-"));
			endIndex = atoi(strtok(NULL, "-"));
		}
	}

	this->regionName = regionName;
	this->startIndex = startIndex;
	this->endIndex = endIndex;

	delete regionCharPtr;
}

//The implementations for the SequenceRegion class.
SequenceRegionInput::SequenceRegionInput(string regionName, long startIndex, long endIndex) {

	this->regionName = regionName;
	this->startIndex = startIndex;
	this->endIndex = endIndex;

}


string SequenceRegionInput::getRegionName() {
	return this->regionName;
}

long SequenceRegionInput::getStartIndex() {
	return this->startIndex;
}

long SequenceRegionInput::getEndIndex() {
	return this->endIndex;
}

string SequenceRegionInput::toString() {

	string regionName = this->getRegionName();
	string startIndex, endIndex;
	stringstream temp1, temp2;
	temp1 << this->getStartIndex();
	startIndex = temp1.str();
	temp2 << this->getEndIndex();
	endIndex = temp2.str();

	string regionStr = regionName + ":" + startIndex + "-" + endIndex;

	return regionStr;
}

SequenceRegionOutput::SequenceRegionOutput(SequenceRegionInput &seqInput, string prev, string target, string next) {

	this->seqRegInput = SequenceRegionInput(seqInput.toString());
	this->prevSequence  = prev;
	this->targetSequence = target;
	this->nextSequence = next;
}

SequenceRegionInput SequenceRegionOutput::getSeqRegInput() {
	return seqRegInput;
}

std::string SequenceRegionOutput::getPrevSequence() {
	return prevSequence;
}

std::string SequenceRegionOutput::getTargetSequence() {
	return targetSequence;
}

std::string SequenceRegionOutput::getNextSequence() {
		return nextSequence;
}

std::string SequenceRegionOutput::getCompleteSequence() {
	return string(prevSequence + targetSequence + nextSequence);
}

