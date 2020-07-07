#pragma once

#include "renderdoc_app.h"

namespace rdoc {
	extern RENDERDOC_API_1_1_2* API;

	// call this before any graphics initializaton
	// PAR launchUI: launch RenderDoc UI connected to this application
	// RETURNs true if RenderDoc is connected and sets the API variable above
	bool initRenderDocAPI(bool launchUI = false);
}
