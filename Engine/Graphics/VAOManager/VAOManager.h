#pragma once

#include <string>
#include <map>

#include <glm/glm.hpp>

struct TrianglePlyFile
{
	unsigned int v0, v1, v2;
};

struct VertexInfo
{
	float x, y, z;
	float nx, ny, nz;
	float r, g, b, a;
};

struct ModelDrawInfo
{
	ModelDrawInfo();

	std::string meshName;

	glm::vec3 size;
	glm::vec3 minVertex;
	glm::vec3 maxVertex;

	unsigned int VAO_ID;
	unsigned int VERTEX_BUFFER_ID;
	unsigned int VERTEX_BUFFER_START_INDEX;

	unsigned int INDEX_BUFFER_ID;
	unsigned int INDEX_BUFFER_START_INDEX;

	unsigned int NUM_OF_VERTICES;
	unsigned int NUM_OF_INDICES;
	unsigned int NUM_OF_TRIANGLES;

	unsigned int* mIndices;
	VertexInfo* mVertices;
};

class VAOManager
{
public:

	VAOManager();

	~VAOManager();

	bool loadModelIntoVAO(std::string fileName, ModelDrawInfo& drawInfo, unsigned int shaderProgramID);

	bool findDrawInfoByModelName(std::string filename, ModelDrawInfo& drawInfo);

	void setBasePath(std::string basePath);

	std::string getBasePath();

	bool updateVAOBuffers(
		std::string fileName,
		ModelDrawInfo& updatedDrawInfo,
		unsigned int shaderProgramID
	);

private:

	bool loadPlyFile_XYZ_N_RGBA(std::string theFileName, ModelDrawInfo& drawInfo);

	std::map<std::string, ModelDrawInfo> pModelInfoMapByName;

	std::string pBasePath;

};