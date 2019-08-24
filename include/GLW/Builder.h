#pragma once

#include "common.h"

namespace GLWRAPPER_NS {

	// Todo: make builder moveable
	class Builder {

		bool built = false;

	protected:

		Builder() {}

	public:

		// Disable copy
		Builder(const Builder&) = delete;

		virtual ~Builder() {
			// Should be built
			assert(built);
		}

		virtual void Build() {
			// Don't build twice
			assert(!built);

			built = true;
		}

	};

}