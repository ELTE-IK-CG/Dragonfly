#include <chrono>
#include <math.h>
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"

using namespace df;

int Camera::CamCount_ = 0; //static variable

bool Camera::Update()
{
	static std::chrono::high_resolution_clock::time_point last_measurement = std::chrono::high_resolution_clock::now();
	deltaTime_ =static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - last_measurement).count())/1000000000.0;
	last_measurement = std::chrono::high_resolution_clock::now();

	if (uv_changed)
	{
		at_ = eye_ + eyeAtDistance_ * glm::vec3(cosf(u_)*sinf(v_), cosf(v_), sinf(u_) * sinf(v_));
		look_changed = true;
	}
	if (look_changed)
	{
		frwdDir_ = glm::normalize(at_ - eye_);
		rghtDir_ = glm::normalize(glm::cross(frwdDir_, up_));
		eyeAtDistance_ = glm::length(at_ - eye_);
		if (!uv_changed) {
			u_ = atan2f(frwdDir_.z, frwdDir_.x);
			v_ = acosf(frwdDir_.y);
		}
		view_changed = true;
	}

	view_changed |= goFrwd_ != 0 || goRght_ != 0;

	if (view_changed)
	{
		glm::vec3 dv = (goFrwd_ * frwdDir_ + goRght_ * rghtDir_) * moveSpeed_ * static_cast<float>(deltaTime_);
		eye_ += dv;	at_ += dv;
		viewMatrix_ = glm::lookAt(eye_, at_, up_);
	}
	if (proj_changed)
	{
		glm::vec2 resn = glm::normalize(resolution_);
		float fow = acos(cos(glm::radians(fowDegs_*0.5f))*resn.x); //i hope
		tanPixelFow_ = sqrtf(2.0f) * tanf(glm::radians(fowDegs_)) / glm::length(resolution_);
		projMatrix_ = glm::perspective(fow, resn.x/resn.y, nearDist_, farDist_);
	}
	bool anychange = view_changed || proj_changed;
	if (anychange)
	{
		viewProjMatrix_ = projMatrix_ * viewMatrix_;
		viewProjInverse_ = glm::inverse(viewProjMatrix_);
	}
	view_changed = proj_changed = look_changed = uv_changed = false;
	return anychange;
}

