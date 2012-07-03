/*********************************************************************
 *
 * ConfigurationLoader.cpp:  The configuration loader used to load the
 * project level configurations.
 *
 * Author: Sunil Kamalakar, VBI
 * Last modified: 03 July 2012
 *
 *********************************************************************
 *
  This file is released under the Virginia Tech Non-Commercial
 * Purpose License. A copy of this license has been provided in
 * the Medical Re-sequencing project root directory.
 *
 *********************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>

#include "ConfigurationLoader.h"
#include "SamtoolsWrapper.h"
#include "Primer3Wrapper.h"
#include "Utility.h"

using namespace std;

//The name of the folder containing files essential for medreseq.
const std::string ConfigurationLoader::ESSENTIALS_FOLDER_NAME = "essentials";
const std::string ConfigurationLoader::FOLDER_DELIMITER = "/";

//The names of parameters in the config file.
const std::string ConfigurationLoader::SEQUENCE_AROUND_LENGTH_NAME = "SEQUENCE_AROUND_LENGTH";
const std::string ConfigurationLoader::SETTINGS_PREFERENCE_FILES_NAME = "SETTINGS_PREFERENCE_FILES";
const std::string ConfigurationLoader::THERMO_CONFIG_LOCATION_NAME = "THERMO_CONFIG_LOCATION";
const std::string ConfigurationLoader::PRIMERS_PER_SEQUENCE_NAME = "PRIMERS_PER_SEQUENCE";

//The default values for the configuration parameters
const int ConfigurationLoader::SEQUENCE_AROUND_LENGTH_DEFAULT = 500;
const std::string ConfigurationLoader::SETTINGS_PREFERENCE_FILE_DEFAULT = ESSENTIALS_FOLDER_NAME + FOLDER_DELIMITER +
																		  "tier1.settings";
const std::string ConfigurationLoader::THERMO_CONFIG_LOCATION_DEFAULT = ESSENTIALS_FOLDER_NAME + FOLDER_DELIMITER +
																		"primer3_config" + FOLDER_DELIMITER;
const int ConfigurationLoader::PRIMERS_PER_SEQUENCE_DEFAULT = 1;

//=============================================================================================================
//Configuration holder class methods

ConfigurationHolder::ConfigurationHolder(int seqArndLen, vector<string> settingsFiles/*=vector<string>()*/){

	this->seqArndLength = seqArndLen;
	if(settingsFiles.empty()) {
		vector<string> settingsFilesDef;
		settingsFilesDef.push_back(ConfigurationLoader::SETTINGS_PREFERENCE_FILE_DEFAULT);
		settingsFiles = settingsFilesDef;
	}
	
	this->settingsFiles = settingsFiles;
}

bool ConfigurationHolder::isError() const {
	return error;
}

void ConfigurationHolder::setError(bool error) {
	this->error = error;
}

std::string ConfigurationHolder::getErrorMsg() const {
	return errorMsg;
}

void ConfigurationHolder::setErrorMsg(std::string errorMsg) {
	this->error = true;
	this->errorMsg = errorMsg;
}

int ConfigurationHolder::getSeqArndLength() const {
	return seqArndLength;
}

void ConfigurationHolder::setSeqArndLength(int seqArndLength) {
	this->seqArndLength = seqArndLength;
}

std::vector<std::string> ConfigurationHolder::getSettingsFiles() const {
	return settingsFiles;
}

std::string ConfigurationHolder::getTermoConfigLoc() const {
	return termoConfigLoc;
}

int ConfigurationHolder::getPrimersPerSequence() const {
	return primersPerSequence;
}

void ConfigurationHolder::setPrimersPerSequence(int primersPerSequence) {
	this->primersPerSequence = primersPerSequence;
}

void ConfigurationHolder::setTermoConfigLoc(std::string termoConfigLoc) {
	this->termoConfigLoc = termoConfigLoc;
}

void ConfigurationHolder::setSettingsFiles(std::vector<std::string> settingsFiles) {
	this->settingsFiles = settingsFiles;
}

//=============================================================================================================
//ConfigurationLoader class methods.

