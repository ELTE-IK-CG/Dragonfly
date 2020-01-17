#pragma once
#include "object.h"
#include "buffer.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <iostream>
#include <typeinfo>
#include "Traits/UniformTypes.hpp" //glm -> ogl type conversion ... TODO refactor

//namespace for opengl classes
namespace eltecg { namespace ogl {

/****************************************************************************
 *						Type constructors									*/

template<typename T_type>
struct dummy_t{T_type dummy; using type = T_type;};

template<typename T_integral> //TODO: check valid types!
struct integral_t{T_integral dummy; using type = T_integral;};

/****************************************************************************
 *						OpenGL Vertex Array Object 							*
 ****************************************************************************/

 //https://www.khronos.org/opengl/wiki/Vertex_Specification
class VertexArray final
	: public base::Object
{
public:

	VertexArray(){ glGenVertexArrays(1, &this->object_id); }
	~VertexArray() { if (s_bound_vao_id() == this->object_id) s_bound_vao_id() = 0; glDeleteVertexArrays(1, &this->object_id); }

	inline void bindVertexArray();

	template<typename ... T_vertex_types>
	void addVBO(ArrayBuffer& vertex_buffer_object);

protected:

	template<GLsizei stride, GLsizei index, GLsizei offset,
		typename Type_head, typename ... Type_tail>
	void addVBOrec();

	template<GLsizei stride, GLsizei index, GLsizei offset>
	void addVBOrec();
	
protected:
	static GLuint& s_bound_vao_id() { static GLuint id = 0; return id; }
	GLuint _curr_attrib_idx = 0;

}; //VertexArray

inline void VertexArray::bindVertexArray()
{
	ASSERT(this->object_id != 0, "VAO id cannot be zero.");
	if (s_bound_vao_id() != this->object_id)
	{
		glBindVertexArray(this->object_id);
		s_bound_vao_id() = this->object_id;
	}
}

template<typename ...T_vertex_types>
inline void VertexArray::addVBO(ArrayBuffer& vertex_buffer_object)
{
	vertex_buffer_object.bindBuffer();
	this->bindVertexArray();
	this->addVBOrec< (0 + ... + sizeof(T_vertex_types)),
					0, 0,
					T_vertex_types ...>();
}

template<typename D> struct is_dummy_t : std::false_type{};
template<typename T> struct is_dummy_t<dummy_t<T>> : std::true_type{};
template<typename D> constexpr bool is_dummy_t_v = is_dummy_t< std::decay_t<D>>::value;

template<typename D> struct is_integral_t : std::false_type {};
template<typename T> struct is_integral_t<integral_t<T>> : std::true_type {};
template<typename D> constexpr bool is_integral_t_v = is_integral_t< std::decay_t<D>>::value;

template <typename T> struct as_array_type { using type = T; };
template <typename T> struct as_array_type<glm::vec<1, T>> { using type = T[1]; };
template <typename T> struct as_array_type<glm::vec<2, T>> { using type = T[2]; };
template <typename T> struct as_array_type<glm::vec<3, T>> { using type = T[3]; };
template <typename T> struct as_array_type<glm::vec<4, T>> { using type = T[4]; };
template <typename T> using as_array_type_t = typename as_array_type<std::decay_t<T>>::type;

template<GLsizei stride, GLsizei index, GLsizei offset,
	typename Type_head, typename ... Type_tail>
inline void VertexArray::addVBOrec()
{
	if constexpr (!is_dummy_t_v<Type_head>)
	{
		glEnableVertexAttribArray(_curr_attrib_idx+index);
		GL_CHECK;

		using attib_t = as_array_type_t<Type_head>;
		using base_t = std::remove_pointer_t<std::remove_all_extents_t<attib_t>>;

		constexpr GLenum	ogl_base_t = df::getOpenGLType<base_t>();
		constexpr GLboolean integral = is_integral_t_v<Type_head>;
		constexpr GLint		components = std::extent_v<attib_t> > std::extent_v<Type_head> ? std::extent_v<attib_t> : std::extent_v<Type_head>;

		std::cout << typeid(Type_head).name() << ": " << "index = " << _curr_attrib_idx + index
			<< "\n\t"<< typeid(base_t).name() << '[' << components << ']'
			<< " size=" << sizeof(Type_head) << " stride=" << stride << " offset=" << offset << std::endl;

		if constexpr (std::is_same_v < base_t, double>)
		{	// double only
			static_assert(ogl_base_t == GL_DOUBLE, "This should be double!");
			glVertexAttribLPointer(_curr_attrib_idx + index, components, ogl_base_t,
				stride, reinterpret_cast<void*>(offset));
			GL_CHECK;
		}
		else if constexpr (integral && std::is_integral_v<base_t>)
		{	
			glVertexAttribIPointer(_curr_attrib_idx + index, components, ogl_base_t,
				stride, reinterpret_cast<void*>(offset));
			GL_CHECK;
		}
		else 
		{	
			glVertexAttribPointer(_curr_attrib_idx + index, components, ogl_base_t,
				true,	stride, reinterpret_cast<void*>(offset));
			GL_CHECK;
		}
		addVBOrec<stride, index + 1, offset + sizeof(Type_head), Type_tail...>();
	}
	else
	{
		addVBOrec<stride, index, offset + sizeof(Type_head), Type_tail...>();
	}
}

template<GLsizei stride, GLsizei index, GLsizei offset>
inline void VertexArray::addVBOrec()
{
	this->_curr_attrib_idx += index; // finally we increment the runtime counter
}


}} //namespace eltecg::ogl