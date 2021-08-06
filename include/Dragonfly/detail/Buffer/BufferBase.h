#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <map>
#include <vector>
#include <tuple>
#include "../../config.h"
#include "../Traits/Tuple.h"

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
	ALL = NONE | READ | WRITE | PERSISTENT | COHERENT | CLIENT | DYNAMIC,
};  ENUM_CLASS_FLAG_OPERATORS(BUFFER_BITS)

} // namespace df;

namespace df::detail{

template<BUFFER_BITS flags_> constexpr bool checkBufferFlag() {
	return  ((flags_ && (BUFFER_BITS::READ | BUFFER_BITS::WRITE)) || !(flags_ && BUFFER_BITS::PERSISTENT))
		&&  ((flags_ && BUFFER_BITS::PERSISTENT)				  || !(flags_ && BUFFER_BITS::COHERENT));
}

inline bool checkBufferFlag(BUFFER_BITS flags_) {
	return  ((flags_ && (BUFFER_BITS::READ | BUFFER_BITS::WRITE)) || !(flags_ && BUFFER_BITS::PERSISTENT))
		&&  ((flags_ && BUFFER_BITS::PERSISTENT)				  || !(flags_ && BUFFER_BITS::COHERENT));
}

class MappedBufferBase;

class BufferLowLevelBase
{
private:
	inline static std::map<GLuint, unsigned int> _instances = std::map<GLuint, unsigned int>();
	GLuint _id;
	size_t _bytes;
	BUFFER_BITS _flags;
public:
	BufferLowLevelBase(const BufferLowLevelBase& other_); // modifies _instances
	~BufferLowLevelBase();
protected:
	BufferLowLevelBase(size_t bytes_, BUFFER_BITS flags_, void* data_ = nullptr);
	BufferLowLevelBase(BufferLowLevelBase&&other_) noexcept;
	BufferLowLevelBase operator=(const BufferLowLevelBase& other_) = delete;
	df::detail::BufferLowLevelBase& operator=(BufferLowLevelBase&& other_) noexcept;

	void _UploadData(const void* data_, size_t size_);
	
	template<typename ItemType_> [[nodiscard]] std::vector<ItemType_> _DownloadData() const;

public:
	[[nodiscard]] size_t Bytes() const { return _bytes; }
	[[nodiscard]] BUFFER_BITS Flags() const { return _flags; }
	[[nodiscard]] int GetID() const { return _id; }
	[[nodiscard]] explicit operator GLuint() const { return _id; }
	friend class MappedBufferBase;
};

template<typename ... ItemTypes_>
class BufferBase : public BufferLowLevelBase
{
protected:
	using Base			= BufferLowLevelBase;
	static constexpr size_t _NumItemTypes	= sizeof...(ItemTypes_);
	using Tuple_Type	= std::tuple<ItemTypes_...>;
	using Single_Type	= std::conditional_t<_NumItemTypes == 1, std::tuple_element_t<0, Tuple_Type>, void**>;
	using Pair_Type		= std::conditional_t<_NumItemTypes == 2, std::pair<std::tuple_element_t<0, Tuple_Type>, std::tuple_element_t<_NumItemTypes-1, Tuple_Type>>, void***>;
	static constexpr size_t	_elemSize		= sizeof(Tuple_Type);
protected:
	explicit BufferBase(BufferLowLevelBase base_) : Base::BufferLowLevelBase(std::move(base_)) {}
	BufferBase(size_t len_, BUFFER_BITS flags_) : Base::BufferLowLevelBase(len_ * _elemSize, flags_) { }
	BufferBase(const std::vector	<Single_Type> &vec_, BUFFER_BITS flags_) : Base::BufferLowLevelBase(vec_.size() * _elemSize, flags_, (void*)vec_.data()) {static_assert(_NumItemTypes==1, "df::detail::BufferBase: Use tuple or pair in Buffer* constructors for compound types.");}
	BufferBase(const std::vector	<Pair_Type>   &vec_, BUFFER_BITS flags_) : Base::BufferLowLevelBase(vec_.size() * _elemSize, flags_, (void*)vec_.data()) {static_assert(_NumItemTypes==2, "df::detail::BufferBase: Use tuple in Buffer* constructors for compound types.");}
	BufferBase(const std::vector	<Tuple_Type>  &vec_, BUFFER_BITS flags_) : Base::BufferLowLevelBase(vec_.size() * _elemSize, flags_, (void*)vec_.data()) {}
	BufferBase(std::initializer_list<Single_Type>  vec_, BUFFER_BITS flags_) : Base::BufferLowLevelBase(vec_.size() * _elemSize, flags_, (void*)vec_.begin()) {static_assert(_NumItemTypes==1, "df::detail::BufferBase: Use tuple or pair in Buffer* constructors for compound types.");}
	BufferBase(std::initializer_list<Tuple_Type>   vec_, BUFFER_BITS flags_) : Base::BufferLowLevelBase(vec_.size() * _elemSize, flags_, (void*)vec_.begin()) {} // no Pair_Type to avoid ambiguity

public:
	[[nodiscard]] size_t Size() const { return _elemSize * Bytes(); }
};

template <typename ItemType_> std::vector<ItemType_> BufferLowLevelBase::_DownloadData() const
{
	ASSERT(this->Flags() && BUFFER_BITS::READ, "df::detail::BufferLowLevelBase: Buffer does not have the read flag set.");
	void* cim = glMapNamedBuffer(static_cast<GLuint>(*this), GL_MAP_READ_BIT);
	const ItemType_* begin = static_cast<const ItemType_*>(cim);
	ASSERT(begin != nullptr, "df::detail::BufferLowLevelBase: Mapping operation failed.");
	const std::vector<ItemType_> ret(begin, begin + this->Bytes() / sizeof(ItemType_));
	GL_CHECK;
	glUnmapNamedBuffer(static_cast<GLuint>(*this));
	return ret;
}

} //namespace df::detail


/*

namespace df
{
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