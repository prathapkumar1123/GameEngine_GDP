#include "VAOManager.h"

#include "../../Core/OpenGlCommons.h"
#include "../../Core/Globals.h"

#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Assimp::Importer assImpImporter;

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

    this->boundingBox = BoundingBox3D(glm::vec3(0.0f), glm::vec3(0.f));

    return;
}

VAOManager::VAOManager() {}

VAOManager::~VAOManager() {
    
}

void VAOManager::setShaderProgramId(unsigned int shaderProgramId) {
    this->shaderProgramId = shaderProgramId;
}

void VAOManager::setBasePath(std::string basePathWithoutSlash) {
    this->pBasePath = basePathWithoutSlash;
    return;
}

std::string VAOManager::getBasePath() {
    return pBasePath;
}

bool VAOManager::updateVAOBuffers(std::string fileName, ModelDrawInfo& updatedDrawInfo) {
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
        (GLvoid*)updatedDrawInfo.mIndices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

bool VAOManager::loadModelIntoVAO(std::string fileName, ModelDrawInfo& mDrawInfo) {

    mDrawInfo.meshName = fileName;
    if (!this->loadTheFile_Ply_XYZ_N_RGBA_UV(fileName, mDrawInfo)) {
        return false;
    };

    // Create a VAO (Vertex Array Object), which will 
    // keep track of all the 'state' needed to draw from this buffer...
    glGenVertexArrays(1, &(mDrawInfo.VAO_ID));
    glBindVertexArray(mDrawInfo.VAO_ID);
    
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
        (GLvoid*)mDrawInfo.mIndices,
        GL_STATIC_DRAW);

    // Set the vertex attributes.
    GLint vpos_location = glGetAttribLocation(shaderProgramId, "vPos");	// get from shader program
    GLint vcol_location = glGetAttribLocation(shaderProgramId, "vCol");	// get from shader program
    GLint vNormal_location = glGetAttribLocation(shaderProgramId, "vNormal");
    
    GLint vTextureCoords_location = glGetAttribLocation(shaderProgramId, "vTextureCoords");

    // Set the vertex attributes for this shader
    glEnableVertexAttribArray(vpos_location);	// vPos
    glVertexAttribPointer(vpos_location, 4,		// vPos
        GL_FLOAT, GL_FALSE,
        sizeof(VertexInfo),
        (void*)offsetof(VertexInfo, x));

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

    glEnableVertexAttribArray(vTextureCoords_location);	    // vTextureCoords;
    glVertexAttribPointer(vTextureCoords_location, 2,		// in vec2 vTextureCoords;
        GL_FLOAT, GL_FALSE,
        sizeof(VertexInfo),
        (void*)offsetof(VertexInfo, u));

    glBindVertexArray(0); // Set the VAO to zero

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(vpos_location);
    glDisableVertexAttribArray(vcol_location);
    glDisableVertexAttribArray(vNormal_location);
    glDisableVertexAttribArray(vTextureCoords_location);

    // Store the draw information into the map
    this->pModelInfoMapByName[mDrawInfo.meshName] = mDrawInfo;

    return true;
}

