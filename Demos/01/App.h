#pragma once

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>


//Dragonfly Runtime
#include <Dragonfly/editor.h>
#include <Dragonfly/detail/buffer.h>
#include <Dragonfly/detail/vao.h>

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

	ProgramEditor<UniformEditor, ShaderEditor<SFile>,	   ShaderEditor<SFile>			> program  = "Side-Prog";
	ProgramEditor<UniformEditor, ShaderEditor<FileEditor>, ShaderEditor<FileEditor>	> program2 = "Main-Prog";
		

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