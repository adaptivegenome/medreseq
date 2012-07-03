/*********************************************************************
*
* SequenceRegions.cpp:The regions wrapper which represents the output and
* 					  input structure for each region.
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


#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>

#include "SequenceRegions.h"
#include "Utility.h"

using namespace std;

//=============================================================================================================
//SequenceRegionInput class methods.

SequenceRegionInput::SequenceRegionInput(string region) {

	string regionName("");
	string rightStr("");
	long startIndex = 0;
	long endIndex = 0;

	//First split by : and then by -
	vector<string> regionArr = Utility::split(region, ":");
	if(!((string)regionArr[0]).empty() && !((string)regionArr[1]).empty()) {

		regionName = regionArr[0];
		rightStr = regionArr[1];

		//Reuse the same array.
		regionArr = Utility::split(rightStr, "-");
		if(!((string)regionArr[0]).empty() && !((string)regionArr[1]).empty()) {
			startIndex = atol(((string)regionArr[0]).c_str());
			endIndex = atol(((string)regionArr[1]).c_str());
		}
	}

	this->regionName = regionName;
	this->startIndex = startIndex;
	this->endIndex = endIndex;
}

//The implementations for the SequenceRegion class.
SequenceRegionInput::SequenceRegionInput(string regionName, long startIndex, long endIndex) {

	this->regionName = regionName;
	this->startIndex = startIndex;
	this->endIndex = endIndex;
}

string SequenceRegionInput::convertToRegionFormat(std::string regionName, long startIndex, long endIndex) {

	string retVal;

	string startIndexStr, endIndexStr;
	stringstream temp("");
	temp << startIndex;
	startIndexStr = temp.str();
	temp.str("");
	temp << endIndex;
	endIndexStr = temp.str();
	temp.str("");
	temp.clear();

	retVal = regionName + ":" + startIndexStr + "-" + endIndexStr;

	return retVal;
}

string convertToRegionFormat(std::string regionName, std::string startIndex, std::string endIndex) {

	string retVal = regionName + ":" + startIndex + "-" + endIndex;
	return retVal;
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

	string regionStr = convertToRegionFormat(this->getRegionName(),
												this->getStartIndex(), this->getEndIndex());
	return regionStr;
}

//=============================================================================================================
//SequenceRegionOutput class methods.

SequenceRegionOutput::SequenceRegionOutput(SequenceRegionInput &seqInput, string completeSequence) {

	string prev = "";
	string target = "";
	string next = "";

	size_t prevPos = completeSequence.find("[");
	prev = completeSequence.substr(0, prevPos);

	size_t nextPos = completeSequence.find("]");
	next = completeSequence.substr(nextPos + 1);

	target = completeSequence.substr(prevPos + 1, nextPos - prevPos - 1);

	this->prevSequence  = prev;
	this->targetSequence = target;
	this->nextSequence = next;
	this->seqRegInput = seqInput;
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

std::string SequenceRegionOutput::getCompleteSequence(bool isTargetEnclosed/*=false*/) {

	if(!isTargetEnclosed) {
		return string(prevSequence + targetSequence + nextSequence);
	}
	else {
		return string(prevSequence + "[" + targetSequence + "]" + nextSequence);
	}
}

//=============================================================================================================

