#pragma once
#include <GL/glew.h>
#include <string>
#include <vector>
#include <map>

namespace df
{

class ShaderSubroutines
{
	friend class SubroutinesBase;
	template<typename Shaders_T>
	friend class Subroutines;
public:
	ShaderSubroutines() = delete;
	ShaderSubroutines(ShaderSubroutines&&) = default;
	// Compile: can be called when shaders change, old settings will be lost
	void Compile();
	// SetSubroutines: has to be called after every program bind before drawing
	void SetSubroutines() const {
		if (!indices.empty())
			glUniformSubroutinesuiv(shadertype, static_cast<GLsizei>(indices.size()), &indices[0]);
	}

	void RenderUI();
protected:
	struct SubUniform {
		std::string name;
		GLint loc;
		GLint size;
		std::vector<GLint> compatibleSubs;
	};

	GLuint program;
	GLenum shadertype;

	std::vector<SubUniform> uniforms;
	std::vector<std::string> subNames;  // sub index -> sub name
	std::vector<GLuint> indices;   // uniName location -> currently set sub index

	ShaderSubroutines(GLuint program, GLenum shadertype);

	bool SubroutineSelector(const std::vector<GLint>& compatibleSubs, GLuint& index);
};

class SubroutinesBase
{
public:
	// Compile: can be called when shaders change, old settings will be lost
	bool Compile();
	// SetSubroutine: set a named uniform subroutin to a named function
	bool SetSubroutine(const std::string& uniform, const std::string& subroutine);
	// SetSubroutines: has to be called after every program bind before drawing
	void SetSubroutines();
	// HasUnifrom: is a particular subroutine uniform present in the program
	bool HasUniform(const std::string& uniform) const;

	void Render(const std::string& program_name);
protected:
	SubroutinesBase() {}

	GLint program_id;
	std::vector<ShaderSubroutines> shaderSubs;

	std::map<std::string, std::pair<uint8_t, size_t>> uniIndices; // uniName -> (shader index, uniform location)
	std::map <std::pair<uint8_t, std::string>, GLuint> subInds; // (shader index, subroutine name) -> subroutine index
};

template<typename Shaders_T>
class Subroutines : public SubroutinesBase
{
public:
	Subroutines() = delete;
	Subroutines(GLint program);
	void Render(const std::string& program_name) {}
};


template<typename Shaders_T>
class SubroutinesEditor : public Subroutines<Shaders_T>
{
public:
	SubroutinesEditor(GLint program) : Subroutines<Shaders_T>(program){}
	using SubroutinesBase::Render;
};

} // namespace df

#include "Subroutines.inl"
