#pragma once
#include <map>
#include <vector>
#include <functional>
#include <SDL/SDL.h>
#include "../../config.h"

namespace df {

class Sample
{
public:
	using Callback_KeyBoard		= std::function<bool(const SDL_KeyboardEvent &)>;
	using Callback_MouseButton	= std::function<bool(const SDL_MouseButtonEvent&)>;
	using Callback_MouseMotion	= std::function<bool(const SDL_MouseMotionEvent&)>;
	using Callback_MouseWheel	= std::function<bool(const SDL_MouseWheelEvent&)>;
	using Callback_Resize		= std::function<void(int, int)>;

	enum class FLAGS : unsigned int
	{
		NONE				= 0,
		INIT_RENDERDOC		= 1 << 0,		RUN_RENDERDOC_ON_CAPTURE	= 1 << 1,
		RENDERDOC			= INIT_RENDERDOC | RUN_RENDERDOC_ON_CAPTURE,
		V_SYNC				= 1 << 2,		V_SYNC_ADAPTIVE		= 1 << 3,
		IMGUI_DOCKING		= 1 << 4,		IMGUI_VIEWPORTS		= 1 << 5,
		WINDOW_RESIZABLE	= 1 << 6,		WINDOW_BORDERLESS	= 1 << 7,
		WINDOW_FULLSCREEN	= 1 << 8,
		DEFAULT				= V_SYNC_ADAPTIVE | IMGUI_DOCKING | IMGUI_VIEWPORTS | WINDOW_RESIZABLE | (df::IS_THIS_DEBUG ? RENDERDOC : NONE)
	};

protected:
	std::multimap<int, Callback_KeyBoard>		_keydown,	_keyup;
	std::multimap<int, Callback_MouseButton>	_mousedown,	_mouseup;
	std::multimap<int, Callback_MouseMotion>	_mousemove;
	std::multimap<int, Callback_MouseWheel>		_mousewheel;
	std::vector<Callback_Resize>				_resize;

	template<typename F_,typename E_>
	static void _CallEventHandlers(std::multimap<int, F_>& queue_, E_&& arg_);
	static void _CallResizeHandlers(std::vector<Callback_Resize>& handlers_, int w_, int h_);
	bool _quit = false;
	Uint32 _mainWindowID;
	SDL_Window *_mainWindowPtr = nullptr;
	SDL_GLContext _mainWindowContext = nullptr;
public:
	Sample(const char* name_ = "OpenGL Sample", int width_ = 720, int height_ = 480, FLAGS flags_ = FLAGS::DEFAULT);
	~Sample();

	void Quit() { _quit = true; }
	
	Sample& AddKeyDown    (Callback_KeyBoard&& f_,    int priority_ = 0) {    _keydown.emplace(-priority_, std::move(f_)); return *this; }
	Sample& AddKeyUp      (Callback_KeyBoard&& f_,    int priority_ = 0) {      _keyup.emplace(-priority_, std::move(f_)); return *this; }
	Sample& AddMouseDown  (Callback_MouseButton&& f_, int priority_ = 0) {  _mousedown.emplace(-priority_, std::move(f_)); return *this; }
	Sample& AddMouseUp    (Callback_MouseButton&& f_, int priority_ = 0) {    _mouseup.emplace(-priority_, std::move(f_)); return *this; }
	Sample& AddMouseMotion(Callback_MouseMotion&& f_, int priority_ = 0) {  _mousemove.emplace(-priority_, std::move(f_)); return *this; }
	Sample& AddMouseWheel (Callback_MouseWheel&& f_,  int priority_ = 0) { _mousewheel.emplace(-priority_, std::move(f_)); return *this; }
	Sample& AddResize     (Callback_Resize&& f_) { _resize.emplace_back(std::move(f_)); return *this; }

	// Sample::AddHandlerClass() adds the following handler functions from the handler class
	//	(if the given class has the function with the given name and signature):
	// bool HandleKeyUp(const SDL_KeyboardEvent&)
	// bool HandleKeyDown(const SDL_KeyboardEvent&)
	// bool HandleMouseUp(const SDL_MouseButtonEvent&)
	// bool HandleMouseDown(const SDL_MouseButtonEvent&)
	// bool HandleMouseMotion(const SDL_MouseMotionEvent&)
	// bool HandleMouseWheel(const SDL_MouseWheelEvent&)
	// void HandleResize(int, int)
	template<typename C_> void AddHandlerClass(C_& c_, int priority_ = 0);
	template<typename C_> void AddHandlerClass(int priority_ = 0);

	//Sample::Run(RenderFunc_) starts the render loop.
	// - RenderFunc_ is any callable object that recieves the frame delta time in ms
	// - Best to use it with an in-line C++ lambda object:
	//	df::Sample sam("Sample Demo", 720, 480); // [...]
	//	sam.Run([&](float delta_time_) {
	//			//Rendering commands to draw your frame goes here...
	//		}); // end of the Sample::Run() function call.
	template<typename F_> void Run(F_&& RenderFunc_); //lambda gets delta time in ms

};	// Sample class

} // namespace df

#include "Sample.inl"