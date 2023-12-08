#include "ShaderManager.h"
#include "../../Core/OpenGlCommons.h"

#include <fstream>
#include <sstream>
#include <vector>

#include <algorithm>
#include <iterator>

const unsigned int MAXLINELENGTH = 8 * 1024;

/// <summary>
/// Shader Implementation
/// </summary>
Shader::Shader(std::string fileName) {
	this->id = 0;
	this->fileName = fileName;
	this->shaderType = eShaderType::UNKNOWN;
}

Shader::~Shader() {
}

std::string Shader::getShaderTypeString(void) {
	switch (this->shaderType) {
		case eShaderType::VERTEX_SHADER:
			return "VERTEX_SHADER";
			break;
		case eShaderType::FRAGMENT_SHADER:
			return "FRAGMENT_SHADER";
			break;
		case eShaderType::GEOMETRY_SHADER:
			return "GEOMETRY_SHADER";
			break;
		case eShaderType::UNKNOWN:
		default:
			return "UNKNOWN_SHADER_TYPE";
			break;
	}

	return "UNKNOWN_SHADER_TYPE";
}

void Shader::setShaderFileName(std::string fileName) {
	this->fileName = fileName;
}

std::string Shader::getShaderFileName() {
	return this->fileName;
}

/// <summary>
/// Shader Program Implementation
/// </summary>
bool ShaderProgram::loadUniformLocation(std::string variableName) {

	GLint uniLocation = glGetUniformLocation(this->id, variableName.c_str());

	if (uniLocation == -1) {
		return false;
	}

	this->mapUniformNameToLocation[variableName.c_str()] = uniLocation;

	return true;
}

// Look up the uniform location and save it.
int ShaderProgram::getUniformIDFromName(std::string name) {
	std::map<std::string, int>::iterator itUniform = this->mapUniformNameToLocation.find(name);

	if (itUniform == this->mapUniformNameToLocation.end()) {
		return -1;
	}

	return itUniform->second;
}


ShaderManager::ShaderManager() {
	return;
}

ShaderManager::~ShaderManager() {
	return;
}

bool ShaderManager::useShaderProgram(unsigned int id) {
	glUseProgram(id);
	return true;
}

bool ShaderManager::useShaderProgram(std::string shaderProgramName) {
	std::map<std::string, unsigned int>::iterator itShader = this->mapNameToID.find(shaderProgramName);

	if (itShader == this->mapNameToID.end()) {
		return false;
	}

	glUseProgram(itShader->second);
	return true;
}

bool ShaderManager::createProgramFromFile(std::string shaderName, Shader& vShader, Shader& gShader, Shader& fShader) {
	std::string errorText = "";

	// Shader loading happening before vertex buffer array
	vShader.id = glCreateShader(GL_VERTEX_SHADER);
	vShader.shaderType = eShaderType::VERTEX_SHADER;

	if (!this->pLoadSourceFromFile(vShader)) {
		return false;
	}

	errorText = "";
	if (!this->pCompileShaderFromSource(vShader, errorText))
	{
		this->m_lastError = errorText;
		return false;
	}

	gShader.id = glCreateShader(GL_GEOMETRY_SHADER);
	gShader.shaderType = eShaderType::GEOMETRY_SHADER;

	if (!this->pLoadSourceFromFile(gShader)) return false;
	if (!this->pCompileShaderFromSource(gShader, errorText)) {
		this->m_lastError = errorText;
		return false;
	}

	fShader.id = glCreateShader(GL_FRAGMENT_SHADER);
	fShader.shaderType = eShaderType::FRAGMENT_SHADER;

	if (!this->pLoadSourceFromFile(fShader)) return false;
	if (!this->pCompileShaderFromSource(fShader, errorText)) {
		this->m_lastError = errorText;
		return false;
	}

	ShaderProgram curProgram;
	curProgram.id = glCreateProgram();

	glAttachShader(curProgram.id, vShader.id);
	glAttachShader(curProgram.id, gShader.id);
	glAttachShader(curProgram.id, fShader.id);
	glLinkProgram(curProgram.id);

	// Was there a link error? 
	errorText = "";
	if (this->pWasThereALinkError(curProgram.id, errorText)) {
		std::stringstream ssError;
		ssError << "Shader program link error: ";
		ssError << errorText;
		this->m_lastError = ssError.str();
		return false;
	}

	// At this point, shaders are compiled and linked into a program
	curProgram.shaderProgramName = shaderName;

	// Add the shader to the map
	this->mapShaderProgramToID[curProgram.id] = curProgram;

	// Save to other map, too
	this->mapNameToID[curProgram.shaderProgramName] = curProgram.id;

	return true;
}

