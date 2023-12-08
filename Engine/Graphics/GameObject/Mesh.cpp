#include "Mesh.h"

Mesh::Mesh(std::string meshName) {
	this->meshName = meshName;

	textureRatios[0] = 1.0f;
	textureRatios[1] = 0.0f;
	textureRatios[2] = 0.0f;
	textureRatios[3] = 0.0f;
	textureRatios[4] = 0.0f;
	textureRatios[5] = 0.0f;
	textureRatios[6] = 0.0f;
	textureRatios[7] = 0.0f;
}

Mesh::~Mesh() {}
