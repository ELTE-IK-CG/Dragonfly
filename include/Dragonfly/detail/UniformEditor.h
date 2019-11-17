#pragma once
#include "Uniform.h"

namespace df
{

class UniformEditor : public df::Uniforms {
	using Base = df::Uniforms;
private:
	class UniformData : OpenGL_BaseType{
		friend class UniformEditor;
		UniformData(const char* name, GLenum type,GLint idx, GLint loc, GLsizei size);
		GLint idx, loc;
		GLsizei gpu_size; size_t cpu_size;
		std::string gpu_type, name, cpu_type;
		bool ignore_input = false; 
		//void* input_var = nullptr; //this way we can change the varible in this ugly manner (very unsafe for the user)
	};
	std::unordered_map<GLint, UniformData> loc2data;
	std::vector<GLint> loc_order;
public:
	UniformEditor(GLuint program_id, SubroutinesBase& sub) : Base(program_id, sub) {}
	void Render(const std::string &program_name = "");
	bool Compile();
	template<typename ValType>
	inline void SetUniform(std::string&& str, ValType& val);
	template<typename ValType>
	inline void SetUniform(std::string&& str, const ValType& val);
	inline void SetUniform(std::string&& str, const char* val);
};

} //namespace df

template<typename ValType>
inline void df::UniformEditor::SetUniform(std::string&& str, const ValType& val)
{

	//static_assert(is_list_member<ValType, valid_types>(), "Invalid type in SetUniform: wrong type given to program with << operator (like a string), or implementation may be missing.");
	GLuint loc = GetUniformLocation(str);
	auto it = loc2data.find(loc);
	if (it == loc2data.end()) {
		GPU_WARNING(true, ("Uniform " + str + " is not set becuase it doesn't exist in the shader file.").c_str());
		return;
	}
	UniformData& d = it->second;
	if (d.ignore_input) return; //input ignored haha
	using VT = std::remove_reference_t<std::remove_cv_t<ValType>>;
	if constexpr (std::is_base_of_v<TextureLowLevelBase, VT>) {
		auto tex_it = texLoc2sampler.find(it->second.loc);
		val.bind(tex_it->second);
		GLint smapler = static_cast<GLint>(tex_it->second);
		d.variant = smapler;
		glUniform1i(loc, smapler); //same as SetUni
	}
	else {
		ASSERT(std::holds_alternative<std::remove_cv_t<std::remove_reference_t<ValType>>>(d.variant), ("The uniform \"" + d.name + "\" had a different type before.").c_str());
		d.variant = val;
		//d.input_var = nullptr;
		SetUni(loc, val);
	}
}

template<typename ValType>
inline void df::UniformEditor::SetUniform(std::string&& str, ValType& val)
{
	using VT = std::remove_reference_t<std::remove_cv_t<ValType>>;
	static_assert(is_list_member<ValType, valid_types>() || std::is_base_of_v<TextureLowLevelBase, VT>, "Invalid type. TODO fix otherwise.");
	GLuint loc = GetUniformLocation(str);
	auto it = loc2data.find(loc);
	ASSERT(it != loc2data.end(), "This location was not stored.");
	UniformData& d = it->second;
	if (d.ignore_input) return; //input ignored haha
	if constexpr (std::is_base_of_v<TextureLowLevelBase, VT>) {
		auto tex_it = texLoc2sampler.find(it->second.loc);
		val.bind(tex_it->second);
		GLint smapler = static_cast<GLint>(tex_it->second);
		d.variant = smapler;
		glUniform1i(loc, smapler); //same as SetUni
	}
	else {
		ASSERT(std::holds_alternative<std::remove_cv_t<std::remove_reference_t<ValType>>>(d.variant), ("The uniform \"" + d.name + "\" had a different type before.").c_str());
		//d.input_var = &val;
		d.variant = val;
		SetUni(loc, val);
	}
}

template<>
inline void df::UniformEditor::SetUniform<>(std::string&& str, const std::string& val)
{
	Base::SetUniform(std::move(str), val);
}

inline void df::UniformEditor::SetUniform(std::string&& str, const char* val)
{
	Base::SetUniform(std::move(str), val);
}
