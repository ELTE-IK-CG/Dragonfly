#pragma once
#include "Uniforms.h"



class UniformEditor : public GreedyUniforms {
	using Base = GreedyUniforms;
private:
	class UniformData : OpenGL_BaseType{
		friend class UniformEditor;
		UniformData(const char* name, GLenum type,GLint idx, GLint loc, GLsizei size);
		GLint idx, loc;
		GLsizei gpu_size; size_t cpu_size;
		std::string gpu_type, name, cpu_type;
		bool ignore_input = false; 
		void* input_var = nullptr; //this way we can change the varible in this ugly manner (very unsafe for the user)
	};
	std::unordered_map<GLint, UniformData> loc2data;
	std::vector<GLint> loc_order;
	static int ID;
	std::string myID;
public:
	UniformEditor(GLuint program_id) : Base(program_id) {}
	void Render(std::string program_name = "");
	bool Compile();
	template<typename ValType>
	inline void SetUniform(std::string&& str, ValType& val);
	template<typename ValType>
	inline void SetUniform(std::string&& str, const ValType& val);
	inline std::string GetNextID() { if (myID.empty()) myID = "Uniform " + std::to_string(++ID); return myID; }
};

template<typename ValType>
inline void UniformEditor::SetUniform(std::string&& str, const ValType& val)
{
	static_assert(is_list_member<ValType, valid_types>(), "Invalid type. TODO fix otherwise.");
	GLuint loc = GetUniformLocation(str);
	auto it = loc2data.find(loc);
	if (it == loc2data.end()) {
		GPU_WARNING(true, ("Uniform " + str + " is not set becuase it doesn't exist in the shader file.").c_str());
		return;
	}
	UniformData& d = it->second;
	if (d.ignore_input) return; //input ignored haha
	ASSERT(std::holds_alternative<std::remove_cv_t<std::remove_reference_t<ValType>>>(d.variant), ("The uniform \""+ d.name + "\" had a different type before.").c_str());
	d.variant = val;
	d.input_var = nullptr;
	SetUni(loc, val);
}

template<typename ValType>
inline void UniformEditor::SetUniform(std::string&& str, ValType& val)
{
	static_assert(is_list_member<ValType, valid_types>(), "Invalid type. TODO fix otherwise.");
	GLuint loc = GetUniformLocation(str);
	auto it = loc2data.find(loc);
	ASSERT(it != loc2data.end(), "This location was not stored.");
	UniformData& d = it->second;
	if (d.ignore_input) return; //input ignored haha
	ASSERT(std::holds_alternative<std::remove_cv_t<std::remove_reference_t<ValType>>>(d.variant), ("The uniform \"" + d.name + "\" had a different type before.").c_str());
	d.input_var = &val;
	d.variant = val;
	SetUni(loc, val);
}
