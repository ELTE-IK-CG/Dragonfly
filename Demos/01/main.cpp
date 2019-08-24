#include <iostream>

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

//ImGui
#include <imgui/imgui.h>
#include <ImGui-addons/auto/auto.h>
#include <ImGui-addons/impl/imgui_impl_sdl.h>
#include <ImGui-addons/impl/imgui_impl_opengl3.h>


// App
#include "app.h"


void program_exit() {
	SDL_Quit();
}

int main(int argc, char* args[]) {

	atexit(program_exit);

	// Init SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return 1;
	}

	// Set GL params
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// antialiasing - off
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1);
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  2);

	// create window
	SDL_Window *win = nullptr;
	win = SDL_CreateWindow(
		"OpenGL Framework Demo",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		1280, 720,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

	if (win == nullptr)
	{
		SDL_Log("Unable to create window: %s", SDL_GetError());
		return 1;
	}
	const Uint32 mainWindowID = SDL_GetWindowID(win);

	SDL_GLContext context = SDL_GL_CreateContext(win);
	if (context == nullptr)
	{
		SDL_Log("Unable to create OpenGL context: %s", SDL_GetError());
		return 1;
	}

	// turn off vsync so that we can run meaningful benchmarks
	SDL_GL_SetSwapInterval(0);

	const char* glsl_version = "#version 450";
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
	ImGui_ImplSDL2_InitForOpenGL(win, context);
	ImGui_ImplOpenGL3_Init(glsl_version);


	// Start GLEW
	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		SDL_Log("Unable to initialize GLEW!");
		return 1;
	}

	// Check OpenGL version
	std::pair<int, int> glVersion = { -1, -1 };
	glGetIntegerv(GL_MAJOR_VERSION, &glVersion.first);
	glGetIntegerv(GL_MINOR_VERSION, &glVersion.second);
	std::cout << "Running OpenGL " << glVersion.first << "." << glVersion.second << std::endl;

	if (glVersion.first == -1 && glVersion.second == -1)
	{
		SDL_Log("Unable to create OpenGL context!");
		return 1;
	}

	{
		App app;
		bool quit = false;
		SDL_Event ev;

		// https://discourse.libsdl.org/t/wip-windows-high-dpi-support/22446
		int canvas_width, canvas_height;
		SDL_GL_GetDrawableSize(win, &canvas_width, &canvas_height);

		if (!app.Init(canvas_width, canvas_height))
		{
			SDL_Log("Error during app initialization!");
			quit = true;
		}

		int window_size[2];
		SDL_GetWindowSize(win, window_size, window_size + 1);
		int window_id = SDL_GetWindowID(win);

		while (!quit)
		{
			// amíg van feldolgozandó üzenet dolgozzuk fel mindet:
			while (SDL_PollEvent(&ev))
			{
				ImGui_ImplSDL2_ProcessEvent(&ev);
				bool is_mouse_captured = ImGui::GetIO().WantCaptureMouse;
				bool is_keyboard_captured = ImGui::GetIO().WantCaptureKeyboard;
				switch (ev.type)
				{
				case SDL_WINDOWEVENT:
					if (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED && window_id == ev.window.windowID) {
						app.Resize(window_size[0] = ev.window.data1, window_size[1] = ev.window.data2);
						glViewport(0,0, window_size[0], window_size[1]);
					}
					else if (ev.window.event == SDL_WINDOWEVENT_CLOSE) {
						if (ev.window.windowID == mainWindowID) {
							quit = true;
						}
					}
					break;
				case SDL_KEYDOWN:
					if (!is_keyboard_captured && ev.key.keysym.sym == SDLK_ESCAPE)	quit = true;
					if(!is_keyboard_captured) app.KeyboardDown(ev.key);
					break;
				case SDL_KEYUP:
					if(!is_keyboard_captured) app.KeyboardUp(ev.key);	break;
				case SDL_MOUSEBUTTONDOWN:	if(!is_mouse_captured) app.MouseDown(ev.button);	break;
				case SDL_MOUSEBUTTONUP:		if(!is_mouse_captured) app.MouseUp(ev.button);		break;
				case SDL_MOUSEWHEEL:		if(!is_mouse_captured) app.MouseWheel(ev.wheel);	break;
				case SDL_MOUSEMOTION:		if(!is_mouse_captured) app.MouseMove(ev.motion);	break;
				case SDL_QUIT:				quit = true;										break;
				}
			}

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame(win);
			ImGui::NewFrame();

			//ImGui::ShowDemoWindow();
			//ImGui::ShowAutoTestWindow();

			app.Update();
			app.Render();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				//SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				SDL_GL_MakeCurrent(win, context);
			}
			SDL_GL_SwapWindow(win);
		}

	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(win);

	return 0;
}