bool Camera::RenderUI()
{
	ImGui::SetNextWindowSize({ 500,350 }, ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Cameras",&isUiOpen_))
	{
		if (ImGui::CollapsingHeader(name_.c_str(), ImGuiTreeNodeFlags_CollapsingHeader)) {
			if (ImGui::BeginChild(name_.c_str(), { 0,150 }, true)) {
				float w = ImGui::GetContentRegionAvailWidth();

				ImGui::Text("Last frame times %f ms", 1000 * this->deltaTime_);

				ImGui::PushItemWidth(0.9f * w);
				look_changed |= ImGui::DragFloat3("Eye", &eye_.x, 0.4f);
				look_changed |= ImGui::DragFloat3("Up", &up_.x, 0.2f);
				look_changed |= ImGui::DragFloat3("At", &at_.x, 0.4f);
				ImGui::PopItemWidth();

				ImGui::PushItemWidth(0.25f * w);
				proj_changed |= ImGui::DragFloat("FoW", &fowDegs_, 1.f, 0.f, 180.f, "%.1f%°");	ImGui::SameLine();
				proj_changed |= ImGui::DragFloat("Near", &nearDist_);						ImGui::SameLine();
				proj_changed |= ImGui::DragFloat("Far", &farDist_);

				ImGui::DragFloat("Speed", &moveSpeed_, 0.5f);			ImGui::SameLine();
				ImGui::Checkbox("Slow", &shiftHeld_);					ImGui::SameLine();
				static bool bdebug = false;
				ImGui::Checkbox("Debug", &bdebug);
				ImGui::PopItemWidth();

				if (bdebug) {
					ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.5, 0, 0, 1));
					ImGui::BeginChild("DangerZone", { 0, 110 }, true);
					ImGui::PushItemWidth(0.6f * w);
					view_changed |= ImGui::DragFloat3("Frwrd", &frwdDir_.x);	ImGui::SameLine();
					ImGui::PopItemWidth(); ImGui::PushItemWidth(0.2f * w);
					view_changed |= ImGui::DragFloat("goFw", &goFrwd_);
					ImGui::PopItemWidth(); ImGui::PushItemWidth(0.6f * w);
					view_changed |= ImGui::DragFloat3("Right", &rghtDir_.x);	ImGui::SameLine();
					ImGui::PopItemWidth(); ImGui::PushItemWidth(0.2f * w);
					view_changed |= ImGui::DragFloat("goRg", &goRght_);
					ImGui::PopItemWidth(); ImGui::PushItemWidth(0.4f * w);
					ImGui::DragFloat2("u and v   ", &u_); ImGui::SameLine();
					ImGui::PopItemWidth(); ImGui::PushItemWidth(0.2f * w);
					ImGui::DragFloat("dist", &eyeAtDistance_);
					ImGui::PopItemWidth(); ImGui::PushItemWidth(0.4f * w);
					proj_changed |= ImGui::DragFloat2("Resolution", &resolution_.x, 1.f, 0.f, 2000.f, "%0.1f"); ImGui::SameLine();
					ImGui::PopItemWidth(); ImGui::PushItemWidth(0.2f * w);
					proj_changed |= ImGui::DragFloat("Pixel FoW", &tanPixelFow_);
					ImGui::PopItemWidth();
					ImGui::EndChild();
					ImGui::PopStyleColor();
				}
			}ImGui::EndChild();
		}
	}
	ImGui::End();
	return isUiOpen_;
}


bool Camera::HandleKeyDown(const SDL_KeyboardEvent& key)
{
	switch ( key.keysym.sym )
	{
	case SDLK_LSHIFT: case SDLK_RSHIFT:
		if ( !shiftHeld_ ) {
			shiftHeld_ = true;
			moveSpeed_ /= 4.0f;
		} break;
	case SDLK_w:		goFrwd_ = 1;		break;
	case SDLK_s:		goFrwd_ = -1;	break;
	case SDLK_a:		goRght_ = -1;	break;
	case SDLK_d:		goRght_ = 1;	break;
	}
	view_changed |= goFrwd_ != 0 || goRght_ != 0;
	return goFrwd_ != 0 || goRght_ != 0;
}

bool Camera::HandleKeyUp(const SDL_KeyboardEvent& key)
{
	float current_speed = moveSpeed_;
	switch ( key.keysym.sym )
	{
	case SDLK_LSHIFT: case SDLK_RSHIFT:
		if ( shiftHeld_ ) {
			shiftHeld_ = false;
			moveSpeed_ *= 4.0f;
		} break;
	case SDLK_w: case SDLK_s:		goFrwd_ = 0;		break;
	case SDLK_a: case SDLK_d:		goRght_ = 0;	break;
	}
	return goFrwd_ == 0 || goRght_ == 0;
}

bool Camera::HandleMouseMotion(const SDL_MouseMotionEvent& mouse)
{
	if ( mouse.state & SDL_BUTTON_LMASK ) {
		u_ += mouse.xrel / 128.0f;
		v_ = glm::clamp<float>(v_ + mouse.yrel / 128.0f, 0.01f, 3.14f);
		uv_changed = true;
		return true;
	}
	return false;
}
bool Camera::HandleMouseWheel(const SDL_MouseWheelEvent& wheel)
{
	if (wheel.y == 1) {
		moveSpeed_ *= 1.25f;
		return true;
	}
	else if (wheel.y == -1) {
		moveSpeed_ *= 0.75f;
		return true;
	}
	else return false;
}