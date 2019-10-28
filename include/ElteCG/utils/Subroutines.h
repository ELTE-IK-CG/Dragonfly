#pragma once
#include <GL/glew.h>
#include <string>
#include <vector>
#include <map>

class Subroutines
{
public:
	Subroutines() {}
	// Init: can be recalled with a new program, old settings will be lost
	void Init(GLuint program, GLenum shadertype);
	// SetSubroutines: has to be called after every program bind before drawing
	void SetSubroutines() const;

	bool RenderUI();
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

	bool uiIsOpen;

	bool SubroutineSelector(const std::vector<GLint>& compatibleSubs, GLuint& index);
};

class CachedSubroutines : public Subroutines
{
public:
	class UniformToSet {
		friend class CachedSubroutines;
		CachedSubroutines& that;
		std::string uniName;
		UniformToSet() = delete;
		UniformToSet(CachedSubroutines& c, const std::string s) : that(c), uniName(s) {}
		UniformToSet(const UniformToSet&) = delete;
		UniformToSet& operator=(const UniformToSet&) = delete;
	public:
		CachedSubroutines& operator<<(const std::string& subName) {
			that.SetSubroutine(uniName, subName);
			return that;
		}
	};

	void Init(GLuint program, GLenum shadertype);
	bool SetSubroutine(const std::string& uniform, const std::string& subroutine);
	UniformToSet operator<<(const std::string& uniName) {
		return UniformToSet(*this, uniName);
	}

protected:
	std::map<std::string, GLint> uniLocs; // uniName name -> uniName location
	std::map<std::string, GLuint> subInds; // sub name -> sub index
};