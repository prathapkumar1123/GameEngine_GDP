#pragma once
#include "../Graphics/GameObject/GameObject.h"
#include "../Graphics/VAOManager/VAOManager.h"

#include <glm/glm.hpp>

static void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}