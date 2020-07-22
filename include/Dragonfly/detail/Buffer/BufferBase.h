#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <map>
#include <vector>
#include "../../config.h"

namespace df
{
	static std::map<GLuint, int> bufferInstances;

	namespace detail {
		template<typename NewType>
		struct ConvertVecStore {
			std::vector<NewType> vec;
		};
		template<typename NewType>
		struct ConvertVecRef {
			std::vector<NewType>& vec;
		};
	}

	template<typename NewType>
	detail::ConvertVecStore<NewType> Convert(std::vector<NewType>&& vec) {
		return ConvertVec(std::move(vec));
	}

	template<typename NewType>
	detail::ConvertVecRef<NewType> Convert(std::vector<NewType>& vec) {
		return ConvertVecRef(vec);
	}

	class BufferBase
	{
	protected:
		GLuint buffer_id = 0;
		size_t size = 0;

		// This MUST be called in each constructor, otherwise the bufferInstance counter
		// won't be increased and ~Buffer will have undefined behavior.
		// void _initBuffer(int size, void* data);

		template<GLenum role> inline void Bind();
		template<GLenum role> inline void Bind(GLuint index);
		template<GLenum role> inline void Bind(GLuint index, GLintptr offset, GLsizeiptr size);

	public:
		BufferBase(int size, void* data = nullptr);
		BufferBase(const BufferBase&) = delete;
		BufferBase(BufferBase&&) noexcept;
		BufferBase& operator=(const BufferBase&) = delete;
		BufferBase& operator=(BufferBase&&);

		~BufferBase();

	};

	namespace detail
	{
		template<GLenum Role_> constexpr bool _isBufferRole();
		template<GLenum Role_> constexpr bool _isBufferIndexedRole();
	} //namespace detail

} // namespace df


template<GLenum Role_>
constexpr bool df::detail::_isBufferRole()
{
	return	Role_ == GL_ARRAY_BUFFER
		||	Role_ == GL_COPY_WRITE_BUFFER
		||	Role_ == GL_DRAW_INDIRECT_BUFFER
		||	Role_ == GL_ELEMENT_ARRAY_BUFFER
		||	Role_ == GL_PIXEL_PACK_BUFFER
		||	Role_ == GL_PIXEL_UNPACK_BUFFER
		||	Role_ == GL_TRANSFORM_FEEDBACK_BUFFER
#if OPENGL_VERSION >= 31
		||	Role_ == GL_TEXTURE_BUFFER
		||	Role_ == GL_UNIFORM_BUFFER
		||	Role_ == GL_COPY_READ_BUFFER
#endif
#if OPENGL_VERSION >= 42
		||	Role_ == GL_ATOMIC_COUNTER_BUFFER
#endif
#if OPENGL_VERSION >= 43
		||	Role_ == GL_SHADER_STORAGE_BUFFER
		||	Role_ == GL_DISPATCH_INDIRECT_BUFFER
#endif
#if OPENGL_VERSION >= 44
		||	Role_ == GL_QUERY_BUFFER
#endif
		;
}

template<GLenum Role_>
constexpr bool df::detail::_isBufferIndexedRole()
{
	return false
#if OPENGL_VERSION >= 31
		|| Role_ == GL_UNIFORM_BUFFER
#endif
#if OPENGL_VERSION >= 42
		|| Role_ == GL_ATOMIC_COUNTER_BUFFER
#endif
#if OPENGL_VERSION >= 43
		|| Role_ == GL_SHADER_STORAGE_BUFFER
#endif
		;
}


template<GLenum Role_>
inline void df::BufferBase::Bind()
{
	static_assert(df::detail::_isBufferRole<Role_>(), "Role_ is not a valid OpenGL Buffer Type");
	glBindBuffer(Role_, buffer_id);
}

template<GLenum Role_>
inline void df::BufferBase::Bind(GLuint index)
{
	static_assert(df::detail::_isBufferIndexedRole<Role_>(), "This buffer Role_ cannot be bound to an indexed position");
	//TODO assert index https://www.khronos.org/opengl/wiki/Buffer_Object#Binding_indexed_targets
	glBindBufferBase(Role_, index, buffer_id);
}

template<GLenum Role_>
inline void df::BufferBase::Bind(GLuint index, GLintptr offset, GLsizeiptr size)
{
	static_assert(df::detail::_isBufferIndexedRole<Role_>(), "This buffer Role_ cannot be bound to an indexed position");
	//TODO assert index https://www.khronos.org/opengl/wiki/Buffer_Object#Binding_indexed_targets
	glBindBufferRange(Role_, index, buffer_id);
}