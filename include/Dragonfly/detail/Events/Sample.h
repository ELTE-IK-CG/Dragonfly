#pragma once
#include <map>
#include <vector>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <functional>
#include <SDL/SDL.h>
#include "../../config.h"
#include "../Traits/EventHandlerTraits.h"
#include <ImGui/imgui.h>
#include <ImGui-addons/impl/imgui_impl_sdl.h>
#include <ImGui-addons/impl/imgui_impl_opengl3.h>

namespace df {

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
	static void callEventHandlers(std::multimap<int, F>& queue, E&& arg);
	static void callResizeHandlers(std::vector<Callback_Resize>& handlers, int w, int h);
	bool quit = false;
	Uint32 mainWindowID;
	SDL_Window *win = nullptr;
	SDL_GLContext context = nullptr;
public:
	Sample(const char* name = "OpenGL Sample", int width = 720, int height = 480, int vsync = -1);
	~Sample();

	void Quit() { quit = true; }
	
	Sample& AddKeyDown    (Callback_KeyBoard&& f,    int priority = 0) {    _keydown.emplace(-priority, std::move(f)); return *this; }
	Sample& AddKeyUp      (Callback_KeyBoard&& f,    int priority = 0) {      _keyup.emplace(-priority, std::move(f)); return *this; }
	Sample& AddMouseDown  (Callback_MouseButton&& f, int priority = 0) {  _mousedown.emplace(-priority, std::move(f)); return *this; }
	Sample& AddMouseUp    (Callback_MouseButton&& f, int priority = 0) {    _mouseup.emplace(-priority, std::move(f)); return *this; }
	Sample& AddMouseMotion(Callback_MouseMotion&& f, int priority = 0) {  _mousemove.emplace(-priority, std::move(f)); return *this; }
	Sample& AddMouseWheel (Callback_MouseWheel&& f,  int priority = 0) { _mousewheel.emplace(-priority, std::move(f)); return *this; }
	Sample& AddResize     (Callback_Resize&& f) { _resize.emplace_back(std::move(f)); return *this; }

	// The following member functions will be added as handler functions from the handler class (if it has them):
	// HandleKeyUp(const SDL_KeyboardEvent&)
	// HandleKeyDown(const SDL_KeyboardEvent&)
	// HandleMouseUp(const SDL_MouseButtonEvent&)
	// HandleMouseDown(const SDL_MouseButtonEvent&)
	// HandleMouseMotion(const SDL_MouseMotionEvent&)
	// HandleMouseWheel(const SDL_MouseWheelEvent&)
	// HandleResize(int, int)
	template<typename C> void AddHandlerClass(C& c, int priority = 0);
	template<typename C> void AddStaticHandlerClass(int priority = 0);

	template<typename F> void Run(F&& RenderFunc); //lambda gets delta time in ms
};

template<typename F, typename E>
inline void Sample::callEventHandlers(std::multimap<int, F>& queue, E&& arg)
{
	bool wastrue = false;	int priority;
	for (auto& pF : queue)
	{
		if (wastrue && pF.first > priority) break;
		wastrue |= pF.second(arg);
		priority = pF.first;
	}
}

