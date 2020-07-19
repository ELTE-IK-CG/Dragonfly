#pragma once
#include "Sample.h"
#include <chrono>
#include <algorithm>
#include <iostream>
#include <functional>
#include "../Traits/EventHandlerTraits.h"
#include <ImGui/imgui.h>
#include <ImGui-addons/impl/imgui_impl_sdl.h>
#include <ImGui-addons/impl/imgui_impl_opengl3.h>

namespace df {

ENUM_CLASS_FLAG_OPERATORS(Sample::FLAGS);

template<typename F_, typename E_>
inline void Sample::_CallEventHandlers(std::multimap<int, F_>& queue_, E_&& arg_)
{
	bool wastrue = false;	int priority;
	for (auto& pF : queue_)
	{
		if (wastrue && pF.first > priority) break;
		wastrue |= pF.second(arg_);
		priority = pF.first;
	}
}

template<typename C_>
inline void Sample::AddHandlerClass(C_& c_, int priority_)
{
	using namespace std::placeholders;
	if constexpr (df::has_static_HandleKeyDown<C_, SDL_KeyboardEvent>()) {
		_keydown.emplace(-priority_, Callback_KeyBoard(&C_::HandleKeyDown));
	}
	else if constexpr (df::has_HandleKeyDown<C_, SDL_KeyboardEvent>()) {
		_keydown.emplace(-priority_, std::bind(&C_::HandleKeyDown, &c_, _1));
	}
	if constexpr (df::has_static_HandleKeyUp<C_, SDL_KeyboardEvent>()) {
		_keyup.emplace(-priority_, Callback_KeyBoard(&C_::HandleKeyUp));
	}
	else if constexpr (df::has_HandleKeyUp<C_, SDL_KeyboardEvent>()) {
		_keyup.emplace(-priority_, std::bind(&C_::HandleKeyUp, &c_, _1));
	}
	if constexpr (df::has_static_HandleMouseUp<C_, SDL_MouseButtonEvent>()) {
		_mouseup.emplace(-priority_, &C_::HandleMouseUp);
	}
	else if constexpr (df::has_HandleMouseUp<C_, SDL_MouseButtonEvent>()) {
		_mouseup.emplace(-priority_, std::bind(&C_::HandleMouseUp, &c_, _1));
	}
	if constexpr (df::has_static_HandleMouseDown<C_, SDL_MouseButtonEvent>()) {
		_mousedown.emplace(-priority_, &C_::HandleMouseDown);
	}
	else if constexpr (df::has_HandleMouseDown<C_, SDL_MouseButtonEvent>()) {
		_mousedown.emplace(-priority_, std::bind(&C_::HandleMouseDown, &c_, _1));
	}
	if constexpr (df::has_static_HandleMouseMotion<C_, SDL_MouseMotionEvent>()) {
		_mousemove.emplace(-priority_, &C_::HandleMouseMotion);
	}
	else if constexpr (df::has_HandleMouseMotion<C_, SDL_MouseMotionEvent>()) {
		_mousemove.emplace(-priority_, std::bind(&C_::HandleMouseMotion, &c_, _1));
	}
	if constexpr (df::has_static_HandleMouseWheel<C_, SDL_MouseWheelEvent>()) {
		_mousewheel.emplace(-priority_, &C_::HandleMouseWheel);
	}
	else if constexpr (df::has_HandleMouseWheel<C_, SDL_MouseWheelEvent>()) {
		_mousewheel.emplace(-priority_, std::bind(&C_::HandleMouseWheel, &c_, _1));
	}
	if constexpr (df::has_static_HandleResize<C_, int, int>()) {
		_resize.emplace_back(&C_::HandleResize);
	}
	else if constexpr (df::has_HandleResize<C_, int, int>()) {
		_resize.emplace_back(std::bind(&C_::HandleResize, &c_, _1, _2));
	}
}

template<typename C_>
inline void Sample::AddHandlerClass(int priority_)
{
	if constexpr (df::has_static_HandleKeyDown<C_, SDL_KeyboardEvent>()) {
		_keydown.emplace(-priority_, &C_::HandleKeyDown);
	}
	if constexpr (df::has_static_HandleKeyUp<C_, SDL_KeyboardEvent>()) {
		_keyup.emplace(-priority_, &C_::HandleKeyUp);
	}
	if constexpr (df::has_static_HandleMouseUp<C_, SDL_MouseButtonEvent>()) {
		_mouseup.emplace(-priority_, &C_::HandleMouseUp);
	}
	if constexpr (df::has_static_HandleMouseDown<C_, SDL_MouseButtonEvent>()) {
		_mousedown.emplace(-priority_, &C_::HandleMouseDown);
	}
	if constexpr (df::has_static_HandleMouseMotion<C_, SDL_MouseMotionEvent>()) {
		_mousemove.emplace(-priority_, &C_::HandleMouseMotion);
	}
	if constexpr (df::has_static_HandleMouseWheel<C_, SDL_MouseWheelEvent>()) {
		_mousewheel.emplace(-priority_, &C_::HandleMouseWheel);
	}
	if constexpr (df::has_static_HandleResize<C_, int, int>()) {
		_resize.emplace_back(&C_::HandleResize);
	}
}

inline void df::Sample::_CallResizeHandlers(std::vector<Callback_Resize>& handlers_, int w_, int h_)
{
	for (auto& pF : handlers_) {
		pF(w_, h_);
	}
}

template<typename F>
inline void Sample::Run(F&& RenderFunc_)
{
	_quit = false;
	SDL_Event ev;
	int canvas_width, canvas_height;
	SDL_GL_GetDrawableSize(_mainWindowPtr, &canvas_width, &canvas_height);
	_CallResizeHandlers(_resize, canvas_width, canvas_height);
	while (!_quit)
	{
		while (SDL_PollEvent(&ev))
		{
			ImGui_ImplSDL2_ProcessEvent(&ev);
			switch (ev.type)
			{
			case SDL_KEYUP:				_CallEventHandlers(_keyup, ev.key);				break;
			case SDL_KEYDOWN:			_CallEventHandlers(_keydown, ev.key);			break;
			case SDL_MOUSEBUTTONDOWN:	_CallEventHandlers(_mousedown, ev.button);		break;
			case SDL_MOUSEBUTTONUP:		_CallEventHandlers(_mouseup, ev.button);		break;
			case SDL_MOUSEMOTION:		_CallEventHandlers(_mousemove, ev.motion);		break;
			case SDL_MOUSEWHEEL:		_CallEventHandlers(_mousewheel, ev.wheel);		break;
			case SDL_WINDOWEVENT:
				if (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED && _mainWindowID == ev.window.windowID)
				{
					SDL_GL_GetDrawableSize(_mainWindowPtr, &ev.window.data1, &ev.window.data2);
					_CallResizeHandlers(_resize, ev.window.data1, ev.window.data2);
					glViewport(0, 0, ev.window.data1, ev.window.data2); //are we sure?
				}
				else if (ev.window.event == SDL_WINDOWEVENT_CLOSE && _mainWindowID == ev.window.windowID) {
					Quit();
				}
				break;
			case SDL_QUIT:	Quit();	break;
			default: break;
			}
		}
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(_mainWindowPtr);
		ImGui::NewFrame();

		static std::chrono::high_resolution_clock::time_point last_measurement = std::chrono::high_resolution_clock::now();
		float deltaTime_ = static_cast<float>(static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - last_measurement).count()) / 1000000000.0);
		last_measurement = std::chrono::high_resolution_clock::now();

		RenderFunc_(deltaTime_); //delta time in ms

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			SDL_GL_MakeCurrent(_mainWindowPtr, _mainWindowContext);
		}
		SDL_GL_SwapWindow(_mainWindowPtr);
	}
}

} // namespace df