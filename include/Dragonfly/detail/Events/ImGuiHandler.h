#pragma once
#include <SDL/SDL.h>
#include <ImGui/imgui.h>

namespace df
{

class ImGuiHandler {
public:
	static bool HandleKeyUp(const SDL_KeyboardEvent&) {
		return ImGui::GetIO().WantCaptureKeyboard;
	}
	static bool HandleKeyDown(const SDL_KeyboardEvent&) {
		return ImGui::GetIO().WantCaptureKeyboard;
	}
	 static bool HandleMouseUp(const SDL_MouseButtonEvent&) {
		return ImGui::GetIO().WantCaptureMouse;
	}
	 static bool HandleMouseDown(const SDL_MouseButtonEvent&) {
		return ImGui::GetIO().WantCaptureMouse;
	}
	 static bool HandleMouseMotion(const SDL_MouseMotionEvent&) {
		return ImGui::GetIO().WantCaptureMouse;
	}
	 static bool HandleMouseWheel(const SDL_MouseWheelEvent&) {
		return ImGui::GetIO().WantCaptureMouse;
	}
};

}