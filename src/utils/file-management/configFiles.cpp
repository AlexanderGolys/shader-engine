//
// Created by PC on 08.09.2025.
//

#include "configFiles.hpp"


string ConfigFile::operator[](const string &key) const{
	if (!config.contains(key))
		throw ValueError("Key " + key + " not found in config file", __FILE__, __LINE__);
	return config.at(key);
}

string ConfigFile::check(const string &key) const {
	if (!config.contains(key))
		throw ValueError("Key " + key + " not found in config file", __FILE__, __LINE__);
	return config.at(key);
}

Path ConfigFile::getRoot() const {
	return Path(check("root"));
}

Path ConfigFile::getMainShaderDirectory() const {
	return Path(check("shader-templates"));
}

Path ConfigFile::getSDFMacroShader() const {
	return Path(check("sdfTools"));
}

Path ConfigFile::getMathToolsShaderMacro() const {
	return Path(check("mathTools"));
}

Path ConfigFile::getLightToolsShaderMacro() const {
	return Path(check("lightTools"));
}

Path ConfigFile::getStructsShaderMacro() const {
	return Path(check("glslStructs"));
}

Path ConfigFile::getShadersDir() const {
	return Path(check("shadersDir"));
}

Path ConfigFile::getScreenshotsDir() const {
	return Path(check("screenshotsDir"));
}
