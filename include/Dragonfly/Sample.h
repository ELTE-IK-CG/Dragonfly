#pragma once
#include <functional>
#include <algorithm>
#include <map>
#include <SDL/SDL.h>

class Sample
{
protected:
	using Callback_KeyBoardEvent	= std::function<bool(const SDL_KeyboardEvent &)>;
	using Callback_MouseButtonEvent = std::function<bool(const SDL_MouseButtonEvent&)>;
	using Callback_MouseMotionEvent = std::function<bool(const SDL_MouseMotionEvent&)>;
	using Callback_ResizeEvent = std::function<bool(const SDL_WindowEvent&)>;

	std::multimap<int, Callback_KeyBoardEvent>		_keydown, _keyup;
	std::multimap<int, Callback_MouseButtonEvent>	_mousedown, _mouseup;
	std::multimap<int, Callback_MouseMotionEvent>	_mousemove;
	std::multimap<int, Callback_ResizeEvent>		_resize;

	template<typename F,typename E>
	inline void callEventHandlers(std::multimap<int, F>& queue, E&&arg)
	{
		bool wastrue = false;	int priority;
		for (auto& pF : queue)
		{
			if (wastrue && pF.first > priority) break;
			wastrue |= pF.second(arg);
			priority = pF.first;
		}
	}
	bool quit = false;
	Uint32 mainWindowID;
public:
	inline void Quit() { quit = true; }


	template<typename F>
	void AddKeyDownEventHandler(F&& f, int priority = 0){_keydown.emplace(-priority, Callback_KeyEvent(f));	}
	template<typename F>
	void AddKeyUpEventHandler(F&& f, int priority = 0){	_keyup.emplace(-priority, Callback_KeyEvent(f));	}

	template<typename F>
	void AddMouseDownEventHandler(F&& f, int priority = 0) { _mousedown.emplace(-priority, Callback_MouseButtonEvent(f)); }
	template<typename F>
	void AddMouseUpEventHandler(F&& f, int priority = 0) { _mouseup.emplace(-priority, Callback_MouseButtonEvent(f)); }
	template<typename F>
	void AddMouseMotionEventHandler(F&& f, int priority = 0) { _mousemove.emplace(-priority, Callback_MouseMotionEvent(f)); }


	template<typename F>
	inline void Run(F&& RenderFunc)
	{
		quit = false;
		while (!quit)
		{
			while (SDL_PollEvent(&ev))
			{
				switch (ev.type)
				{
				case SDL_UP:				callEventHandlers(_keyup, ev.key);				break;
				case SDL_KEYDOWN:			callEventHandlers(_keydown, ev.key);			break;
				case SDL_MOUSEBUTTONDOWN:	callEventHandlers(_mouseup, ev.mouse);			break;
				case SDL_MOUSEBUTTONUP:		callEventHandlers(_mousedown, ev.mouse);		break;
				case SDL_MOUSEBUTTONUP:		callEventHandlers(_mousemove, ev.motion);		break;
				case SDL_MOUSEBUTTONUP:		callEventHandlers(_resize, ev.motion);			break;
				case SDL_WINDOWEVENT:
					if (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED && window_id == ev.window.windowID)
					{
						callEventHandlers(_resize, ev.window);
					}
					else if (ev.window.event == SDL_WINDOWEVENT_CLOSE && ev.window.windowID == mainWindowID)
					{
						Quit();
					}
					break;
				case SDL_QUIT:	Quit();	break;
				default: break;
				}
			}
			//todo delta time
			RenderFunc();
		}
	}
};