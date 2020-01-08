#pragma once
#include <functional>
#include <algorithm>
#include <map>
#include <vector>
#include <SDL/SDL.h>
#include "config.h"
#include <ImGui/imgui.h>
#include <ImGui-addons/impl/imgui_impl_sdl.h>
#include <ImGui-addons/impl/imgui_impl_opengl3.h>
#include "detail/Traits/EventHandlerTraits.h"

#include <iostream>

class Sample
{
protected:
	using Callback_KeyBoard		= std::function<bool(const SDL_KeyboardEvent &)>;
	using Callback_MouseButton	= std::function<bool(const SDL_MouseButtonEvent&)>;
	using Callback_MouseMotion	= std::function<bool(const SDL_MouseMotionEvent&)>;
	using Callback_MouseWheel	= std::function<bool(const SDL_MouseWheelEvent&)>;
	using Callback_Resize		= std::function<void(int, int)>;

	std::multimap<int, Callback_KeyBoard>		_keydown,	_keyup;
	std::multimap<int, Callback_MouseButton>	_mousedown,	_mouseup;
	std::multimap<int, Callback_MouseMotion>	_mousemove;
	std::multimap<int, Callback_MouseWheel>		_mousewheel;
	std::vector<Callback_Resize>				_resize;

	template<typename F,typename E>
	static void callEventHandlers(std::multimap<int, F>& queue, E&&arg)
	{
		bool wastrue = false;	int priority;
		for (auto& pF : queue)
		{
			if (wastrue && pF.first > priority) break;
			wastrue |= pF.second(arg);
			priority = pF.first;
		}
	}
	static void callResizeHandlers(std::vector<Callback_Resize>& handlers, int w, int h)
	{
		for (auto& pF : handlers)
		{
			pF(w, h);
		}
	}
	bool quit = false;
	Uint32 mainWindowID;
	SDL_Window *win = nullptr;
	SDL_GLContext context = nullptr;
public:
	Sample(const char* name = "OpenGL Sample", int width = 720, int height = 480, int vsync = -1);
	~Sample();

	void Quit() { quit = true; }

	template<typename F>
	void AddKeyDown(F&& f, int priority = 0) { _keydown.emplace(-priority, Callback_KeyBoard(f)); }
	template<typename F>
	void AddKeyUp(F&& f, int priority = 0){	_keyup.emplace(-priority, Callback_KeyBoard(f)); }

	template<typename F>
	void AddMouseDown(F&& f, int priority = 0) { _mousedown.emplace(-priority, Callback_MouseButton(f)); }
	template<typename F>
	void AddMouseUp(F&& f, int priority = 0) { _mouseup.emplace(-priority, Callback_MouseButton(f)); }
	template<typename F>
	void AddMouseMotion(F&& f, int priority = 0) { _mousemove.emplace(-priority, Callback_MouseMotion(f)); }
	template<typename F>
	void AddMouseWheel(F&& f, int priority = 0) { _mousewheel.emplace(-priority, Callback_MouseMotion(f)); }
	template<typename F>
	void AddResize(F&& f) { _resize.emplace_back(Callback_Resize(f)); }

