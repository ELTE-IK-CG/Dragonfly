#pragma once
#include "Uniforms.h"
#include "Texture.h"
#include <typeinfo>

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
struct _TextureBindHelper
{
	static void bind(const ValType&,unsigned int){}
};

template<typename InternalFormat, TextureType TexType>
struct _TextureBindHelper<Texture<InternalFormat, TexType>>
{
	static void bind(const Texture<InternalFormat, TexType>& tex, unsigned int hwSamplerUnit) { tex.bind(hwSamplerUnit); }
};

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
	auto it2 = texLoc2sampler.find(it->second.loc);
	if (it2 == texLoc2sampler.end())
	{
		ASSERT(getOpenGLType<ValType>() == it->second.gpu_type, ("The uniform \"" + str + "\" of type \"" + typeid(ValType).name() + "\" had a different type in the shader.").c_str());
		SetUni(it->second.loc, val); // Regular uniforms
	}
	else
	{
		//Setting a TEXTURE
		//_TextureBindHelper<ValType>::bind(val, it2->second); //does glActiveTexture call as well (for optimizing later)
		glUniform1i(it->second.loc, static_cast<GLint>(it2->second)); //same as SetUni
	}
}
