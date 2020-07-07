#include "renderdoc_load_api.h"

#include <windows.h>
#include <iostream>

namespace rdoc {
	RENDERDOC_API_1_1_2* API = NULL;

	// Implementation based on https://renderdoc.org/docs/in_application_api.html
	bool initRenderDocAPI(bool launchUI)
	{
		if (HMODULE mod = LoadLibrary(TEXT("renderdoc.dll")))
		{
			pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
			int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void**)&API);
			if (ret == 1) {
				// API loaded successfully, RenderDoc is connected
				std::cout << "[RenderDoc] API loaded" << std::endl;
				int major, minor, patch;
				API->GetAPIVersion(&major, &minor, &patch);
				std::cout << "[RenderDoc] API version: " << major << "." << minor << "." << patch << std::endl;
				if(launchUI && !API->IsTargetControlConnected())
					API->LaunchReplayUI(1, nullptr);
			}
			else {
				// Renderdoc is loaded but the API can't be accessed, this shouldn't normally happen
				std::cout << "[RenderDoc] API couldn't be loaded" << std::endl;
				FreeLibrary(mod);
				API = NULL;
			}
		}
		else {
			// Couldn't load renderdoc.dll
			std::cout << "[RenderDoc] not connected" << std::endl;
		}
		return bool(API);
	}
}
