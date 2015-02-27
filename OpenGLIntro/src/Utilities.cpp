#include "gl_core_4_4.h"
#include <cstdio>


bool	LoadShaderType(char* fileName, GLenum shaderType, unsigned int* output)
{
	bool	succeeded = false;
	//	open shader file
	FILE*	shader_file = fopen(fileName, "r");
	//	did it open successfully
	if (shader_file == 0)
	{
		if (shader_file == 0)
		{
			printf("ERROR: Failed to open the \"%s\" shader file!!\n", fileName);
		}
		succeeded = false;
	}
	else
	{
		//	find out how long the files is
		fseek(shader_file, 0, SEEK_END);
		int	vertex_file_length = ftell(shader_file);
		fseek(shader_file, 0, SEEK_SET);

		//	allocate space for the file
		char*	shader_source = new char[vertex_file_length];

		//	read the file and update the length to be accurate
		vertex_file_length = fread(shader_source, 1, vertex_file_length, shader_file);
		succeeded = true;
		//	create the shader based on the type passed in
		unsigned int	shaderHandle = glCreateShader(shaderType);
		//	compile the shader
		glShaderSource(shaderHandle, 1, &shader_source, &vertex_file_length);
		glCompileShader(shaderHandle);

		//	error checking now ...
		int	success = GL_FALSE;
		glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE)
		{
			//	then we had a problem
			int	log_length = 0;
			glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &log_length);
			GLchar*	log = new char[log_length];
			glGetShaderInfoLog(shaderHandle, log_length, 0, log);

			printf("ERROR: Shader compile failed!!\nFile: %s\nError Log:\n", fileName);
			printf("%s\n", log);
			delete[] log;
			succeeded = false;
		}
		//	only give the result to the caller if we succeeded
		if (succeeded)
		{
			*output = shaderHandle;
		}
		//glDeleteShader(shaderHandle);

		delete[] shader_source;
	}
	if (shader_file != NULL)
	{
		fclose(shader_file);
	}
	return succeeded;
}

bool	LoadShader(char* vertex_filename, char* geometry_filename, char* fragment_filename, GLuint* result)
{
	bool	succeeded = true;
	unsigned int	vertex_shader;

	*result = glCreateProgram();
	LoadShaderType(vertex_filename, GL_VERTEX_SHADER, &vertex_shader);
	glAttachShader(*result, vertex_shader);
	glDeleteShader(vertex_shader);
	if (geometry_filename != nullptr)
	{
		unsigned int	geometry_shader;
		if (LoadShaderType(geometry_filename, GL_GEOMETRY_SHADER, &geometry_shader))
		{
			glAttachShader(*result, geometry_shader);
			glDeleteShader(geometry_shader);
		}
	}
	if (fragment_filename != nullptr)
	{
		unsigned int	fragment_shader;
		if (LoadShaderType(fragment_filename, GL_FRAGMENT_SHADER, &fragment_shader))
		{
			glAttachShader(*result, fragment_shader);
			glDeleteShader(fragment_shader);
		}
	}
	glLinkProgram(*result);

	GLint	success;
	glGetProgramiv(*result, GL_LINK_STATUS, &success);
	/////////
	if (success == GL_FALSE)
	{
		//	then we had a problem
		GLint	log_length = 0;
		glGetProgramiv(*result, GL_INFO_LOG_LENGTH, &log_length);
		char*	log = new char[log_length];
		glGetProgramInfoLog(*result, log_length, 0, log);

		printf("ERROR: Shader compile failed!!\nError Log:\n");
		printf("%s", log);
		delete[] log;
		succeeded = false;
	}

	return succeeded;
}

/*
bool	LoadShader(char* vertex_filename, char* fragment_filename, GLuint* result)
{
	bool	succeeded = false;
	FILE*	vertex_file = fopen(vertex_filename, "r");
	FILE*	fragment_file = fopen(fragment_filename, "r");

	if (vertex_file == 0 || fragment_file == 0)
	{
		if (vertex_file == 0)
		{
			printf("ERROR: Failed to open the \"%s\" shader file!!\n", vertex_filename);
		}
		if (fragment_file == 0)
		{
			printf("ERROR: Failed to open the \"%s\" shader file!!\n", fragment_filename);
		}
		succeeded = false;
	}
	else
	{
		fseek(vertex_file, 0, SEEK_END);
		int	vertex_file_length = ftell(vertex_file);
		fseek(vertex_file, 0, SEEK_SET);

		fseek(fragment_file, 0, SEEK_END);
		int	fragment_file_length = ftell(fragment_file);
		fseek(fragment_file, 0, SEEK_SET);

		char*	vs_source = new char[vertex_file_length];
		char*	fs_source = new char[fragment_file_length];

		vertex_file_length = fread(vs_source, 1, vertex_file_length, vertex_file);
		fragment_file_length = fread(fs_source, 1, fragment_file_length, fragment_file);


		succeeded = true;
		unsigned int	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		unsigned int	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(vertex_shader, 1, &vs_source, &vertex_file_length);
		glCompileShader(vertex_shader);
		glShaderSource(fragment_shader, 1, &fs_source, &fragment_file_length);
		glCompileShader(fragment_shader);
		*result = glCreateProgram();
		glAttachShader(*result, vertex_shader);
		glAttachShader(*result, fragment_shader);
		glLinkProgram(*result);

		//	error checking now ...
		int	success = GL_FALSE;
		glGetProgramiv(*result, GL_LINK_STATUS, &success);
		if (success == GL_FALSE)
		{
			//	then we had a problem
			int	log_length = 0;
			glGetProgramiv(*result, GL_INFO_LOG_LENGTH, &log_length);
			char*	log = new char[log_length];
			glGetProgramInfoLog(*result, log_length, 0, log);

			printf("ERROR: Shader compile failed!!\nError Log:\n");
			printf("%s", log);
			delete[] log;
			succeeded = false;
		}

		glDeleteShader(fragment_shader);
		glDeleteShader(vertex_shader);

		delete[] vs_source;
		delete[] fs_source;
	}
	if (vertex_file != NULL)
	{
		fclose(vertex_file);
	}
	if (fragment_file != NULL)
	{
		fclose(fragment_file);
	}
	return succeeded;
}
*/