bool VAOManager::findDrawInfoByModelName(
    std::string filename,
    ModelDrawInfo& drawInfo
) {
    try {
        std::map<std::string, ModelDrawInfo>::iterator itDrawInfo = this->pModelInfoMapByName.find(filename);

        // Search for the ModelDrawInfo
        if (itDrawInfo == this->pModelInfoMapByName.end()) {
            return false;
        }

        // Found the ModelDrawInfo
        drawInfo = itDrawInfo->second;
        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}

bool VAOManager::loadPlyFile_XYZ_N_RGBA(
    std::string fileName,
    ModelDrawInfo& mDrawInfo
) {
    wchar_t buffer[MAX_PATH];
    std::wstring path;
    if (_wgetcwd(buffer, MAX_PATH) != nullptr) {
        path = buffer + std::wstring(pBasePath.begin(), pBasePath.end());
    }

    std::ifstream modelFile(pBasePath + "/" + fileName.c_str());

    if (!modelFile.is_open()) {
        std::cout << modelFile.tellg() << std::endl;
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

    glm::vec3 minPoint(0, 0, 0);
    glm::vec3 maxPoint(0, 0, 0);

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

        float discard1;
        float discard2;

        modelFile >> discard1;
        modelFile >> discard2;
            
        minPoint.x = minPoint.x < tempVertex.x ? minPoint.x : tempVertex.x;
        minPoint.y = minPoint.y < tempVertex.y ? minPoint.y : tempVertex.y;
        minPoint.z = minPoint.z < tempVertex.z ? minPoint.z : tempVertex.z;

        maxPoint.x = maxPoint.x > tempVertex.x ? maxPoint.x : tempVertex.x;
        maxPoint.y = maxPoint.y > tempVertex.y ? maxPoint.y : tempVertex.y;
        maxPoint.z = maxPoint.z > tempVertex.z ? maxPoint.z : tempVertex.z;

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

bool VAOManager::loadTheFile_Ply_XYZ_N_RGBA_UV(std::string fileName, ModelDrawInfo& drawInfo) {

    wchar_t buffer[MAX_PATH];
    std::wstring path;
    if (_wgetcwd(buffer, MAX_PATH) != nullptr) {
        path = buffer + std::wstring(pBasePath.begin(), pBasePath.end());
    }

    std::ifstream modelFile(pBasePath + "/" + fileName.c_str());

    if (!modelFile.is_open()) {
        std::cout << modelFile.tellg() << std::endl;
        return false;
    }

    std::string temp;
    while (modelFile >> temp) {
        if (temp == "vertex") break;
    };

    modelFile >> drawInfo.NUM_OF_VERTICES;

    while (modelFile >> temp) {
        if (temp == "face") break;
    };

    modelFile >> drawInfo.NUM_OF_TRIANGLES;
    drawInfo.NUM_OF_INDICES = drawInfo.NUM_OF_TRIANGLES * 3;


    while (modelFile >> temp) {
        if (temp == "end_header") break;
    };

    VertexInfo* pTheVerticesFile = new VertexInfo[drawInfo.NUM_OF_VERTICES];

    for (unsigned int index = 0; index != drawInfo.NUM_OF_VERTICES; index++)
    {
        VertexInfo tempVertex;
        modelFile >> tempVertex.x;                //std::cin >> a.x;
        modelFile >> tempVertex.y;                //std::cin >> a.y;
        modelFile >> tempVertex.z;                //std::cin >> a.z;

        modelFile >> tempVertex.nx;
        modelFile >> tempVertex.ny;
        modelFile >> tempVertex.nz;              //std::cin >> a.z;

        modelFile >> tempVertex.r;       tempVertex.r /= 255.0f;
        modelFile >> tempVertex.g;       tempVertex.g /= 255.0f;
        modelFile >> tempVertex.b;       tempVertex.b /= 255.0f;
        modelFile >> tempVertex.a;       tempVertex.a /= 255.0f;

        // ALSO load the UV coordinates
        modelFile >> tempVertex.u;
        modelFile >> tempVertex.v;

        pTheVerticesFile[index] = tempVertex;
    }


    TrianglePlyFile* pTheTriangles = new TrianglePlyFile[drawInfo.NUM_OF_TRIANGLES];

    for (unsigned int index = 0; index != drawInfo.NUM_OF_TRIANGLES; index++) {
        TrianglePlyFile tempTriangle;

        unsigned int discard;
        modelFile >> discard;
        modelFile >> tempTriangle.v0;
        modelFile >> tempTriangle.v1;
        modelFile >> tempTriangle.v2;

        pTheTriangles[index] = tempTriangle;
    }

    // ... now we just copy the vertices from the file as is (unchanged)
    drawInfo.mVertices = new VertexInfo[drawInfo.NUM_OF_VERTICES];
    for (unsigned int vertIndex = 0; vertIndex != drawInfo.NUM_OF_VERTICES; vertIndex++)
    {
        drawInfo.mVertices[vertIndex].x = pTheVerticesFile[vertIndex].x;
        drawInfo.mVertices[vertIndex].y = pTheVerticesFile[vertIndex].y;
        drawInfo.mVertices[vertIndex].z = pTheVerticesFile[vertIndex].z;
        drawInfo.mVertices[vertIndex].w = 1.0f;

        drawInfo.mVertices[vertIndex].nx = pTheVerticesFile[vertIndex].nx;
        drawInfo.mVertices[vertIndex].ny = pTheVerticesFile[vertIndex].ny;
        drawInfo.mVertices[vertIndex].nz = pTheVerticesFile[vertIndex].nz;
        drawInfo.mVertices[vertIndex].nw = 1.0f;

        drawInfo.mVertices[vertIndex].r = pTheVerticesFile[vertIndex].r;
        drawInfo.mVertices[vertIndex].g = pTheVerticesFile[vertIndex].g;
        drawInfo.mVertices[vertIndex].b = pTheVerticesFile[vertIndex].b;
        drawInfo.mVertices[vertIndex].a = pTheVerticesFile[vertIndex].a;

        // Copy the UV coords to the array that will eventually be copied to the GPU
        drawInfo.mVertices[vertIndex].u = pTheVerticesFile[vertIndex].u;
        drawInfo.mVertices[vertIndex].v = pTheVerticesFile[vertIndex].v;
    }

    drawInfo.mIndices = new unsigned int[drawInfo.NUM_OF_INDICES];

    unsigned int elementIndex = 0;
    for (unsigned int triIndex = 0; triIndex != drawInfo.NUM_OF_TRIANGLES; triIndex++) {
        drawInfo.mIndices[elementIndex + 0] = pTheTriangles[triIndex].v0;
        drawInfo.mIndices[elementIndex + 1] = pTheTriangles[triIndex].v1;
        drawInfo.mIndices[elementIndex + 2] = pTheTriangles[triIndex].v2;

        elementIndex += 3;
    }

    return true;
}


bool VAOManager::loadModel(const std::string filepath, ModelDrawInfo& drawInfo) {

    drawInfo.meshName = filepath;

    std::string modelPath = "assets/models/" + filepath;
    try {
        const aiScene* scene = assImpImporter.ReadFile(modelPath, aiProcess_GenNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "Could not load model at " << modelPath << std::endl << assImpImporter.GetErrorString() << std::endl;
            return false;
        }

        aiMesh* mesh = scene->mMeshes[0];

        unsigned int numVerticesInVertArray = mesh->mNumVertices;
        unsigned int numIndicesInIndexArray = mesh->mNumFaces * 3;

        drawInfo.NUM_OF_TRIANGLES = mesh->mNumFaces;
        drawInfo.NUM_OF_VERTICES = mesh->mNumVertices;
        drawInfo.NUM_OF_INDICES = mesh->mNumFaces * 3;

        VertexInfo* pTempVertArray = new VertexInfo[drawInfo.NUM_OF_VERTICES];
        GLuint* pIndexArrayLocal = new GLuint[numIndicesInIndexArray];


        unsigned int vertArrayIndex = 0;
        drawInfo.mIndices = new unsigned int[drawInfo.NUM_OF_INDICES];

        for (unsigned int faceIdx = 0; faceIdx != mesh->mNumFaces; faceIdx++)
        {
            aiFace face = mesh->mFaces[faceIdx];

            for (int idx = 0; idx != 3; idx++) {

                unsigned int index = face.mIndices[idx];

                aiVector3D position = mesh->mVertices[index];
                pTempVertArray[vertArrayIndex].x = position.x;
                pTempVertArray[vertArrayIndex].y = position.y;
                pTempVertArray[vertArrayIndex].z = position.z;
                pTempVertArray[vertArrayIndex].w = 1.0f;

                if (mesh->HasVertexColors(index)) {
                    aiColor4D* color = mesh->mColors[index];
                    pTempVertArray[vertArrayIndex].r = color->r;
                    pTempVertArray[vertArrayIndex].g = color->g;
                    pTempVertArray[vertArrayIndex].b = color->b;
                    pTempVertArray[vertArrayIndex].a = color->a;

                    delete color;
                }
                else
                {
                    pTempVertArray[vertArrayIndex].r = 1.0f; // Default to white if no color information
                    pTempVertArray[vertArrayIndex].g = 1.0f;
                    pTempVertArray[vertArrayIndex].b = 1.0f;
                    pTempVertArray[vertArrayIndex].a = 1.0f;
                }

                if (mesh->HasTextureCoords(0)) {
                    aiVector3D textureCoord = mesh->mTextureCoords[0][index];
                    pTempVertArray[vertArrayIndex].u = textureCoord.x;
                    pTempVertArray[vertArrayIndex].v = textureCoord.y;
                }
                else
                {
                    pTempVertArray[vertArrayIndex].u = 0.0f;
                    pTempVertArray[vertArrayIndex].v = 0.0f;
                }

                aiVector3D normal = mesh->mNormals[index];
                pTempVertArray[vertArrayIndex].nx = normal.x;
                pTempVertArray[vertArrayIndex].ny = normal.y;
                pTempVertArray[vertArrayIndex].nz = normal.z;
                pTempVertArray[vertArrayIndex].nw = 0.0f;

                pIndexArrayLocal[vertArrayIndex] = vertArrayIndex;

                vertArrayIndex++;
            }
        }

        drawInfo.boundingBox = BoundingBox3D(
            glm::vec3(mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z),
            glm::vec3(mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z)
        );

        drawInfo.mIndices = pIndexArrayLocal;
        drawInfo.mVertices = pTempVertArray;

        delete[] pTempVertArray;
        delete[] pIndexArrayLocal;

    }
    catch (const std::exception& e) {
        std::cerr << "Exception during Assimp loading: " << e.what() << std::endl;
        return false;
    }

    return true;
}