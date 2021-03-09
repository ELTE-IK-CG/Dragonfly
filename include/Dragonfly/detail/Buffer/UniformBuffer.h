#pragma once
#include <GL/glew.h>
#include <map>
#include <vector>
#include <string>

struct UniformBlockLayout
{
	struct ValueData
	{
		std::string name;
		GLuint location;
		GLenum type;
		GLint array_size;
	};
	std::map<std::string, ValueData> uniforms;
};

class UniformBuffer
{
	//UniformBuffer
	
};

void programParseUniformProperties()
{
	GLuint prog;

	//TODO goes to program class
	std::map<std::string, UniformBlockLayout> _UboLayouts; //create UBO-s using these
	UniformBlockLayout _nonBufferBacked;
	
	/// Loop over each uniform block
	
	GLint numBlocks = 0;
	glGetProgramInterfaceiv(prog, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numBlocks);
	
	const GLenum blockProperties[1] = { GL_NUM_ACTIVE_VARIABLES };
	const GLenum activeUnifProp[1] = { GL_ACTIVE_VARIABLES };
	const GLenum unifProperties[3] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };

	struct BufferBlockData
	{
		std::string name;
		GLint binding;
		GLint data_size;
		GLint active_variables;
	};
	std::vector<BufferBlockData> blocks(numBlocks);

	for (int blockIx = 0; blockIx < numBlocks; ++blockIx)
	{
		struct Values {
			GLint name_length;
			GLint binding;
			GLint data_size;
			GLint active_variables;
		} values;
		const GLenum unifProperties[] = { GL_NAME_LENGTH, GL_BUFFER_BINDING, GL_BUFFER_DATA_SIZE, GL_NUM_ACTIVE_VARIABLES };
		glGetProgramResourceiv(prog, GL_UNIFORM_BLOCK, blockIx, 4, unifProperties, 4, nullptr, &values.name_length);
		std::string name(values.name_length, ' ');
		glGetProgramResourceName(prog, GL_UNIFORM_BLOCK, blockIx, values.name_length, nullptr, &name[0]);
		auto& b = blocks[blockIx];
		b.name				= std::move(name);
		b.binding			= values.binding;
		b.data_size			= values.data_size;
		b.active_variables	= values.active_variables;
	}

	/// Loop over every uniform
	
	GLint numUniforms = 0;
	glGetProgramInterfaceiv(prog, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);
		
	for (int unif = 0; unif < numUniforms; ++unif)
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
		std::string name(values.name_length,' ');
		glGetProgramResourceName(prog, GL_UNIFORM, unif, values.name_length, nullptr, &name[0]);
		
		UniformBlockLayout::ValueData dat;
		dat.name		= name;
		dat.location	= values.location;
		dat.type		= values.type;
		dat.array_size	= values.array_size;
		
		if(values.block_index == -1) // not in uniform buffer block
		{
			_nonBufferBacked.uniforms.emplace(name, dat);
		}
		else	// in uniform buffer block
		{
			_UboLayouts[blocks[values.block_index].name].uniforms.emplace(name,dat);
		}
	}

}



//{
//	//non block uniforms
//	GLint numUniforms = 0;
//	glGetProgramInterfaceiv(prog, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);
//	const GLenum properties[4] = { GL_BLOCK_INDEX​, GL_TYPE​, GL_NAME_LENGTH​, GL_LOCATION };

//	for (int unif = 0; unif < numUniforms; ++unif)
//	{
//		GLint values[4];
//		glGetProgramResourceiv(prog, GL_UNIFORM, unif, 4, properties, 4, NULL, values);

//		// Skip any uniforms that are in a block.
//		if (values[0] != -1)
//			continue;

//		// Get the name. Must use a std::vector rather than a std::string for C++03 standards issues.
//		// C++11 would let you use a std::string directly.
//		std::vector<char> nameData(values[2]);
//		glGetProgramResourceName(prog, GL_UNIFORM, unif, nameData.size(), NULL, &nameData[0]);
//		std::string name(nameData.begin(), nameData.end() - 1);
//	}

//	//uniform blocks
//	GLint numBlocks = 0;
//	glGetProgramInterfaceiv(prog, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numBlocks);
//	const GLenum blockProperties[1] = { GL_NUM_ACTIVE_VARIABLES };
//	const GLenum activeUnifProp[1] = { GL_ACTIVE_VARIABLES };
//	const GLenum unifProperties[3] = { GL_NAME_LENGTH​, GL_TYPE​, GL_LOCATION };

//	for (int blockIx = 0; blockIx < numBlocks; ++blockIx)
//	{
//		GLint numActiveUnifs = 0;
//		glGetProgramResourceiv(prog, GL_UNIFORM_BLOCK, blockIx, 1, blockProperties, 1, NULL, numActiveUnifs);

//		if (!numActiveUnifs)
//			continue;

//		std::vector<GLint> blockUnifs(numActiveUnifs);
//		glGetProgramResourceiv(prog, GL_UNIFORM_BLOCK, blockIx, 1, activeUnifProp, numActiveUnifs, NULL, &blockUnifs[0]);

//		for (int unifIx = 0; unifIx < numActiveUnifs; ++unifIx)
//		{
//			GLint values[3];
//			glGetProgramResourceiv(prog, GL_UNIFORM, blockUnifs[unifIx], 3, unifProperties, 3, NULL, values);

//			// Get the name. Must use a std::vector rather than a std::string for C++03 standards issues.
//			// C++11 would let you use a std::string directly.
//			std::vector<char> nameData(values[0]);
//			glGetProgramResourceName(prog, GL_UNIFORM, blockUnifs[unifIx], nameData.size(), NULL, &nameData[0]);
//			std::string name(nameData.begin(), nameData.end() - 1);
//		}
//	}
//}