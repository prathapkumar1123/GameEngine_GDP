#pragma once

#include <string>
#include <map>

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>

struct TrianglePlyFile
{
	unsigned int v0, v1, v2;
};

struct VertexInfo
{
	float x, y, z, w;
	float nx, ny, nz, nw;
	float r, g, b, a;
	float u, v;
};

struct BoundingBox3D {
	glm::vec3 min, max;

	BoundingBox3D() {}

	BoundingBox3D(const glm::vec3& _min, const glm::vec3& _max) : min(_min), max(_max) {}

	bool contains(const glm::vec3& point) {
		return (point.x >= min.x && point.x <= max.x &&
			point.y >= min.y && point.y <= max.y &&
			point.z >= min.z && point.z <= max.z);
	}

	bool intersects(const BoundingBox3D& other) {
		// Exit with no intersection if separated along an axis
		if (this->max.x < other.min.x || this->min.x > other.max.x) return false;
		if (this->max.y < other.min.y || this->min.y > other.max.y) return false;
		if (this->max.z < other.min.z || this->min.z > other.max.z) return false;

		// Overlapping on all axes means BoundingBoxes are intersecting
		return true;
	}
};

struct ModelDrawInfo
{
	ModelDrawInfo();

	std::string meshName;

	BoundingBox3D boundingBox;

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

	void setShaderProgramId(unsigned int shaderProgramId);

	bool loadModelIntoVAO(std::string fileName, ModelDrawInfo& drawInfo);

	bool findDrawInfoByModelName(std::string filename, ModelDrawInfo& drawInfo);

	void setBasePath(std::string basePath);

	bool loadModel(const std::string filepath, ModelDrawInfo& drawInfo);

	std::string getBasePath();

	bool updateVAOBuffers(std::string fileName, ModelDrawInfo& updatedDrawInfo);

private:

	bool loadPlyFile_XYZ_N_RGBA(std::string theFileName, ModelDrawInfo& drawInfo);

	bool loadTheFile_Ply_XYZ_N_RGBA_UV(std::string theFileName, ModelDrawInfo& drawInfo);

	std::map<std::string, ModelDrawInfo> pModelInfoMapByName;

	std::string pBasePath;

	unsigned int shaderProgramId;

};