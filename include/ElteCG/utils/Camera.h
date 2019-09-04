#pragma once

#include <SDL/SDL.h>
#include <glm/glm.hpp>
#include <string>

class Camera
{
public:
	Camera() = default;
	~Camera() = default;

	bool Update(); //
	bool RenderUI(std::string program_name);

	//setters

	inline Camera& LookAt(const glm::vec3 &_at) { at_ = _at; look_changed = true;	return *this; }
	inline Camera& SetView(const glm::vec3 &_eye, const glm::vec3 &_at, const glm::vec3 &_up) { eye_ = _eye; up_ = _up; return LookAt(_at); }
	inline Camera& SetSpeed(float units_per_sec) { moveSpeed_ = units_per_sec; return *this; }

	//getters

	inline float		GetSpeed() const { return moveSpeed_; }
	inline const glm::vec3&	GetEye()const {	return eye_; }
	inline const glm::vec3&	GetAt() const {	return at_; }
	inline const glm::vec3&	GetUp() const {	return up_; }
	inline		 glm::vec3	GetDir() const { return glm::normalize(at_ - eye_); }

	inline float GetTanPixelFow() { return tanPixelFow_; }
	inline const glm::mat4& GetView() const { return viewMatrix_; }
	inline const glm::mat4& GetProj() const { return projMatrix_;}
	inline const glm::mat4& GetViewProj() const { return viewProjMatrix_; }
	inline const glm::mat4& GetInverseViewProj() const { return viewProjInverse_; }

	inline		 glm::vec2 GetNearFarClips() const { return glm::vec2(nearDist_, farDist_); }
	inline		 glm::vec2 GetDepthcalcCoeffs() const { return glm::vec2(0.5f*(farDist_+nearDist_)/(farDist_-nearDist_)+0.5f, - farDist_*nearDist_/ (farDist_ - nearDist_)); }

	//integer pixel coordinates
	inline glm::vec3 GerRayDir(int x, int y) const { return GetRayDir(2.f*glm::vec2((float)x / resolution_.x, (float)y / resolution_.y)-1.f); }
	//[-1,1] normalized coordinates
	inline glm::vec3 GetRayDir(const glm::vec2 &pix) const {glm::vec4 v = viewProjInverse_ * glm::vec4(pix, +1, 1); return glm::normalize(glm::vec3(v.x, v.y, v.z) / v.w - eye_);}
	// Basically, the vertex shader does the same
	inline glm::vec3 GetVSProjection(const glm::vec3& pos) const { glm::vec4 v = viewProjMatrix_ * glm::vec4(pos, 1); return glm::vec3(v.x, v.y, v.z) / v.w; }
	// Returns pixel coordinates (could be optimized to be even faster, but who cares)
	inline glm::vec2 GetProjection(const glm::vec3& pos) const { glm::vec4 v = viewProjMatrix_ * glm::vec4(pos, 1);  return (0.5f * glm::vec2(v.x, -v.y) / v.w + 0.5f) * resolution_; }

	inline std::string GetNextID() { if (myID.empty()) myID = "Camera " + std::to_string(++ID); return myID; }

	bool KeyboardDown(SDL_KeyboardEvent& key);
	bool KeyboardUp(SDL_KeyboardEvent& key);
	bool MouseMove(SDL_MouseMotionEvent& mouse);
	bool MouseWheel(SDL_MouseWheelEvent& wheel);

	inline Camera& Resize(glm::vec2 resolution) { resolution_ = resolution; proj_changed = true; return *this; }
	inline Camera& Resize(int width, int height) { return Resize(glm::vec2(width, height)); }
	inline Camera& Resize(float width, float height) { return Resize(glm::vec2(width, height)); }

private:
	bool proj_changed = true, view_changed = true, uv_changed = false, look_changed = true;

	float	moveSpeed_ = 3.0;
	bool	shiftHeld_ = false;

	glm::vec3	eye_ = { 5,5,5 }, up_ = { 0.f,1.f,0.f }, at_ = {0,0,0};

	float	u_, v_;
	float	eyeAtDistance_;
	float	nearDist_ = 0.1f, farDist_= 1000.f;

	glm::vec3	frwdDir_, rghtDir_;
	float	goFrwd_ = 0, goRght_ = 0;

	float fowDegs_	=45;			// in degrees
	float tanPixelFow_;	//tangent of a single pixel's cone's half opening angle
	glm::vec2 resolution_;

	glm::mat4 projMatrix_, viewMatrix_;
	glm::mat4 viewProjMatrix_, viewProjInverse_;

	bool isUiOpen_ = true;
	float deltaTime_;
	static int ID;
	std::string myID = "";
};
	
