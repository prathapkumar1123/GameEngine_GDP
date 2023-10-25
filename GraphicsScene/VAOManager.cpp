#include "VAOManager/VAOManager.h"

#include "Globals/OpenGLCommons.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>
#include <sstream>
#include <fstream>

ModelDrawInfo::ModelDrawInfo()
{
	this->VAO_ID = 0;

	this->VERTEX_BUFFER_ID = 0;
	this->VERTEX_BUFFER_START_INDEX = 0;

	this->INDEX_BUFFER_ID = 0;
	this->INDEX_BUFFER_START_INDEX = 0;

	this->NUM_OF_VERTICES = 0;
	this->NUM_OF_INDICES = 0;
	this->NUM_OF_TRIANGLES = 0;

	this->mVertices = 0;
	this->mIndices = 0;

	// You could store the max and min values of the 
	//  vertices here (determined when you load them):
	glm::vec3 maxValues;
	glm::vec3 minValues;

	float maxExtent;

	return;
}

void VAOManager::setBasePath(std::string basePathWithoutSlash) {
    this->pBasePath = basePathWithoutSlash;
    return;
}

bool VAOManager::updateVAOBuffers(
    std::string fileName,
    ModelDrawInfo& updatedDrawInfo,
    unsigned int shaderProgramID
) {
    // This exists? 
    ModelDrawInfo updatedDrawInfo_TEMP;
    if (!this->findDrawInfoByModelName(fileName, updatedDrawInfo_TEMP))
    {
        // Didn't find this buffer
        return false;
    }


    glBindBuffer(GL_ARRAY_BUFFER, updatedDrawInfo.VERTEX_BUFFER_ID);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
        sizeof(VertexInfo) * updatedDrawInfo.NUM_OF_VERTICES,
        (GLvoid*)updatedDrawInfo.mVertices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, updatedDrawInfo.INDEX_BUFFER_ID);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
        0,
        sizeof(unsigned int) * updatedDrawInfo.NUM_OF_INDICES,
        (GLvoid*) updatedDrawInfo.mIndices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

bool VAOManager::loadModelIntoVAO(
	std::string fileName,
	ModelDrawInfo& mDrawInfo,
	unsigned int shaderProgramID
) {

	mDrawInfo.meshName = fileName;
	if (!this->loadPlyFile_XYZ_N_RGBA(fileName, mDrawInfo)) {
		return false;
	};

	// Create a VAO (Vertex Array Object), which will 
	// keep track of all the 'state' needed to draw 
	// from this buffer...
	glGenVertexArrays(1, &(mDrawInfo.VAO_ID)); // Ask OpenGL for a new buffer ID
	glBindVertexArray(mDrawInfo.VAO_ID);       // Bind and make this current VAO Buffer
	glGenBuffers(1, &(mDrawInfo.VERTEX_BUFFER_ID));
	glBindBuffer(GL_ARRAY_BUFFER, mDrawInfo.VERTEX_BUFFER_ID);

	// sVert vertices[3]
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(VertexInfo) * mDrawInfo.NUM_OF_VERTICES,
		(GLvoid*)mDrawInfo.mVertices,
		GL_STATIC_DRAW);

	// Copy the index buffer into the video card, too
	// Create an index buffer.
	glGenBuffers(1, &(mDrawInfo.INDEX_BUFFER_ID));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mDrawInfo.INDEX_BUFFER_ID);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(unsigned int) * mDrawInfo.NUM_OF_INDICES,
		(GLvoid*) mDrawInfo.mIndices,
		GL_STATIC_DRAW);

	// Set the vertex attributes.
	GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPos");	// get from shader program
	GLint vcol_location = glGetAttribLocation(shaderProgramID, "vCol");	// get from shader program
    GLint vNormal_location = glGetAttribLocation(shaderProgramID, "vNormal");

    // Set the vertex attributes for this shader
    glEnableVertexAttribArray(vpos_location);	    // vPos
    glVertexAttribPointer(vpos_location, 4,		// vPos
        GL_FLOAT, GL_FALSE,
        sizeof(VertexInfo),
        (void*) offsetof(VertexInfo, x));

    glEnableVertexAttribArray(vcol_location);	    // vCol
    glVertexAttribPointer(vcol_location, 4,		// vCol
        GL_FLOAT, GL_FALSE,
        sizeof(VertexInfo),
        (void*)offsetof(VertexInfo, r));

    glEnableVertexAttribArray(vNormal_location);	// vNormal
    glVertexAttribPointer(vNormal_location, 4,		// vNormal
        GL_FLOAT, GL_FALSE,
        sizeof(VertexInfo),
        (void*)offsetof(VertexInfo, nx));

	glBindVertexArray(0); // Set the VAO to zero

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(vpos_location);
	glDisableVertexAttribArray(vcol_location);

	// Store the draw information into the map
	this->mModelInfoMapByName[mDrawInfo.meshName] = mDrawInfo;

	return true;
}

