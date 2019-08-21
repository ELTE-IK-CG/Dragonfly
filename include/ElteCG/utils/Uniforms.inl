#pragma once
#include "Uniforms.h"
#include <typeinfo>



template<typename ValType>
inline void LazyUniforms::SetUniform(std::string && str, ValType && val) {
	GLuint location = GetUniformLocation(str);
	ASSERT(location == -1, ("Uniform \"" + str + "\" not found in shader code.").c_str());
	SetUni(location, val);
}

template<typename ValType>
inline void GreedyUniforms::SetUniform(std::string&& str, ValType&& val)
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
	ASSERT(getOpenGLType<ValType>() == it->second.gpu_type, ("The uniform \"" + str + "\" of type \"" + typeid(ValType).name() + "\" had a different type in the shader.").c_str());
	SetUni(it->second.loc, val);
}

template<typename ValType>
inline void CachedUniforms::SetUniform(std::string && str, const ValType & val) {
	using Type = std::remove_cv_t < std::remove_reference_t <ValType>>;
	GLuint location = GetUniformLocation(str);
	if constexpr (UniformLowLevelBase::is_list_member<Type, valid_types>()) {
		// ValType is a cached tdype
		if (auto it = cache.find(location); it != cache.end()) {
			auto pvar = std::get_if<std::remove_cv_t < std::remove_reference_t <ValType>>>(&it->second);
			ASSERT(pvar != nullptr, ("The uniform" + str + " was assigned with a different type before.").c_str());
			if (pvar  && *pvar != val) {
				it->second = val;	// in cache but had a different value
				SetUni(location, val);
			}
		}
		else {	// not in the cache yet
			cache.emplace(location, val);
			SetUni(location, val);
		}
	}
	else {// ValType is not a cached type. We still check for errors
#ifdef _DEBUG
		if (auto it = cache.find(location); it != cache.end()) {
			// TODO TODO FIX
			if (auto pvar = std::get_if<uni_hash_type>(&it->second))	{
				ASSERT(pvar->h == typeid(ValType).hash_code(), ("The (uncached) uniform"+ str+" was assigned with a different (uncached) type before.").c_str());
			}
			else { ASSERT(false, ("The (uncached) uniform" + str + " was assigned with a different (cached) type before.").c_str()); }
			//XIF ODOT ODOT */
		}
		else cache.emplace(location, uni_hash_type(typeid(ValType).hash_code()));
#endif
		this->SetUni(location, val);
	}
}