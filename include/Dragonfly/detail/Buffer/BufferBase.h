#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <map>
#include <vector>
#include "../../config.h"

namespace df{

enum class BUFFER_BITS : GLenum
{
	NONE		= GL_NONE,						// Zero
	READ		= GL_MAP_READ_BIT,				// Only use this with df::Buffer
	WRITE		= GL_MAP_WRITE_BIT,				// Automatically set
	PERSISTENT	= GL_MAP_PERSISTENT_BIT,		// Cannot use with StorageBuffer
	COHERENT	= GL_MAP_COHERENT_BIT,			// Only if persistent
	CLIENT		= GL_CLIENT_STORAGE_BIT,		// Suggest storage in CPU RAM
	DYNAMIC		= GL_DYNAMIC_STORAGE_BIT,		// Please do not use
};  ENUM_CLASS_FLAG_OPERATORS(BUFFER_BITS)


namespace detail{

constexpr bool checkBufferFlag(BUFFER_BITS flags) {
	return  ((flags && (BUFFER_BITS::READ | BUFFER_BITS::WRITE)) || !(flags && BUFFER_BITS::PERSISTENT))
		&&  ((flags && BUFFER_BITS::PERSISTENT)					|| !(flags && BUFFER_BITS::COHERENT));
}


template<typename ...Types_>  struct EnableIfSingle{};
template<typename SingleType> struct EnableIfSingle<SingleType>{ using Type = SingleType;};

template<typename ...Types_>
using EnableIfSingle_Type = typename EnableIfSingle<Types_...>::Type;

class BufferLowLevelBase
{
private:
	static std::map<GLuint, unsigned int> _instances;
	GLuint _id;
	size_t _bytes;
	BUFFER_BITS _flags;
protected:
	BufferLowLevelBase(const BufferLowLevelBase& other_);
	BufferLowLevelBase(size_t bytes_, BUFFER_BITS flags_, void* data = nullptr);
	~BufferLowLevelBase();
public:
	size_t Bytes() const { return _bytes; }
	BUFFER_BITS Flags() const { return _flags; }
	operator GLuint() const { return _id; }
};

template<typename ...ItemTypes_>
class BufferBase : public BufferLowLevelBase
{
protected:
	using Base = BufferLowLevelBase;
	using Single_Type = EnableIfSingle_Type<ItemTypes_...>;
	using Tuple_Type = std::tuple<ItemTypes_...>;
	constexpr size_t _ElemSize = sizeof(Tuple_Type);
protected:
	BufferBase(const BufferLowLevelBase& base_) : Base::Base(base_) {}
	BufferBase(size_t len_, BUFFER_BITS flags_) : Base::Base(len_* _ElemSize, flags_) { }
	BufferBase(const std::vector<Single_Type>& vec_,	BUFFER_BITS flags_) : Base::Base(vec_.size() * _ElemSize, flags_, (void*)vec_.data()) {}
	BufferBase(std::initializer_list<Single_Type> vec_, BUFFER_BITS flags_) : Base::Base(vec_.size() * _ElemSize, flags_, (void*)vec_.begin()) {}
	BufferBase(const std::vector<Tuple_Type>& vec_,		BUFFER_BITS flags_) : Base::Base(vec_.size() * _ElemSize, flags_, (void*)vec_.data()) {}
	BufferBase(std::initializer_list<Tuple_Type> vec_,	BUFFER_BITS flags_) : Base::Base(vec_.size() * _ElemSize, flags_, (void*)vec_.begin()) {}
public:
	size_t Size() const { return _ElemSize * Bytes(); }
};

} //namespace detail
} //namespace df


/*

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
		GLuint _bufferId;
		size_t _bytes = 0;

		// This MUST be called in each constructor, otherwise the bufferInstance counter
		// won't be increased and ~Buffer will have undefined behavior.
		// void _initBuffer(int size, void* data);

		template<GLenum role> inline void Bind();
		template<GLenum role> inline void Bind(GLuint index);
		template<GLenum role> inline void Bind(GLuint index, GLintptr offset, GLsizeiptr size);

		BufferBase(GLuint bufferId_, size_t bytes_);

	public:
		BufferBase(int size, void* data = nullptr);
		BufferBase(const BufferBase&) = delete;
		BufferBase(BufferBase&&) noexcept;
		BufferBase& operator=(const BufferBase&) = delete;
		BufferBase& operator=(BufferBase&&) noexcept;
				
		operator GLuint() const { return _bufferId; }
		size_t Bytes() const { return _bytes; };
 
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
	glBindBuffer(Role_, _bufferId);
}

template<GLenum Role_>
inline void df::BufferBase::Bind(GLuint index)
{
	static_assert(df::detail::_isBufferIndexedRole<Role_>(), "This buffer Role_ cannot be bound to an indexed position");
	//TODO assert index https://www.khronos.org/opengl/wiki/Buffer_Object#Binding_indexed_targets
	glBindBufferBase(Role_, index, _bufferId);
}

template<GLenum Role_>
inline void df::BufferBase::Bind(GLuint index, GLintptr offset, GLsizeiptr size)
{
	static_assert(df::detail::_isBufferIndexedRole<Role_>(), "This buffer Role_ cannot be bound to an indexed position");
	//TODO assert index https://www.khronos.org/opengl/wiki/Buffer_Object#Binding_indexed_targets
	glBindBufferRange(Role_, index, _bufferId, offset, size);
}

*/