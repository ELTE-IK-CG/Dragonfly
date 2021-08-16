#pragma once
// New!!
#include <GL/glew.h>
#include <Dragonfly/detail/Buffer/Buffer.h>
#include <map>
#include <vector>
#include <string>
#include <memory>

namespace df
{
	

struct UniformBlockLayout
{
	struct ValueData
	{
		std::string name;
		GLuint offset; //location if not backed by a (uniform) buffer block
		GLenum type;
		GLint array_size;
	};
	std::map<std::string, ValueData> uniforms;
	
	int size;
	int active_variables;

	bool operator== (const UniformBlockLayout &other_) const
	{
		if (other_.size != size || other_.uniforms.size() != uniforms.size()) return false;
		//TODODO
		return true;
	}
};


class UniformBuffer : public detail::BufferLowLevelBase
{
private:
	using Binary_Data = std::vector<unsigned char>;
public:
	class MemoryAdapter
	{
	private:
		std::weak_ptr<std::vector<unsigned char>> data_ptr; // weak ptr
		UniformBlockLayout::ValueData vardata;
		friend UniformBuffer;
		int offset = 0;
	private:
		explicit MemoryAdapter(){}
		explicit MemoryAdapter(std::weak_ptr<Binary_Data> data_ptr_, UniformBlockLayout::ValueData vardata_, int offset_ = 0) :
			data_ptr(data_ptr_), vardata(std::move(vardata_)), offset(offset_)
		{}
	public:
		template<typename T_>
		void operator=(T_&& val_)
		{
			using NakedValType = std::remove_reference_t<std::remove_cv_t<T_>>;
			auto sp = data_ptr.lock();
			if(!sp) return; //invalid uniform buffer cpu data
			if(sizeof(NakedValType) + vardata.offset >= sp->size()) return; // too large value
			// TODO Type warning check -- think...
			// TODO test arrays
			*reinterpret_cast<NakedValType*>(sp->data() + vardata.offset) = val_;
		}

		/*template<typename T_>
		T_& get()
		{
			//todo maybe?
		}
		
		MemoryAdapter& operator[](int indx_) &&
		{
			offset = 4*indx_; //todo type size
			return *this;
		}*/
	};
private:
	UniformBlockLayout layout;
	std::shared_ptr<std::vector<unsigned char>> data; // CPU copy of the buffer
public:
	explicit UniformBuffer(UniformBlockLayout layout_) :
		BufferLowLevelBase(layout_.size, BUFFER_BITS::WRITE),
		layout(std::move(layout_)),
		data(std::make_shared<std::vector<unsigned char>>(layout.size))
	{}

	MemoryAdapter operator[](const std::string& key_)
	{
		if(const auto it = layout.uniforms.find(key_); it != layout.uniforms.end())
		{
			return MemoryAdapter(data, it->second);
		}
		else return MemoryAdapter();
	}
	// TODO upload automatically (when?)
	void UploadBuffer()
	{
		if(data->size() > 0)
			_UploadData(data->data(), data->size());
	}
	void Bind(const GLuint index_)
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, index_, static_cast<GLuint>(*this));
	}
};

}

//TODO this goes to program class
inline std::pair<std::map<std::string, df::UniformBlockLayout>,df::UniformBlockLayout> programParseUniformProperties(GLuint prog)
{
	std::map<std::string, df::UniformBlockLayout> ubo_layouts; //create UBO-s using these
	df::UniformBlockLayout non_buffer_backed;
	
	/// Loop over each uniform block
	
	GLint num_blocks = 0;
	glGetProgramInterfaceiv(prog, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &num_blocks);

	struct BufferBlockData
	{
		std::string name;
		GLint binding;
		GLint data_size;
		GLint active_variables;
	};
	std::vector<BufferBlockData> blocks(num_blocks);

	for (int blockIx = 0; blockIx < num_blocks; ++blockIx)
	{
		struct Values {
			GLint name_length;
			GLint binding;
			GLint data_size;
			GLint active_variables;
		} values;
		const GLenum unifProperties[] = { GL_NAME_LENGTH, GL_BUFFER_BINDING, GL_BUFFER_DATA_SIZE, GL_NUM_ACTIVE_VARIABLES };
		glGetProgramResourceiv(prog, GL_UNIFORM_BLOCK, blockIx, 4, unifProperties, 4, nullptr, &values.name_length);
		std::string name(values.name_length - 1, '\0');
		glGetProgramResourceName(prog, GL_UNIFORM_BLOCK, blockIx, values.name_length, nullptr, &name[0]);
		auto& b = blocks[blockIx];
		b.name				= std::move(name);
		b.binding			= values.binding;
		b.data_size			= values.data_size;
		b.active_variables	= values.active_variables;
		auto& lay = ubo_layouts[b.name];
		lay.size = b.data_size;
		lay.active_variables = b.active_variables;
	}

	/// Loop over every uniform
	
	GLint num_uniforms = 0;
	glGetProgramInterfaceiv(prog, GL_UNIFORM, GL_ACTIVE_RESOURCES, &num_uniforms);
		
	for (int unif = 0; unif < num_uniforms; ++unif)
	{
		struct Values
		{
			GLint name_length;
			GLenum type;
			GLint array_size;
			GLint offset;
			GLint block_index;
			GLint array_stride;
			GLint matrix_stride;
			GLenum is_row_major;
			GLint atomic_counter_buffer_index;
			GLint location;
		} values;
		const GLenum properties[] = {GL_NAME_LENGTH, GL_TYPE, GL_ARRAY_SIZE, GL_OFFSET,GL_BLOCK_INDEX, GL_ARRAY_STRIDE, GL_MATRIX_STRIDE, GL_IS_ROW_MAJOR, GL_ATOMIC_COUNTER_BUFFER_INDEX, GL_LOCATION};

		glGetProgramResourceiv(prog, GL_UNIFORM, unif, 10, properties, 10, nullptr, &values.name_length);
		std::string name(values.name_length - 1,'\0');
		glGetProgramResourceName(prog, GL_UNIFORM, unif, values.name_length, nullptr, &name[0]);
		// remove "[0]" from array names
		if(name[name.size() - 1] == ']')
		{
			name.resize(name.size() - 3);
		}
		df::UniformBlockLayout::ValueData dat;
		dat.name		= name;
		dat.type		= values.type;
		dat.array_size	= values.array_size;
		
		if(values.block_index == -1) // not in uniform buffer block
		{
			dat.offset	= values.location;
			non_buffer_backed.uniforms.emplace(name, dat);
		}
		else	// in uniform buffer block
		{
			dat.offset	= values.offset;
			ubo_layouts[blocks[values.block_index].name].uniforms.emplace(name,dat);
		}
	}
	// TODO uniform block arrays are not filled (only arr[0])
	return {ubo_layouts, non_buffer_backed};
}