std::vector<std::string> ConfigurationLoader::obtainSettingsFilesArray(std::string settingsStr, std::string delimiter) {

	std::vector<std::string> settingsFiles;

	char* settingsStrPtr = new char[settingsStr.length()];
	std::strcpy(settingsStrPtr, settingsStr.c_str());

	char* settingsPtr = std::strtok(settingsStrPtr, delimiter.c_str());

	while(settingsPtr != NULL) {
		std::string settingsFile = std::string(settingsPtr);
		Utility::trimString(settingsFile);
		if(Utility::fileExists(settingsFile)) {
			settingsFiles.push_back(settingsFile);
		}
		else {
			std::cerr << "Settings file does not exist: " << settingsFile << std::endl;
		}

		settingsPtr = std::strtok(NULL, delimiter.c_str());
	}

	return settingsFiles;
}

ConfigurationHolder ConfigurationLoader::obtainConfigurationSettings(std::string configFileLoc) {

	std::string line;

	ConfigurationHolder configHolder(SEQUENCE_AROUND_LENGTH_DEFAULT);
	configHolder.setError(false);

	if(!Utility::fileExists(configFileLoc)) {
		configHolder.setErrorMsg("Config file does not exist. Using default configurations.");
		return configHolder;
	}

	//Read the file
	std::ifstream configFile (configFileLoc.c_str(), std::ifstream::in);
	if(configFile.is_open())
	{
		while(configFile.good())
		{
			getline(configFile,line);
			if(!line.empty()) {
				if(Utility::regexMatch(line.c_str(), SEQUENCE_AROUND_LENGTH_NAME.c_str())) {
					std::string seqArdStr = (Utility::split(line, "="))[1];
					int seqArdLength = atoi(seqArdStr.c_str());
					if(seqArdLength > 0 && seqArdLength < 5000) {
						configHolder.setSeqArndLength(seqArdLength);
					}
					else {
						stringstream msg;
						msg << "Using the default value for SEQUENCE_AROUND_LENGTH=" <<
										SEQUENCE_AROUND_LENGTH_DEFAULT << "\n";
						configHolder.setErrorMsg(msg.str());
						configHolder.setSeqArndLength(SEQUENCE_AROUND_LENGTH_DEFAULT);
					}
				}
				else if(Utility::regexMatch(line.c_str(), SETTINGS_PREFERENCE_FILES_NAME.c_str())) {
					std::string settingsStr = (Utility::split(line, "="))[1];
					configHolder.setSettingsFiles(obtainSettingsFilesArray(settingsStr, ","));

					if(configHolder.getSettingsFiles().empty()) {
						std::vector<std::string> settingsFiles;
						configHolder.setErrorMsg("Using the default value for SETTINGS_PREFERENCE_FILE=" +
													SETTINGS_PREFERENCE_FILE_DEFAULT + "\n");
						settingsFiles.push_back(SETTINGS_PREFERENCE_FILE_DEFAULT);
						configHolder.setSettingsFiles(settingsFiles);
					}
				}
				else if(Utility::regexMatch(line.c_str(), THERMO_CONFIG_LOCATION_NAME.c_str())) {
					std::string thermoConfigLoc = (Utility::split(line, "="))[1];
					if(!thermoConfigLoc.empty()) {
						configHolder.setTermoConfigLoc(thermoConfigLoc);
					}
					else {
						configHolder.setTermoConfigLoc(THERMO_CONFIG_LOCATION_DEFAULT);
					}
				}
				else if(Utility::regexMatch(line.c_str(), PRIMERS_PER_SEQUENCE_NAME.c_str())) {
					std::string primerPerSeqStr = (Utility::split(line, "="))[1];
					int primersPerSeq = atoi(primerPerSeqStr.c_str());
					if(!primerPerSeqStr.empty()) {
						configHolder.setPrimersPerSequence(primersPerSeq);
					}
					else {
						configHolder.setPrimersPerSequence(PRIMERS_PER_SEQUENCE_DEFAULT);
					}
				}
			}
		}
		configFile.close();
	}

	return configHolder;
}

void ConfigurationLoader::applyConfigurationSetting(ConfigurationHolder &configHolder) {

	SamtoolsWrapper::SEQUENCE_AROUND_LENGTH = configHolder.getSeqArndLength();
	Primer3Wrapper::PRIMER_THERMO_CONFIG_DEFAULT = configHolder.getTermoConfigLoc();
	Primer3Wrapper::PRIMERS_PER_SEQUENCE = configHolder.getPrimersPerSequence();
}

//=============================================================================================================
