#pragma once
#include "Program.h"
#include "UniformTypes.hpp"
#include "Texture.h"

#include <unordered_map>
#include <glm/glm.hpp>
#include <map>

class Uniforms : public UniformLowLevelBase {
	friend class ProgramBase<Uniforms>;
	struct Values {
		uint16_t loc; //minimum required uniform locations are 1024, much less then 2^16
#ifdef _DEBUG
	private:
		uint16_t _dummy_for_alignment;
	public:
		GLenum gpu_type;		//type information from opengl
		size_t cpu_type = 0;	//type information from typeinfo
		GLint size;
#endif // _DEBUG
	};

	template<typename ValType>
	struct _UniformSetterHelper
	{
		void call(const Uniforms& u, const std::string& str, const Uniforms::Values& vals, const ValType& val)
		{
			ASSERT(getOpenGLType<ValType>() == vals.gpu_type, ("The uniform \"" + str + "\" of type \"" + typeid(ValType).name() + "\" had a different type in the shader.").c_str());
			ASSERT(u.texLoc2sampler.find(vals.loc) == u.texLoc2sampler.end(), ("The uniform \"" + str + "\" of type \"" + typeid(ValType).name() + "\" is supposed to be a texture.").c_str());
			u.SetUni(vals.loc, val); // Regular uniforms
		}
	};

	template<typename InternalFormat, TextureType TextureType>
	struct _UniformSetterHelper < Texture<InternalFormat, TextureType>>
	{
		inline void call(const Uniforms& u, const std::string& str, const Uniforms::Values& vals, const Texture<InternalFormat, TextureType>& val)
		{
			auto it = u.texLoc2sampler.find(vals.loc);
			ASSERT(it != u.texLoc2sampler.end(), ("Texture sampler \"" + str + "\" not found of type \"Texture<" + typeid(InternalFormat).name() + ">\". This error should not occur.").c_str());
			//TODO ASSERT TYPE CHECK
			val.bind(it->second);
			glUniform1i(vals.loc, static_cast<GLint>(it->second)); //same as SetUni
		}
	};

protected:
	GLuint program_id = 0;
	std::unordered_map<std::string, Values> locations;
	std::unordered_map<uint16_t, uint8_t> texLoc2sampler;
	std::vector<uint16_t> sampler2texLoc;
	Uniforms(GLuint program_id) : program_id(program_id) {}
	GLuint GetUniformLocation(const std::string& str) const;
public:
	Uniforms() = delete;
	template<typename ValType>
	inline void SetUniform(std::string&& str, ValType&& val);
	//Do this on shader program compilation.
	bool Compile();

	//Does absolutely nothing. For UI use UniformEditor
	inline void Render(const std::string& program_name = "") {}
};


using GreedyUniforms [[deprecated("Use the \"Uniforms\" class instead.")]] = Uniforms;

#include "Uniforms.inl"
