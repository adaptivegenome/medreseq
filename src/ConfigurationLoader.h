/*********************************************************************
 *
 * ConfigurationLoader.h:  The configuration loader used to load the
 * project level configurations.
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

#ifndef CONFIGURATIONLOADER_H_
#define CONFIGURATIONLOADER_H_

/**
 * The configuration holder class represents all the configuration information that are required
 * for the Medical Re-sequencing project.
 */
class ConfigurationHolder {

private:

	//Represents if there was an error in loading the configuration.
	bool error;

	//The correspoinding error message for the error.
	std::string errorMsg;

	//The length of sequences that we choose on either side of the target sequence.
	int seqArndLength;

	//The parameter for thermodynamic setting in primer3
	std::string termoConfigLoc;

	//The number of primer generated per sequence.
	int primersPerSequence;

	//The list of settings files in the order of preference.
	std::vector<std::string> settingsFiles;

public:
	//Consturctors
	ConfigurationHolder() {};
	ConfigurationHolder(int, std::vector<std::string> settingsFiles=std::vector<std::string>());

	//Getters and setters.
	bool isError() const;
	void setError(bool error);
	std::string getErrorMsg() const;
	void setErrorMsg(std::string errorMsg);
	int getSeqArndLength() const;
	void setSeqArndLength(int seqArndLength);
	std::vector<std::string> getSettingsFiles() const;
	void setSettingsFiles(std::vector<std::string> settingsFiles);
	std::string getTermoConfigLoc() const;
	void setTermoConfigLoc(std::string termoConfigLoc);
	int getPrimersPerSequence() const;
	void setPrimersPerSequence(int primersPerSequence);
};

/**
 * The Configuration Loader class is used to load the configurations required for the
 * project. If certain parameters are not available then it sets them to default value.
 */
class ConfigurationLoader {

public:

	//The name of the folder containing the settings.
	const static std::string ESSENTIALS_FOLDER_NAME;
	const static std::string FOLDER_DELIMITER;

	//The names of the parameters in the config-file
	const static std::string SEQUENCE_AROUND_LENGTH_NAME;
	const static std::string SETTINGS_PREFERENCE_FILES_NAME;
	const static std::string THERMO_CONFIG_LOCATION_NAME;
	const static std::string PRIMERS_PER_SEQUENCE_NAME;

	//The default values for the configuration parameters
	const static int SEQUENCE_AROUND_LENGTH_DEFAULT;
	const static std::string SETTINGS_PREFERENCE_FILE_DEFAULT;
	const static std::string THERMO_CONFIG_LOCATION_DEFAULT;
	const static int PRIMERS_PER_SEQUENCE_DEFAULT;

private:

	/*
	 * From the configuration parameters, obtain the list of settings files to be used.
	 */
	std::vector<std::string> static obtainSettingsFilesArray(std::string settingsStr, std::string delimiter);

public:
	/*
	 * This method loads the configuration from the config-file passed as the parameter and
	 * returns a configuration holder which contains the necessary configurations.
	 */
	ConfigurationHolder static obtainConfigurationSettings(std::string configFileLoc);

	/**
	 * This method applies the configuration settings required for all the static parameters
	 * in the project.
	 */
	void static applyConfigurationSetting(ConfigurationHolder &);
};

#endif /* CONFIGURATIONLOADER_H_ */
