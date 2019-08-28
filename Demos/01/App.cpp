#include "app.h"

#include <iostream>

// glm
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform2.hpp>

// offsetof
#include <cstddef>

bool App::Init(int width, int height) {

	InitShaders();
	InitGL();
	Resize(width, height);

	GL_CHECK;
	return true;
}

void App::InitGL() {
	//glEnable              ( GL_DEBUG_OUTPUT );
	//glDebugMessageCallback( MessageCallback, 0 );
	
	glm::vec2 Vert[] = {
		glm::vec2(-1, -1),
		glm::vec2(1, -1),
		glm::vec2(0, 1)
	};

	VaoID.bindVertexArray();
	VboID.bindBuffer();
	VboID.assignMutable(&Vert);
	VaoID.addVBO(VboID);


	// Set clear color to blue
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	// Enable backface culling and depth test
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	GL_CHECK;
}

void App::Resize(int width, int height) {
	canvas_width = width;
	canvas_height = height;
	cam.Resize(width, height);

	glViewport(0, 0, canvas_width, canvas_height);
	GL_CHECK;
}

void App::InitShaders() {
	program << "Shaders/types.glsl"_fs <<  "Shaders/sdf.glsl"_fs << "Shaders/main.frag.glsl"_fs;
	program	<< "Shaders/quad.vert.glsl"_vs << LinkProgram;

	std::cout << program.GetErrors();
	GL_CHECK;
}

void App::Update() {
	cam.Update();
}

void App::Render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	program << "gCameraPos" << cam.GetEye(); //<< "gInverseViewProj" << cam.GetInverseViewProj() << "gTanPixelAngle" << cam.GetTanPixelFow();
	//program << "gCameraDir" << cam.GetDir() << "gDepthcalcCoeffs" << cam.GetDepthcalcCoeffs() << "gNearFarClips" << cam.GetNearFarClips();
	program << "col_intensity" << col_intensity;

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
	GL_CHECK;

	cam.RenderUI();
	program.Render();
}


void App::KeyboardDown(SDL_KeyboardEvent& key) {
	cam.KeyboardDown(key);
}

void App::KeyboardUp(SDL_KeyboardEvent& key) {
	cam.KeyboardUp(key);
}

void App::MouseMove(SDL_MouseMotionEvent& mouse) { cam.MouseMove(mouse); }
void App::MouseDown(SDL_MouseButtonEvent&) {}
void App::MouseUp(SDL_MouseButtonEvent&) {}
void App::MouseWheel(SDL_MouseWheelEvent&wheel) { cam.MouseWheel(wheel); }