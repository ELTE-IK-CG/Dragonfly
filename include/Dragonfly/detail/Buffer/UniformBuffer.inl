#pragma once


template<typename T_>
inline void df::UniformBuffer::MemoryAdapter::operator=(T_&& val_)&&
{
	using NakedValType = std::remove_reference_t<std::remove_cv_t<T_>>;
	if (!data_ptr) return; // non existent uniform
	// TODO type check
	// TODO size check
	// TODO implement array, init list, and vector assignement
	df::detail::getUniformTypeOpenGL<NakedValType>();
	void* ptr = df::detail::getUniformTypePtr(val_);
	size_t size = df::detail::getUnifromTypeSize(val_);
	memcpy(data_ptr, ptr, size);
}

template<typename T_>
inline T_* df::UniformBuffer::MemoryAdapter::Get()&&
{
	// TODO type check
	return reinterpret_cast<T_*>(data_ptr);
}