template<typename C>
inline void Sample::AddHandlerClass(C& c, int priority)
{
	using namespace std::placeholders;
	if constexpr (df::has_static_HandleKeyDown<C, SDL_KeyboardEvent>()) {
		_keydown.emplace(-priority, Callback_KeyBoard(&C::HandleKeyDown));
	}
	else if constexpr (df::has_HandleKeyDown<C, SDL_KeyboardEvent>()) {
		_keydown.emplace(-priority, std::bind(&C::HandleKeyDown, &c, _1));
	}
	if constexpr (df::has_static_HandleKeyUp<C, SDL_KeyboardEvent>()) {
		_keyup.emplace(-priority, Callback_KeyBoard(&C::HandleKeyUp));
	}
	else if constexpr (df::has_HandleKeyUp<C, SDL_KeyboardEvent>()) {
		_keyup.emplace(-priority, std::bind(&C::HandleKeyUp, &c, _1));
	}
	if constexpr (df::has_static_HandleMouseUp<C, SDL_MouseButtonEvent>()) {
		_mouseup.emplace(-priority, &C::HandleMouseUp);
	}
	else if constexpr (df::has_HandleMouseUp<C, SDL_MouseButtonEvent>()) {
		_mouseup.emplace(-priority, std::bind(&C::HandleMouseUp, &c, _1));
	}
	if constexpr (df::has_static_HandleMouseDown<C, SDL_MouseButtonEvent>()) {
		_mousedown.emplace(-priority, &C::HandleMouseDown);
	}
	else if constexpr (df::has_HandleMouseDown<C, SDL_MouseButtonEvent>()) {
		_mousedown.emplace(-priority, std::bind(&C::HandleMouseDown, &c, _1));
	}
	if constexpr (df::has_static_HandleMouseMotion<C, SDL_MouseMotionEvent>()) {
		_mousemove.emplace(-priority, &C::HandleMouseMotion);
	}
	else if constexpr (df::has_HandleMouseMotion<C, SDL_MouseMotionEvent>()) {
		_mousemove.emplace(-priority, std::bind(&C::HandleMouseMotion, &c, _1));
	}
	if constexpr (df::has_static_HandleMouseWheel<C, SDL_MouseWheelEvent>()) {
		_mousewheel.emplace(-priority, &C::HandleMouseWheel);
	}
	else if constexpr (df::has_HandleMouseWheel<C, SDL_MouseWheelEvent>()) {
		_mousewheel.emplace(-priority, std::bind(&C::HandleMouseWheel, &c, _1));
	}
	if constexpr (df::has_static_HandleResize<C, int, int>()) {
		_resize.emplace_back(&C::HandleResize);
	}
	else if constexpr (df::has_HandleResize<C, int, int>()) {
		_resize.emplace_back(std::bind(&C::HandleResize, &c, _1, _2));
	}
}

template<typename C>
inline void Sample::AddStaticHandlerClass(int priority)
{
	if constexpr (df::has_static_HandleKeyDown<C, SDL_KeyboardEvent>()) {
		_keydown.emplace(-priority, &C::HandleKeyDown);
	}
	if constexpr (df::has_static_HandleKeyUp<C, SDL_KeyboardEvent>()) {
		_keyup.emplace(-priority, &C::HandleKeyUp);
	}
	if constexpr (df::has_static_HandleMouseUp<C, SDL_MouseButtonEvent>()) {
		_mouseup.emplace(-priority, &C::HandleMouseUp);
	}
	if constexpr (df::has_static_HandleMouseDown<C, SDL_MouseButtonEvent>()) {
		_mousedown.emplace(-priority, &C::HandleMouseDown);
	}
	if constexpr (df::has_static_HandleMouseMotion<C, SDL_MouseMotionEvent>()) {
		_mousemove.emplace(-priority, &C::HandleMouseMotion);
	}
	if constexpr (df::has_static_HandleMouseWheel<C, SDL_MouseWheelEvent>()) {
		_mousewheel.emplace(-priority, &C::HandleMouseWheel);
	}
	if constexpr (df::has_static_HandleResize<C, int, int>()) {
		_resize.emplace_back(&C::HandleResize);
	}
}

inline void df::Sample::callResizeHandlers(std::vector<Callback_Resize>& handlers, int w, int h)
{
	for (auto& pF : handlers) {
		pF(w, h);
	}
}

template<typename F>
inline void Sample::Run(F&& RenderFunc)
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
			case SDL_MOUSEBUTTONDOWN:	callEventHandlers(_mousedown, ev.button);		break;
			case SDL_MOUSEBUTTONUP:		callEventHandlers(_mouseup, ev.button);			break;
			case SDL_MOUSEMOTION:		callEventHandlers(_mousemove, ev.motion);		break;
			case SDL_MOUSEWHEEL:		callEventHandlers(_mousewheel, ev.wheel);		break;
			case SDL_WINDOWEVENT:
				if (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED && mainWindowID == ev.window.windowID)
				{
					SDL_GL_GetDrawableSize(win, &ev.window.data1, &ev.window.data2);
					callResizeHandlers(_resize, ev.window.data1, ev.window.data2);
					glViewport(0, 0, ev.window.data1, ev.window.data2); //are we sure?
				}
				else if (ev.window.event == SDL_WINDOWEVENT_CLOSE && mainWindowID == ev.window.windowID) {
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

		static std::chrono::high_resolution_clock::time_point last_measurement = std::chrono::high_resolution_clock::now();
		float deltaTime_ = static_cast<float>(static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - last_measurement).count()) / 1000000000.0);
		last_measurement = std::chrono::high_resolution_clock::now();

		RenderFunc(deltaTime_); //delta time in ms

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

} // namespace df