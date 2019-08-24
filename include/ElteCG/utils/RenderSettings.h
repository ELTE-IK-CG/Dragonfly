#pragma once
#include <glm/glm.hpp>
//#include "glw/ShaderProgram.h"

class RenderSettings
{
public:
	void SetUniforms(GLWRAPPER_NS::ShaderProgram& program);
	bool RenderUI();

	bool uiIsOpen = false;

	// settings
	glm::vec3 gLightColor = glm::vec3(1.0f,1.0f,1.0f);
	glm::vec3 gLightPos = glm::vec3(2.0f,7.0f,10.0f);
	float gLightSize = 0.5f;
	float gSphereLightStrength = 4.0f;

	int gAOMaxIters = 5;
	float gAOStepSize = 0.4f;
	float gAOStrength = 0.7f;

	glm::vec3 gAmbient = glm::vec3(0.01f, 0.01f, 0.011f);
	glm::vec3 gDiffuse = glm::vec3(0.68f, 0.78f, 0.92f);
	glm::vec3 gCookRoughness = glm::vec3(0.03f, 0.034f, 0.032f);
	glm::vec3 gCookIOR = glm::vec3(1.4f, 1.41f, 1.42f);

	int gShadowMaxiter = 20;
	float gShadowEpsilon = 0.05f;
};