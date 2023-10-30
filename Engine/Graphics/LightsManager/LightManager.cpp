#include "LightManager.h"

#include <sstream>

cLight::cLight()
{
	this->position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);		// White light
	this->specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);		// White light

	// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	this->atten = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);


	// Spot, directional lights
	// (Default is stright down)
	this->direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);

	// x = lightType, y = inner angle, z = outer angle, w = TBD
	// type = 0 => point light
	this->param1 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);


	// 0 = pointlight
	// 1 = spot light
	// 2 = directional light
	// x = 0 for off, 1 for on
	this->param2 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);


	this->position_UL = -1;
	this->diffuse_UL = -1;
	this->specular_UL = -1;
	this->atten_UL = -1;
	this->direction_UL = -1;
	this->param1_UL = -1;
	this->param2_UL = -1;

}

// And the uniforms:
void cLight::TurnOn(void)
{
	// x = 0 for off, 1 for on
	this->param2.x = 1.0f;
	return;
}

void cLight::TurnOff(void)
{
	// x = 0 for off, 1 for on
	this->param2.x = 0.0f;
	return;
}


LightManager::LightManager() {}

void LightManager::SetUniformLocations(GLuint shaderID) {
	for (int i = 0; i < NUMBER_OF_LIGHTS_IM_USING; i++) {

		// vec4 position;
		std::string position_UL = "theLights[" + std::to_string(i) + "].position";

		this->theLights[i].position_UL = glGetUniformLocation(shaderID, position_UL.c_str());

		// vec4 diffuse;	// Colour of the light (used for diffuse)
		std::string diffuse_UL = "theLights[" + std::to_string(i) + "].diffuse";
		this->theLights[i].diffuse_UL = glGetUniformLocation(shaderID, diffuse_UL.c_str());

		// vec4 specular;	// rgb = highlight colour, w = power
		std::string specular_UL = "theLights[" + std::to_string(i) + "].specular";
		this->theLights[i].specular_UL = glGetUniformLocation(shaderID, specular_UL.c_str());

		// vec4 atten;
		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
		std::string atten_UL = "theLights[" + std::to_string(i) + "].atten";
		this->theLights[i].atten_UL = glGetUniformLocation(shaderID, atten_UL.c_str());

		// vec4 direction;	// Spot, directional lights
		std::string direction_UL = "theLights[" + std::to_string(i) + "].direction";
		this->theLights[i].direction_UL = glGetUniformLocation(shaderID, direction_UL.c_str());

		// vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
		std::string param1_UL = "theLights[" + std::to_string(i) + "].param1";
		this->theLights[i].param1_UL = glGetUniformLocation(shaderID, param1_UL.c_str());

		// vec4 param2;	// x = 0 for off, 1 for on
		std::string param2_UL = "theLights[" + std::to_string(i) + "].param2";
		this->theLights[i].param2_UL = glGetUniformLocation(shaderID, param2_UL.c_str());

	}
}

// This is called every frame
void LightManager::UpdateUniformValues(GLuint shaderID)
{
	for (unsigned int index = 0; index != NUMBER_OF_LIGHTS_IM_USING; index++)
	{
		glUniform4f(theLights[index].position_UL,
			theLights[index].position.x,
			theLights[index].position.y,
			theLights[index].position.z,
			theLights[index].position.w);

		glUniform4f(theLights[index].diffuse_UL,
			theLights[index].diffuse.x,
			theLights[index].diffuse.y,
			theLights[index].diffuse.z,
			theLights[index].diffuse.w);

		glUniform4f(theLights[index].specular_UL,
			theLights[index].specular.x,
			theLights[index].specular.y,
			theLights[index].specular.z,
			theLights[index].specular.w);

		glUniform4f(theLights[index].atten_UL,
			theLights[index].atten.x,
			theLights[index].atten.y,
			theLights[index].atten.z,
			theLights[index].atten.w);

		glUniform4f(theLights[index].direction_UL,
			theLights[index].direction.x,
			theLights[index].direction.y,
			theLights[index].direction.z,
			theLights[index].direction.w);

		glUniform4f(theLights[index].param1_UL,
			theLights[index].param1.x,
			theLights[index].param1.y,
			theLights[index].param1.z,
			theLights[index].param1.w);

		glUniform4f(theLights[index].param2_UL,
			theLights[index].param2.x,
			theLights[index].param2.y,
			theLights[index].param2.z,
			theLights[index].param2.w);
	}

	return;
}
