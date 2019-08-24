#pragma once

#include "Builder.h"

namespace GLWRAPPER_NS {

	template<typename T>
	class ResourceBuilder: public Builder {
	protected:

		T & object;

		ResourceBuilder(T& object) :
			Builder(), object(object)
		{}
		
	};

}