unsigned int ShaderManager::getIDFromName(std::string name) {
	std::map<std::string, unsigned int>::iterator itShad = this->mapNameToID.find(name);

	if (itShad == this->mapNameToID.end()) {
		return 0;
	}

	return itShad->second;
}

void ShaderManager::setBasePath(std::string basePath)
{
	this->pbasePath = basePath;
	return;
}

ShaderProgram* ShaderManager::pGetShaderProgramFromName(std::string name) {
	unsigned int shaderID = this->getIDFromName(name);

	std::map< unsigned int, ShaderProgram>::iterator
		itShad = this->mapShaderProgramToID.find(shaderID);

	if (itShad == this->mapShaderProgramToID.end()) {
		return NULL;
	}

	ShaderProgram* pShaderIdFound = &(itShad->second);

	return pShaderIdFound;
}


std::string ShaderManager::getLastError(void)
{
	std::string lastErrorTemp = this->m_lastError;
	this->m_lastError = "";
	return lastErrorTemp;
}


bool ShaderManager::pLoadSourceFromFile(Shader& shader)
{
	std::string fullFileName = this->pbasePath + "/" + shader.getShaderFileName();

	std::ifstream theFile(fullFileName.c_str());
	if (!theFile.is_open()) {
		return false;
	}

	shader.vecSource.clear();

	char pLineTemp[MAXLINELENGTH] = { 0 };
	while (theFile.getline(pLineTemp, MAXLINELENGTH)) {
		std::string tempString(pLineTemp);
		shader.vecSource.push_back(tempString);
	}

	theFile.close();
	return true;
}

bool ShaderManager::pCompileShaderFromSource(Shader& shader, std::string& error) {

	error = "";
	unsigned int numberOfLines = static_cast<unsigned int>(shader.vecSource.size());

	// This is an array of pointers to strings. aka the lines of source
	char** arraySource = new char* [numberOfLines];

	// Clear array to all zeros (i.e. '\0' or null terminator)
	memset(arraySource, 0, numberOfLines);

	for (unsigned int indexLine = 0; indexLine != numberOfLines; indexLine++) {
		unsigned int numCharacters = (unsigned int)shader.vecSource[indexLine].length();
		// Create an array of chars for each line
		arraySource[indexLine] = new char[numCharacters + 2];		// For the '\n' and '\0' at end
		memset(arraySource[indexLine], 0, numCharacters + 2);

		// Copy line of source into array
		for (unsigned int indexChar = 0; indexChar != shader.vecSource[indexLine].length(); indexChar++) {
			arraySource[indexLine][indexChar] = shader.vecSource[indexLine][indexChar];
		}

		// At a '\0' at end (just in case)
		arraySource[indexLine][numCharacters + 0] = '\n';
		arraySource[indexLine][numCharacters + 1] = '\0';
	}

	glShaderSource(shader.id, numberOfLines, arraySource, NULL);
	glCompileShader(shader.id);

	// Get rid of the temp source "c" style array
	for (unsigned int indexLine = 0; indexLine != numberOfLines; indexLine++)
	{	// Delete this line
		delete[] arraySource[indexLine];
	}
	// And delete the original char** array
	delete[] arraySource;

	// Did it work? 
	std::string errorText = "";
	if (this->pWasThereACompileError(shader.id, errorText)) {
		std::stringstream ssError;
		ssError << shader.getShaderTypeString();
		ssError << " compile error: ";
		ssError << errorText;
		error = ssError.str();
		return false;
	}

	return true;
}

bool ShaderManager::pWasThereACompileError(unsigned int shaderID, std::string & errorText) {
	errorText = "";

	GLint isCompiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

		char* pLogText = new char[maxLength];
		glGetShaderInfoLog(shaderID, maxLength, &maxLength, pLogText);
		errorText.append(pLogText);

		this->m_lastError.append("\n");
		this->m_lastError.append(errorText);

		delete[] pLogText;

		return true;
	}

	return false;
}

bool ShaderManager::pWasThereALinkError(unsigned int programID, std::string& errorText) {

	errorText = "";

	GLint wasError = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &wasError);
	if (wasError == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

		char* pLogText = new char[maxLength];
		glGetProgramInfoLog(programID, maxLength, &maxLength, pLogText);

		errorText.append(pLogText);

		this->m_lastError.append("\n");
		this->m_lastError.append(errorText);


		delete[] pLogText;

		return true;
	}

	return false;
}