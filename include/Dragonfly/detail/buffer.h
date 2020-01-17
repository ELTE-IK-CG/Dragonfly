#pragma once
#include "object.h"
#include <GL/glew.h>

//namespace for opengl base classes
namespace eltecg { namespace ogl {
	
/****************************************************************************
 *						Enums												*/
enum class BufferType
{
	ARRAY_BUFFER				= GL_ARRAY_BUFFER				,
	ATOMIC_COUNTER_BUFFER		= GL_ATOMIC_COUNTER_BUFFER		,
	COPY_READ_BUFFER			= GL_COPY_READ_BUFFER			,
	COPY_WRITE_BUFFER			= GL_COPY_WRITE_BUFFER			,
	DRAW_INDIRECT_BUFFER		= GL_DRAW_INDIRECT_BUFFER		,
	DISPATCH_INDIRECT_BUFFER	= GL_DISPATCH_INDIRECT_BUFFER	,
	ELEMENT_ARRAY_BUFFER		= GL_ELEMENT_ARRAY_BUFFER		,
	PIXEL_PACK_BUFFER			= GL_PIXEL_PACK_BUFFER			,
	PIXEL_UNPACK_BUFFER			= GL_PIXEL_UNPACK_BUFFER		,
	QUERY_BUFFER				= GL_QUERY_BUFFER				,
	SHADER_STORAGE_BUFFER		= GL_SHADER_STORAGE_BUFFER		,
	TEXTURE_BUFFER				= GL_TEXTURE_BUFFER				,
	TRANSFORM_FEEDBACK_BUFFER	= GL_TRANSFORM_FEEDBACK_BUFFER	,
	UNIFORM_BUFFER				= GL_UNIFORM_BUFFER				,
};

enum class BufferFlags
{
	NONE				= 0,
	DYNAMIC_STORAGE_BIT = GL_DYNAMIC_STORAGE_BIT,
	MAP_READ_BIT        = GL_MAP_READ_BIT       ,
	MAP_WRITE_BIT       = GL_MAP_WRITE_BIT      ,
	MAP_PERSISTENT_BIT  = GL_MAP_PERSISTENT_BIT ,
	MAP_COHERENT_BIT    = GL_MAP_COHERENT_BIT   ,
	CLIENT_STORAGE_BIT  = GL_CLIENT_STORAGE_BIT ,
};
ENUM_CLASS_FLAG_OPERATORS(BufferFlags)

/****************************************************************************
 *						OpenGL Buffer base class							*
 ****************************************************************************/
//https://www.khronos.org/opengl/wiki/Buffer_Object
template<BufferType T_buffer_type>
	class Buffer final
		: public base::Object
{


public:
/****************************************************************************
 *						Constructors and destructors						*/
	Buffer();
	~Buffer();
	//todo tidy

	Buffer(const Buffer&) = delete;
	Buffer& operator=(const Buffer&) = delete;

	Buffer(Buffer&& o) : base::Object(std::move(o)), m_buffer_size(o.m_buffer_size) {}
	Buffer& operator=(Buffer&& o) {
		if (this == &o) return *this;
		base::Object::operator=(std::move(o));
		m_buffer_size = o.m_buffer_size;
		return *this;
	}

	constexpr BufferType Type() const { return T_buffer_type; }
	constexpr GLenum GLtype() const { return static_cast<GLenum>(T_buffer_type); }
	
/****************************************************************************
 *						Operators											*/
	
	//TODO: check code and do operator = and (explicit!) constructors for these
	template<typename Container>
	void constructImmutable(const Container &container, BufferFlags flags = BufferFlags::NONE)
	{
		//TODO assert valid flags (attention!)
		bindBuffer();
		this->m_buffer_size = container.size()*sizeof(Container::value_type);
		glBufferStorage(GLtype(), this->m_buffer_size,
			(GLvoid*) container.data(), static_cast<GLbitfield>(flags));
	}

	template<typename Container>
	void constructMutable(const Container &container, GLenum usage) //TODO: make 'usage' easier
	{
		bindBuffer();
		this->m_buffer_size = container.size()*sizeof(Container::value_type);
		glBufferData(this->GLtype(), this->m_buffer_size , (GLvoid*) container.data(), usage);
	}

	template<typename Container>	//todo make it work with c array, std::array and std::vector
	void assignMutable(const Container &container, size_t offset = 0)
	{
		bindBuffer();
		size_t to_write = container.size() * sizeof(Container::value_type);
		ASSERT(offset + to_write < this->m_buffer_size, "Container to be assigned is larger then it should be!");
		glBufferSubData(GLtype(), offset, to_write, (GLvoid*) container.data());
	}
	
/****************************************************************************
 *						Binding												*/

	inline void bindBuffer()
	{	
		if(this->s_bound_buffer_id() != this->object_id)
		{
			glBindBuffer(GLtype(), this->object_id);
			this->s_bound_buffer_id() = this->object_id;
		}
	}
	
	inline void bindBufferRange(GLuint index, GLintptr offset = 0, GLintptr size = 0) //TODO: Smarthen by binding to opengl program/pipeline object
	{
		static_assert( T_buffer_type == BufferType::TRANSFORM_FEEDBACK_BUFFER
			|| T_buffer_type == BufferType::UNIFORM_BUFFER
			|| T_buffer_type == BufferType::ATOMIC_COUNTER_BUFFER
			|| T_buffer_type == BufferType::SHADER_STORAGE_BUFFER, "Invalid buffer type for binding buffer range.");
		glBindBufferRange(GLtype(), index, this->object_id, offset, size == 0 ? this->m_buffer_size: size);
	}
	//TODO: Multibind with glBindBuffersRange (note 's' in Buffers).
	
protected:

/****************************************************************************
 *						Variables											*/
	static GLuint& s_bound_buffer_id() { static GLuint id = 0; return id; }

	GLsizeiptr m_buffer_size = 0;

#undef BUFFER_BIND_ASSERT

}; // Buffer class
	
template<BufferType T_buffer_type>
inline Buffer<T_buffer_type>::Buffer()
{
	static_assert(T_buffer_type == BufferType::ARRAY_BUFFER
		|| T_buffer_type == BufferType::ATOMIC_COUNTER_BUFFER
		|| T_buffer_type == BufferType::COPY_READ_BUFFER
		|| T_buffer_type == BufferType::COPY_WRITE_BUFFER
		|| T_buffer_type == BufferType::DRAW_INDIRECT_BUFFER
		|| T_buffer_type == BufferType::DISPATCH_INDIRECT_BUFFER
		|| T_buffer_type == BufferType::ELEMENT_ARRAY_BUFFER
		|| T_buffer_type == BufferType::PIXEL_PACK_BUFFER
		|| T_buffer_type == BufferType::PIXEL_UNPACK_BUFFER
		|| T_buffer_type == BufferType::QUERY_BUFFER
		|| T_buffer_type == BufferType::SHADER_STORAGE_BUFFER
		|| T_buffer_type == BufferType::TEXTURE_BUFFER
		|| T_buffer_type == BufferType::TRANSFORM_FEEDBACK_BUFFER
		|| T_buffer_type == BufferType::UNIFORM_BUFFER, "Invalid buffer type.");

	glGenBuffers(1, &this->object_id);
}
template<BufferType T_buffer_type>
inline Buffer<T_buffer_type>::~Buffer()
{
	if(s_bound_buffer_id() == this->object_id) s_bound_buffer_id() = 0;
	glDeleteBuffers(1, &this->object_id);
}

using ArrayBuffer = Buffer<BufferType::ARRAY_BUFFER>;
using ElementArrayBuffer = Buffer<BufferType::ELEMENT_ARRAY_BUFFER>;
using UniformBuffer = Buffer<BufferType::UNIFORM_BUFFER>;
using ShaderStorageBuffer = Buffer<BufferType::SHADER_STORAGE_BUFFER>;

}} //namespace eltecg::ogl