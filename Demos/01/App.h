#pragma once

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>


//Dragonfly Runtime
#include "ElteCG/Utils/Camera.h"
#include "ElteCG/utils/ShaderEditor.h"
#include "ElteCG/utils/Shader.h"
#include "ElteCG/utils/Program.h"
#include "ElteCG/utils/UniformEditor.h"
#include "ElteCG/utils/ProgramEditor.h"
#include "ElteCG/ogl/buffer.h"
#include "ElteCG/ogl/vao.h"
#include <ElteCG/utils/Texture.h>

class App {

public:

	App() = default;

	bool Init(int width, int height);
	void Resize(int width, int height);

	void Update();
	void Render();

	void KeyboardDown(SDL_KeyboardEvent&);
	void KeyboardUp(SDL_KeyboardEvent&);
	void MouseMove(SDL_MouseMotionEvent&);
	void MouseDown(SDL_MouseButtonEvent&);
	void MouseUp(SDL_MouseButtonEvent&);
	void MouseWheel(SDL_MouseWheelEvent&);

private:

	int canvas_width=720;
	int canvas_height=480;
	float col_intensity = 0.15f;
	
	ShaderProgramEditorVF program = "Side-Prog";
	ShaderProgramEditorVF program2 = "Main-Prog";
	//ShaderProgramVF program = "Side-Prog";
	//ShaderProgramVF program2 = "Main-Prog";

	eltecg::ogl::ArrayBuffer MyVBO;
	eltecg::ogl::VertexArray MyVAO;

	Camera cam;
	Camera cam2 = Camera("CameraWithName");

	void InitShaders();

	void InitGL();

	Texture2D<> testTex = Texture2D<>("Assets/scales.jpg");
	Texture2D<> testTex2;
	Texture2D<> testTex3;

	TextureCube<> testCubemap = TextureCube<>("Assets/xpos.png", "Assets/xneg.png", "Assets/ypos.png", "Assets/yneg.png", "Assets/zpos.png", "Assets/zneg.png");
	TextureCube<> testCubemap2;

	Texture2D<> testAlias;

	Texture3D<float> testField;
};