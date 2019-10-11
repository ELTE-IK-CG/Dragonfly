#pragma once
#include "Program.h"
#include "UniformTypes.hpp"

#include <unordered_map>
#include <glm/glm.hpp>
#include <map>

class LazyUniforms : public UniformLowLevelBase {
	friend class ProgramBase<LazyUniforms>;
protected:
	GLuint program_id = 0;
	mutable std::map<std::string, GLuint> locations;
#ifdef GPU_DEBUG
	std::map<GLuint, GLenum> debug_types; //only use it in gpu_debug mode
#endif

	LazyUniforms(GLuint program_id) : program_id(program_id) {}
	GLuint GetUniformLocation(const std::string& str) const;

public:
	LazyUniforms() = delete;
	template<typename ValType>
	inline void SetUniform(std::string &&str, ValType &&val);
	inline bool Compile() { return true; }
};

class GreedyUniforms : public UniformLowLevelBase {
	friend class ProgramBase<GreedyUniforms>;
protected:
	struct Values {
		GLuint loc;
#ifdef _DEBUG
		size_t cpu_type = 0;	//type information from typeinfo
		GLenum gpu_type;	//type information from opengl
		GLint size;
#endif // _DEBUG
	};
	GLuint program_id = 0;
	std::unordered_map<std::string, Values> locations;
	GreedyUniforms(GLuint program_id) : program_id(program_id) {}
	GLuint GetUniformLocation(const std::string& str) const;
public:
	GreedyUniforms() = delete;
	template<typename ValType>
	inline void SetUniform(std::string&& str, ValType&& val);
	//Do this on shader program compilation.
	bool Compile();
};

class CachedUniforms : public GreedyUniforms {
	using Base = GreedyUniforms;
	friend class ProgramBase<CachedUniforms>;
protected:
	using UniData = decltype(UniformLowLevelBase::get_types_as_variant(valid_types{}));

	std::map<GLuint, UniData> cache;
	CachedUniforms(GLuint program_id) : Base(program_id) {}
public:
	CachedUniforms() = delete;
	template<typename ValType>
	void SetUniform(std::string &&str, const ValType &val);

};

#include "Uniforms.inl"
