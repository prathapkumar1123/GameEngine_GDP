#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

class LightHelper {

public:

	static const float DEFAULT_ATTEN_CONST;					// = 0.1f;
	static const float DEFAULT_ATTEN_LINEAR;				// = 0.1f;
	static const float DEFAULT_ATTEN_QUADRATIC;				// = 0.1f;
	static const float DEFAULT_AMBIENT_TO_DIFFUSE_RATIO;	// = 0.2f;

	static const unsigned int DEFAULTMAXITERATIONS = 50;

	static const float DEFAULTZEROTHRESHOLD;	// = 0.0001f;
	static const float DEFAULTINFINITEDISTANCE;	// = 10,000.0f;	
	static const float DEFAULDIFFUSEACCURACYTHRESHOLD; // = 0.001f;
	
	float calcApproxDistFromAtten(float targetLightLevel);	// Uses the defaults
	
	float calcApproxDistFromAtten(float targetLightLevel, float accuracy);	// Uses the defaults

	float calcApproxDistFromAtten(float targetLightLevel, float accuracy, float infiniteDistance,
		float constAttenuation, float linearAttenuation, float quadraticAttenuation,
		unsigned int maxIterations = DEFAULTMAXITERATIONS /*= 50*/);
	
	
	
	float calcDiffuseFromAttenByDistance(float distance,
		float constAttenuation,
		float linearAttenuation,
		float quadraticAttenuation,
		float zeroThreshold = DEFAULTZEROTHRESHOLD);
};
