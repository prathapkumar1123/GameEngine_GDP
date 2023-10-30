#pragma once

#include <string>
#include <vector>
#include <map>

enum eShaderType { VERTEX_SHADER, FRAGMENT_SHADER, UNKNOWN };

class Shader {
public:
	Shader(std::string fileName);
	~Shader();
	
	unsigned int id;

	eShaderType shaderType;
	std::string getShaderTypeString(void);

	std::string getShaderFileName();

	void setShaderFileName(std::string fileName);

	std::vector<std::string> vecSource;

private:
	std::string fileName;
};

class ShaderProgram {
public:
	ShaderProgram() : id(0) {};
	~ShaderProgram() {};

	unsigned int id;

	std::string fileName;
	std::string shaderProgramName;
	std::map< std::string, int> mapUniformNameToLocation;

	// Returns -1 (just like OpenGL) if NOT found
	int getUniformIDFromName(std::string name);

	// Look up the uniform location and save it.
	bool loadUniformLocation(std::string variableName);
};

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	bool useShaderProgram(unsigned int id);
	bool useShaderProgram(std::string shaderProgramName);
	bool createProgramFromFile(std::string shaderName, Shader& vShader, Shader& fShader);

	void setBasePath(std::string basePath);
	unsigned int getIDFromName(std::string name);

	// Used to load the uniforms. Returns NULL if not found.
	ShaderProgram* pGetShaderProgramFromName(std::string name);

	// Clears last error
	std::string getLastError(void);

private:

	const std::string defaultShadersPath = "assets/shaders";

	// Returns an empty string if it didn't work
	bool pLoadSourceFromFile(Shader& shader);
	std::string pbasePath = "assets/shaders";

	bool pCompileShaderFromSource(Shader& shader, std::string& error);

	// returns false if no error
	bool pWasThereACompileError(unsigned int shaderID, std::string& errorText);
	bool pWasThereALinkError(unsigned int progID, std::string& errorText);

	std::string m_lastError;

	std::map<unsigned int, ShaderProgram> mapShaderProgramToID;
	std::map<std::string, unsigned int> mapNameToID;
};

