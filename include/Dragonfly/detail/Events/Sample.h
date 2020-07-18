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
		INIT_RENDERDOC		= 1 << 0,		LAUNCH_RENDERDOC	= 1 << 1,
		RENDERDOC			= INIT_RENDERDOC | LAUNCH_RENDERDOC,
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

	template<typename F,typename E>
	static void callEventHandlers(std::multimap<int, F>& queue, E&& arg);
	static void callResizeHandlers(std::vector<Callback_Resize>& handlers, int w, int h);
	bool quit = false;
	Uint32 mainWindowID;
	SDL_Window *win = nullptr;
	SDL_GLContext context = nullptr;
public:
	Sample(const char* name = "OpenGL Sample", int width = 720, int height = 480, FLAGS flags = FLAGS::DEFAULT);
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
	template<typename C> void AddHandlerClass(int priority = 0);

	template<typename F> void Run(F&& RenderFunc); //lambda gets delta time in ms
};	// Sample class

ENUM_CLASS_FLAG_OPERATORS(Sample::FLAGS);

} // namespace df

#include "Sample.inl"