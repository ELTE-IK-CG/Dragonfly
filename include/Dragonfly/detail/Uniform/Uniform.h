#pragma once
#include "../Program/Program.h"
#include "../Traits/UniformTypes.hpp"

#include <unordered_map>
#include <glm/glm.hpp>
#include <map>

namespace df
{

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

protected:
	GLuint program_id = 0;
	std::unordered_map<std::string, Values> locations;
	std::unordered_map<uint16_t, uint8_t> texLoc2sampler;
	std::vector<uint16_t> sampler2texLoc;
	SubroutinesBase& subroutines;
	Uniforms(GLuint program_id, SubroutinesBase& sub) : program_id(program_id), subroutines(sub) {}
	GLuint GetUniformLocation(const std::string& str) const;
public:
	Uniforms() = delete;
	template<typename ValType>
	inline void SetUniform(std::string&& str, ValType&& val);
	void SetUniform(std::string && uniform, const char * subroutine);
	//Do this on shader program compilation.
	bool Compile();

	//Does absolutely nothing. For UI use UniformEditor
	inline void Render(const std::string& program_name = "") {}
};


//using GreedyUniforms [[deprecated("Use the \"Uniform\" class instead.")]] = Uniforms;

} //namespace df


#include "Uniform.inl"
