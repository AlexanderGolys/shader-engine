#include "configFiles.hpp"


Path ConfigFile::getRoot() const {
	return Path(at("root"));
}

Path ConfigFile::getMainShaderDirectory() const {
	return Path(at("shader-templates"));
}

Path ConfigFile::getSDFMacroShader() const {
	return Path(at("sdfTools"));
}

Path ConfigFile::getMathToolsShaderMacro() const {
	return Path(at("mathTools"));
}

Path ConfigFile::getLightToolsShaderMacro() const {
	return Path(at("lightTools"));
}

Path ConfigFile::getStructsShaderMacro() const {
	return Path(at("glslStructs"));
}

Path ConfigFile::getShadersDir() const {
	return Path(at("shadersDir"));
}

Path ConfigFile::getScreenshotsDir() const {
	return Path(at("screenshotsDir"));
}
