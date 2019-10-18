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
		_UniformSetterHelper<Texture<glm::u8vec3, TextureType::TEX_2D>>().call(*this, str, it->second, val);
	}
	else {
		_UniformSetterHelper<ValType>().call(*this, str, it->second, val);
	}

}