	// The following member functions will be added as handler functions from the handler class (if it has them):
	// HandleKeyUp(const SDL_KeyboardEvent&)
	// HandleKeyDown(const SDL_KeyboardEvent&)
	// HandleMouseUp(const SDL_MouseButtonEvent&)
	// HandleMouseDown(const SDL_MouseButtonEvent&)
	// HandleMouseMotion(const SDL_MouseMotionEvent&)
	// HandleMouseWheel(const SDL_MouseWheelEvent&)
	// HandleResize(int, int)
	template<typename C>
	void AddHandlerClass(C& c, int priority = 0) 
	{
		using namespace std::placeholders;
		if constexpr (df::has_static_HandleKeyDown<C, SDL_KeyboardEvent>()) {
			std::cout << "HandleKeyDown static\n";
			_keydown.emplace(-priority, Callback_KeyBoard(&C::HandleKeyDown));
		}
		else if constexpr (df::has_HandleKeyDown<C, SDL_KeyboardEvent>()) {
			std::cout << "HandleKeyDown\n";
			_keydown.emplace(-priority, std::bind(&C::HandleKeyDown, &c, _1));
		}
		if constexpr (df::has_static_HandleKeyUp<C, SDL_KeyboardEvent>()) {
			std::cout << "HandleKeyUp static\n";
			_keyup.emplace(-priority, Callback_KeyBoard(&C::HandleKeyUp));
		}
		else if constexpr (df::has_HandleKeyUp<C, SDL_KeyboardEvent>()) {
			std::cout << "HandleKeyUp\n";
			_keyup.emplace(-priority, std::bind(&C::HandleKeyUp, &c, _1));
		}
		if constexpr (df::has_static_HandleMouseUp<C, SDL_MouseButtonEvent>()) {
			std::cout << "HandleMouseUp static\n";
			_mouseup.emplace(-priority, &C::HandleMouseUp);
		}
		else if constexpr (df::has_HandleMouseUp<C, SDL_MouseButtonEvent>()) {
			std::cout << "HandleMouseUp\n";
			_mouseup.emplace(-priority, std::bind(&C::HandleMouseUp, &c, _1));
		}
		if constexpr (df::has_static_HandleMouseDown<C, SDL_MouseButtonEvent>()) {
			std::cout << "HandleMouseDown static\n";
			_mousedown.emplace(-priority, &C::HandleMouseDown);
		}
		else if constexpr (df::has_HandleMouseDown<C, SDL_MouseButtonEvent>()) {
			std::cout << "HandleMouseDown\n";
			_mousedown.emplace(-priority, std::bind(&C::HandleMouseDown, &c, _1));
		}
		if constexpr (df::has_static_HandleMouseMotion<C, SDL_MouseMotionEvent>()) {
			std::cout << "HandleMouseMotion static\n";
			_mousemove.emplace(-priority, &C::HandleMouseMotion);
		}
		else if constexpr (df::has_HandleMouseMotion<C, SDL_MouseMotionEvent>()) {
			std::cout << "HandleMouseMotion\n";
			_mousemove.emplace(-priority, std::bind(&C::HandleMouseMotion, &c, _1));
		}
		if constexpr (df::has_static_HandleMouseWheel<C, SDL_MouseWheelEvent>()) {
			std::cout << "HandleMouseWheel static\n";
			_mousewheel.emplace(-priority, &C::HandleMouseWheel);
		}
		else if constexpr (df::has_HandleMouseWheel<C, SDL_MouseWheelEvent>()) {
			std::cout << "HandleMouseWheel\n";
			_mousewheel.emplace(-priority, std::bind(&C::HandleMouseWheel, &c, _1));
		}
		if constexpr (df::has_static_HandleResize<C, int, int>()) {
			std::cout << "HandleResize static\n";
			_resize.emplace_back(&C::HandleResize);
		}
		else if constexpr (df::has_HandleResize<C, int, int>()) {
			std::cout << "HandleResize\n";
			_resize.emplace_back(std::bind(&C::HandleResize, &c, _1, _2));
		}
	}
	template<typename C>
	void AddStaticHandlerClass(int priority = 0)
	{
		if constexpr (df::has_static_HandleKeyDown<C, SDL_KeyboardEvent>()) {
			std::cout << "HandleKeyDown static\n";
			_keydown.emplace(-priority, &C::HandleKeyDown);
		}
		if constexpr (df::has_static_HandleKeyUp<C, SDL_KeyboardEvent>()) {
			std::cout << "HandleKeyUp static\n";
			_keyup.emplace(-priority, &C::HandleKeyUp);
		}
		if constexpr (df::has_static_HandleMouseUp<C, SDL_MouseButtonEvent>()) {
			std::cout << "HandleMouseUp static\n";
			_mouseup.emplace(-priority, &C::HandleMouseUp);
		}
		if constexpr (df::has_static_HandleMouseDown<C, SDL_MouseButtonEvent>()) {
			std::cout << "HandleMouseDown static\n";
			_mousedown.emplace(-priority, &C::HandleMouseDown);
		}
		if constexpr (df::has_static_HandleMouseMotion<C, SDL_MouseMotionEvent>()) {
			std::cout << "HandleMouseMotion static\n";
			_mousemove.emplace(-priority, &C::HandleMouseMotion);
		}
		if constexpr (df::has_static_HandleMouseWheel<C, SDL_MouseWheelEvent>()) {
			std::cout << "HandleMouseWheel static\n";
			_mousewheel.emplace(-priority, &C::HandleMouseWheel);
		}
		if constexpr (df::has_static_HandleResize<C, int, int>()) {
			std::cout << "HandleResize static\n";
			_resize.emplace_back(&C::HandleResize);
		}
	}

	template<typename F>
	void Run(F&& RenderFunc)
	{
		quit = false;
		SDL_Event ev;
		int canvas_width, canvas_height;
		SDL_GL_GetDrawableSize(win, &canvas_width, &canvas_height);
		callResizeHandlers(_resize, canvas_width, canvas_height);
		while (!quit)
		{
			while (SDL_PollEvent(&ev))
			{
				ImGui_ImplSDL2_ProcessEvent(&ev);
				switch (ev.type)
				{
				case SDL_KEYUP:				callEventHandlers(_keyup, ev.key);				break;
				case SDL_KEYDOWN:			callEventHandlers(_keydown, ev.key);			break;
				case SDL_MOUSEBUTTONDOWN:	callEventHandlers(_mouseup, ev.button);			break;
				case SDL_MOUSEBUTTONUP:		callEventHandlers(_mousedown, ev.button);		break;
				case SDL_MOUSEMOTION:		callEventHandlers(_mousemove, ev.motion);		break;
				case SDL_MOUSEWHEEL:		callEventHandlers(_mousewheel, ev.wheel);		break;
				case SDL_WINDOWEVENT:
					if (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED && mainWindowID == ev.window.windowID)
					{
						SDL_GL_GetDrawableSize(win, &ev.window.data1, &ev.window.data2);
						callResizeHandlers(_resize, ev.window.data1, ev.window.data2);
						glViewport(0, 0, ev.window.data1, ev.window.data2); //are we sure?
					}
					else if (ev.window.event == SDL_WINDOWEVENT_CLOSE && mainWindowID == ev.window.windowID)
					{
						Quit();
					}
					break;
				case SDL_QUIT:	Quit();	break;
				default: break;
				}
			}
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame(win);
			ImGui::NewFrame();
			
			//todo delta time in ms
			RenderFunc(16.66f);

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				SDL_GL_MakeCurrent(win, context);
			}
			SDL_GL_SwapWindow(win);
		}
	}
};