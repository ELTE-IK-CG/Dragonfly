#pragma once

#include "VertexBufferBuilder.h"

namespace GLWRAPPER_NS {

	template<typename V>
	VertexBuffer VertexBuffer::FromVector(V& vector) {
		VertexBuffer buffer;
		VertexBufferBuilder<V::value_type> builder(buffer);

		builder.LoadData(vector);

		return buffer;
	}

}