#include "app.h"

#include <iostream>

// glm
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform2.hpp>

// offsetof
#include <cstddef>

bool App::Init(int width, int height) {

	MyVAO.addVBO<glm::vec2>(MyVBO);

	InitShaders();
	InitGL();
	Resize(width, height);

	GL_CHECK;

	

	return true;
}

void App::InitGL() {
	//glEnable              ( GL_DEBUG_OUTPUT );
	//glDebugMessageCallback( MessageCallback, 0 );
	
	std::vector<glm::vec2> toVBO( 3 * 2 );
	
	toVBO[0] = glm::vec2(-1, -1);
	toVBO[1] = glm::vec2(1, -1);
	toVBO[2] = glm::vec2(0, 1);

	MyVBO.constructMutable(toVBO, GL_STATIC_DRAW);
	

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
	cam2.Resize(width * 1.5f, height * 1.5f);

	glViewport(0, 0, canvas_width, canvas_height);
	GL_CHECK;
}

void App::InitShaders() {
	program << "Shaders/types.glsl"_fs <<  "Shaders/sdf.glsl"_fs << "Shaders/main.frag.glsl"_fs;
	program	<< "Shaders/quad.vert.glsl"_vs << LinkProgram;
	program2 << "Shaders/types.glsl"_fs << "Shaders/sdf.glsl"_fs << "Shaders/main.frag.glsl"_fs;
	program2 << "Shaders/quad.vert.glsl"_vs << LinkProgram;

	std::cout << program.GetErrors();
	std::cout << program2.GetErrors();
	GL_CHECK;
}

void App::Update() {
	cam.Update();
	cam2.Update();
}

void App::Render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	program << "col_intensity" << col_intensity << "gCameraPos" << cam.GetEye();
	program2 << "col_intensity" << col_intensity << "gCameraPos" << cam2.GetEye();
	program << "testTexture" << testTex;
	MyVAO.bindVertexArray();

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
	GL_CHECK;

	cam.RenderUI();
	cam2.RenderUI();
	program.Render();
	//In the UI 'prog2 - Main prog' will override controls
	program2.Render();
	
}


void App::KeyboardDown(SDL_KeyboardEvent& key) {
	cam.KeyboardDown(key);
	cam2.KeyboardDown(key);
}

void App::KeyboardUp(SDL_KeyboardEvent& key) {
	cam.KeyboardUp(key);
	cam2.KeyboardUp(key);
}

void App::MouseMove(SDL_MouseMotionEvent& mouse) { cam.MouseMove(mouse); cam2.MouseMove(mouse);}
void App::MouseDown(SDL_MouseButtonEvent&) {}
void App::MouseUp(SDL_MouseButtonEvent&) {}
void App::MouseWheel(SDL_MouseWheelEvent&wheel) { cam.MouseWheel(wheel); cam2.MouseWheel(wheel);}