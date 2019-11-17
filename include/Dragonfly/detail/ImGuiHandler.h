#pragma once
#include <SDL/SDL.h>
#include <ImGui/imgui.h>

namespace df
{

class ImGuiHandler {
public:
	bool HandleKeyUp(const SDL_KeyboardEvent&) {
		return ImGui::GetIO().WantCaptureKeyboard;
	}
	bool HandleKeyDown(const SDL_KeyboardEvent&) {
		return ImGui::GetIO().WantCaptureKeyboard;
	}
	 bool HandleMouseUp(const SDL_MouseButtonEvent&) {
		return ImGui::GetIO().WantCaptureMouse;
	}
	 bool HandleMouseDown(const SDL_MouseButtonEvent&) {
		return ImGui::GetIO().WantCaptureMouse;
	}
	 bool HandleMouseMotion(const SDL_MouseMotionEvent&) {
		return ImGui::GetIO().WantCaptureMouse;
	}
	 bool HandleMouseWheel(const SDL_MouseWheelEvent&) {
		return ImGui::GetIO().WantCaptureMouse;
	}
};

}