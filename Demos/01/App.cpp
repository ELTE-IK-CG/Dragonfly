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

	//testTex2 = "wood.jpg";
	testTex2.InitTexture(500, 275);
	testTex2.MakeView<>() = "Assets/wood.jpg";

	testCubemap2.InitSizeFromFile("Assets/xpos.png");
	testCubemap2[TextureType::TEX_CUBE_X_POS] = "Assets/xpos.png";
	testCubemap2[TextureType::TEX_CUBE_X_NEG] = "Assets/xneg.png";
	testCubemap2[TextureType::TEX_CUBE_Y_POS] = "Assets/ypos.png";
	testCubemap2[TextureType::TEX_CUBE_Y_NEG] = "Assets/yneg.png";
	testCubemap2[TextureType::TEX_CUBE_Z_POS] = "Assets/zpos.png";
	testCubemap2[TextureType::TEX_CUBE_Z_NEG] = "Assets/zneg.png";

	testAlias = testCubemap.MakeView<TextureType::TEX_CUBE_Y_POS>();

	testAlias = testCubemap2[TextureType::TEX_CUBE_X_POS];

	testAlias = testCubemap2[4_levelAll][TextureType::TEX_CUBE_X_POS];

	testAlias = testCubemap2[TextureType::TEX_CUBE_X_POS][4_levelAll];

	const int WW = 64, HH = 64;
	testTex3.InitTexture(WW, HH);

	std::vector<glm::u8vec3> pixels(WW * HH);
	for (int i = 0; i < HH; ++i) for (int j = 0; j < WW; ++j) {
		pixels[WW*i + j] = glm::u8vec3(4 * j, 4 * i, 0);
	}
	testTex3 = pixels;

	std::vector<glm::vec3> pixels2(WW * HH);
	for (int i = 0; i < HH; ++i) for (int j = 0; j < WW; ++j) {
		pixels2[WW*i + j] = glm::vec3(j/float(WW), 0, i / float(HH));
	}
	testTex3 = pixels2;

	testAlias = testTex3.MakeView();
	
	const int W3 = 10, H3 = 10, D3 = 10;
	testField.InitTexture(W3, H3, D3);
	std::vector<float> fieldValues(W3 * H3 * D3);
	for (int i = 0; i < D3; ++i)for (int j = 0; j < H3; ++j)for (int k = 0; k < W3; ++k) {
		glm::vec3 v = (glm::vec3(k, j, i) - (glm::vec3(W3, H3, D3) - 1.0f) / 2.0f) / (glm::vec3(W3, H3, D3) / 2.0f);
		fieldValues[W3 * H3 * i + W3 * j + k] = glm::length(v) < 1.0f ? 0.0f : 1.0f;
	}
	testField = fieldValues;

	testField[1_levelAll];

	auto x = 0_levelAll & 0_layerAll;
	auto y = 0_level >> ALL & 0_layer >> ALL;
	auto z = 0_level >> 2 & 0_layer >> 3;

	GL_CHECK;

	/* TODO: GOAL to make this work:
	Texture<glm::u8vec3, TextureType::TEX_CUBE_MAP> tex;

	Texture2D<glm::u8vec3> uj = tex.MakeView<GL_TEXTURE_CUBE_MAP_NEGATIVE_X, [default = same]>();
	uj = "negx.png";
	
	tex.GeneralMakeView<glm::u8vec3, TextureType::TEX_2D>(minLevels, numlevels, minLevels, numLayers);
	*/
	
	//[...]

	//myFBO[0] = tex.MakeView<glm::u8vec3, GL_TEXTURE_CUBE_MAP_NEGATIVE_X>();


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
	program << "testTexture" << testAlias << "testField" << testField;
	program << SetSubroutines;
	program2 << SetSubroutines;
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
	if (key.keysym.sym == SDLK_1) program2 << "calcColor" << "vertColor";
	if (key.keysym.sym == SDLK_2) program2 << "calcColor" << "textureColor";
}

void App::KeyboardUp(SDL_KeyboardEvent& key) {
	cam.KeyboardUp(key);
	cam2.KeyboardUp(key);
}

void App::MouseMove(SDL_MouseMotionEvent& mouse) { cam.MouseMove(mouse); cam2.MouseMove(mouse);}
void App::MouseDown(SDL_MouseButtonEvent&) {}
void App::MouseUp(SDL_MouseButtonEvent&) {}
void App::MouseWheel(SDL_MouseWheelEvent&wheel) { cam.MouseWheel(wheel); cam2.MouseWheel(wheel);}