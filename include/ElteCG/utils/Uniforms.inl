#pragma once
#include "Uniforms.h"
#include <typeinfo>
#include <iostream>

inline GLuint Uniforms::GetUniformLocation(const std::string& str) const {
	if (auto it = locations.find(str); it != locations.end()) {
		return it->second.loc;
	}
	else {
		WARNING(true, ("Uniform \"" + str + "\" location not found. The shader program might not have this uniform, or it was not properly compiled.").c_str());
		return -1;
	}
}

template<typename ValType>
inline void Uniforms::SetUniform(std::string&& str, ValType&& val)
{
	ASSERT(!locations.empty(), "Shader program doesn't have any uniforms compiled.");
	auto it = locations.find(str);
	if (it == locations.end())
	{
		WARNING(true, ("The uniform \"" + str + "\" of type \"" + typeid(ValType).name() + "\" was not part of the compiled shader.").c_str());
		return;
	}
#ifdef _DEBUG
	if (it->second.cpu_type == 0) it->second.cpu_type = typeid(ValType).hash_code();
#endif
	WARNING(typeid(ValType).hash_code() != it->second.cpu_type, ("The uniform \"" + str + "\" of type \"" + typeid(ValType).name() + "\" was initialized with a different type. Compare the calling << operator to the first one with this name.").c_str());

	using NakedValType = std::remove_reference_t<std::remove_cv_t<ValType>>;
	if constexpr (std::is_base_of_v<TextureLowLevelBase, NakedValType>) {
		auto tex_it = texLoc2sampler.find(it->second.loc);
		//ASSERT(tex_it != texLoc2sampler.end(), ("Texture sampler \"" + str + "\" not found of type \"Texture<" + typeid(InternalFormat).name() + ">\". This error should not occur.").c_str());
		//TODO ASSERT TYPE CHECK
		val.bind(tex_it->second);
		glUniform1i(it->second.loc, static_cast<GLint>(tex_it->second)); //same as SetUni
	}
	else {
		ASSERT(getOpenGLType<ValType>() == it->second.gpu_type, ("The uniform \"" + str + "\" of type \"" + typeid(ValType).name() + "\" had a different type in the shader.").c_str());
		ASSERT(texLoc2sampler.find(it->second.loc) == texLoc2sampler.end(), ("The uniform \"" + str + "\" of type \"" + typeid(ValType).name() + "\" is supposed to be a texture.").c_str());
		SetUni(it->second.loc, val); // Regular uniforms
	}

}
