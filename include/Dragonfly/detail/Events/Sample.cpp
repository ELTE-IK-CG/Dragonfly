#include <string>
#include <GL/glew.h>
#include "Sample.h"
#include "../../detail/Framebuffer/FramebufferBase.h"
#include "renderdoc_load_api.h"

df::Sample::Sample(const char* name_, int width_, int height_, FLAGS flags_)
{
	if(flags_ && FLAGS::INIT_RENDERDOC)
		rdoc::initRenderDocAPI(false);

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
	Uint32 sdl_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;
	sdl_flags |= flags_ && FLAGS::WINDOW_BORDERLESS	? SDL_WINDOW_BORDERLESS	: 0;
	sdl_flags |= flags_ && FLAGS::WINDOW_FULLSCREEN	? SDL_WINDOW_FULLSCREEN	: 0;
	sdl_flags |= flags_ && FLAGS::WINDOW_RESIZABLE	? SDL_WINDOW_RESIZABLE	: 0;
	_mainWindowPtr = SDL_CreateWindow(name_, DM.w / 2 - width_ / 2, DM.h / 2 - height_ / 2, width_, height_, sdl_flags);
	ASSERT(_mainWindowPtr != nullptr, (std::string("Unable to create SDL window: ") + SDL_GetError()).c_str());
	_mainWindowID = SDL_GetWindowID(_mainWindowPtr);

	_mainWindowContext = SDL_GL_CreateContext(_mainWindowPtr);
	ASSERT(_mainWindowContext != nullptr, (std::string("Unable to create OpenGL context: ") + SDL_GetError()).c_str());
	if(SDL_GL_SetSwapInterval(flags_ && FLAGS::V_SYNC_ADAPTIVE ? -1 : flags_ && FLAGS::V_SYNC ? 1 : 0) == -1 && (flags_ && FLAGS::V_SYNC_ADAPTIVE))
		SDL_GL_SetSwapInterval(1);
	GLenum error = glewInit();
	ASSERT(error == GLEW_OK, "Unable to initialize GLEW");
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

	ImGui_ImplSDL2_InitForOpenGL(_mainWindowPtr, _mainWindowContext);
	ImGui_ImplOpenGL3_Init(glsl_version.c_str());

	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	df::Backbuffer = df::DefaultFramebuffer(width_, height_);
	this->AddHandlerClass(df::Backbuffer);

	if (flags_ && FLAGS::RUN_RENDERDOC_ON_CAPTURE)
		this->AddKeyDown([](const SDL_KeyboardEvent &key)->bool
			{
				if (key.keysym.sym == SDLK_PRINTSCREEN && rdoc::API != nullptr && !rdoc::API->IsTargetControlConnected())
					rdoc::API->LaunchReplayUI(1, nullptr);
				return false;
			}, -10);
}

df::Sample::~Sample()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	if(_mainWindowContext)	SDL_GL_DeleteContext(_mainWindowContext);
	if(_mainWindowPtr)		SDL_DestroyWindow(_mainWindowPtr);
	SDL_Quit();
	//glew cannot quit
}
