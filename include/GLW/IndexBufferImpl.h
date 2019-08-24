#pragma once

#include "IndexBufferBuilder.h"

namespace GLWRAPPER_NS {

	template<typename V>
	IndexBuffer IndexBuffer::FromVector(V& vector) {
		IndexBuffer buffer;
		IndexBufferBuilder<V::value_type> builder(buffer);

		builder.LoadData(vector);

		return buffer;
	}

}