bool VAOManager::findDrawInfoByModelName(
	std::string filename,
	ModelDrawInfo& drawInfo
) {
	std::map<std::string, ModelDrawInfo>::iterator itDrawInfo = this->mModelInfoMapByName.find(filename);

	// Search for the ModelDrawInfo
	if (itDrawInfo == this->mModelInfoMapByName.end()) {
		return false;
	}

	// Found the ModelDrawInfo
	drawInfo = itDrawInfo->second;
	return true;
}

bool VAOManager::loadPlyFile_XYZ_N_RGBA(
	std::string fileName, 
	ModelDrawInfo& mDrawInfo
) {

    std::ifstream modelFile(pBasePath + "/" + fileName.c_str());
	if (!modelFile.is_open()) {
        return false;
    }

    std::string temp;
    while (modelFile >> temp) {
        if (temp == "vertex") {
            break;
        }
    };

    modelFile >> mDrawInfo.NUM_OF_VERTICES;

    while (modelFile >> temp) {
        if (temp == "face") {
            break;
        }
    };

    modelFile >> mDrawInfo.NUM_OF_TRIANGLES;
    mDrawInfo.NUM_OF_INDICES = mDrawInfo.NUM_OF_TRIANGLES * 3;

    while (modelFile >> temp) {
        if (temp == "end_header") {
            break;
        }
    };

    // Dynamically allocate memory on the heap;
    mDrawInfo.mVertices = new VertexInfo[mDrawInfo.NUM_OF_VERTICES];

    // -0.036872 0.127727 0.00440925
    for (unsigned int index = 0; index != mDrawInfo.NUM_OF_VERTICES; index++)
    {
        VertexInfo tempVertex;
        modelFile >> tempVertex.x;
        modelFile >> tempVertex.y;
        modelFile >> tempVertex.z;

        modelFile >> tempVertex.nx;
        modelFile >> tempVertex.ny;
        modelFile >> tempVertex.nz;

        modelFile >> tempVertex.r;
        tempVertex.r /= 255.0f;

        modelFile >> tempVertex.g;
        tempVertex.g /= 255.0f;

        modelFile >> tempVertex.b;
        tempVertex.b /= 255.0f;

        modelFile >> tempVertex.a;
        tempVertex.a /= 255.0f;

        mDrawInfo.mVertices[index] = tempVertex;
    }

    TrianglePlyFile* pTheTriangles = new TrianglePlyFile[mDrawInfo.NUM_OF_TRIANGLES];

    for (unsigned int index = 0; index != mDrawInfo.NUM_OF_TRIANGLES; index++)
    {
        TrianglePlyFile tempTriangle;

        unsigned int discard;
        modelFile >> discard;            // Discard the first one
        modelFile >> tempTriangle.v0;
        modelFile >> tempTriangle.v1;
        modelFile >> tempTriangle.v2;

        pTheTriangles[index] = tempTriangle;
    }

    // Allocate an array for all the indices (which is 3x the number of triangles)
    // Element array is an 1D array of integers
    mDrawInfo.mIndices = new unsigned int[mDrawInfo.NUM_OF_INDICES];

    unsigned int elementIndex = 0;
    for (unsigned int triIndex = 0; triIndex != mDrawInfo.NUM_OF_TRIANGLES; triIndex++)
    {
        // 3 1582 1581 2063 
		mDrawInfo.mIndices[elementIndex + 0] = pTheTriangles[triIndex].v0;
		mDrawInfo.mIndices[elementIndex + 1] = pTheTriangles[triIndex].v1;
		mDrawInfo.mIndices[elementIndex + 2] = pTheTriangles[triIndex].v2;

        elementIndex += 3;      // Next "triangle"
    }

    return true;
}