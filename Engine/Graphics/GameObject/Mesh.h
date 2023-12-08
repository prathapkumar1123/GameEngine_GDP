#pragma once

#include <string>

class Mesh
{
public:
	Mesh(std::string meshName);
	~Mesh();

	static const int NUM_TEXTURES = 8;

	float textureRatios[NUM_TEXTURES];
	std::string textureName[NUM_TEXTURES];

private:
	std::string meshName;
};

