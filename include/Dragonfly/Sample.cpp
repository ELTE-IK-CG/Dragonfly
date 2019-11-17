#include <string>
#include <GL/glew.h>
#include "Sample.h"

Sample::Sample(const char* name, int width, int height, int vsync)
{
	auto err = SDL_Init(SDL_INIT_EVERYTHING);
	ASSERT( err != -1, (std::string("Unable to initialize SDL: ") + SDL_GetError()).c_str());
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,	32);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,	8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,	8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,	8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,	8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,	24);

	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);
	win = SDL_CreateWindow(name, DM.w / 2 - width / 2, DM.h / 2 - height / 2, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	ASSERT(win != nullptr, (std::string("Unable to create SDL window: ") + SDL_GetError()).c_str());
	mainWindowID = SDL_GetWindowID(win);

	context = SDL_GL_CreateContext(win);
	ASSERT(context != nullptr, (std::string("Unable to create OpenGL context: ") + SDL_GetError()).c_str());
	SDL_GL_SetSwapInterval(vsync);

	GLenum error = glewInit();
	ASSERT(error != GLEW_OK, "Unable to initialize GLEW");
	int v_ma = -1, v_mi = -1;
	glGetIntegerv(GL_MAJOR_VERSION, &v_ma);
	glGetIntegerv(GL_MINOR_VERSION, &v_mi);
	ASSERT(3 <= v_ma && v_ma <= 4 && 0 <= v_mi && v_mi <= 9, ("Unsupported OpenGL Version: "+ std::to_string(v_ma) + '.' + std::to_string(v_mi)).c_str());
	
	//https://en.wikipedia.org/wiki/OpenGL_Shading_Language#Versions
	if (v_ma == 3 && v_mi == 2) { v_ma = 1; v_mi = 5; }
	if (v_ma == 3 && v_mi == 1) { v_ma = 1; v_mi = 4; }
	if (v_ma == 3 && v_mi == 0) { v_ma = 1; v_mi = 3; }
	if (v_ma == 2 && v_mi == 1) { v_ma = 1; v_mi = 2; }
	if (v_ma == 2 && v_mi == 0) { v_ma = 1; v_mi = 1; }
	std::string glsl_version = "#version " + std::to_string(v_ma) + std::to_string(v_mi) + '0';

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	ImGui_ImplSDL2_InitForOpenGL(win, context);
	ImGui_ImplOpenGL3_Init(glsl_version.c_str());

}

Sample::~Sample()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	if(context)	SDL_GL_DeleteContext(context);
	if(win)		SDL_DestroyWindow(win);
	SDL_Quit();
	//glew cannot quit
}
