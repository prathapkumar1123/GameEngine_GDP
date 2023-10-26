#pragma once

#include <string>
#include <map>

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

	bool loadModelIntoVAO(std::string fileName, ModelDrawInfo& drawInfo, unsigned int shaderProgramID);

	bool findDrawInfoByModelName(std::string filename, ModelDrawInfo& drawInfo);

	void setBasePath(std::string basePathWithoutSlash);

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