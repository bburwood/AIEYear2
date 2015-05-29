#include <cstdio>
#include "Utility.h"
#include "gl_core_4_4.h"


bool LoadShaderType(char* filename, 
					GLenum shader_type, 
					unsigned int* output)
{
	//we want to be able to return if we succeded
	bool succeeded = true;

	//open the shader file
    FILE* shader_file;
    fopen_s(&shader_file, filename, "r");

	//did it open successfully 
	if (shader_file == 0)
	{
		succeeded = false;
	}
	else
	{
		//find out how long the file is
		fseek(shader_file, 0, SEEK_END); 
		int shader_file_length = ftell(shader_file);
		fseek(shader_file, 0, SEEK_SET);
		//allocate enough space for the file
		char *shader_source = new char[shader_file_length];
		//read the file and update the length to be accurate
		shader_file_length = fread(shader_source, 1, shader_file_length, shader_file);

		//create the shader based on the type that got passed in
		unsigned int shader_handle = glCreateShader(shader_type);
		//compile the shader
		glShaderSource(shader_handle, 1, &shader_source, &shader_file_length);
		glCompileShader(shader_handle);

		//chech the shader for errors
		int success = GL_FALSE;
		glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			int log_length = 0;
			glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &log_length);
			char* log = new char[log_length];
			glGetShaderInfoLog(shader_handle, log_length, NULL, log);
			printf("%s\n", log);
			delete[] log;
			succeeded = false;
		}
		//only give the result to the caller if we succeeded
		if (succeeded)
		{
			*output = shader_handle;
		}

		//clean up the stuff we allocated
		delete[] shader_source;
		fclose(shader_file);
	}

	return succeeded;
}

bool LoadShader(
	char* vertex_filename,
	char* geometry_filename,
	char* fragment_filename,
	GLuint* result)
{
	bool succeeded = true;

	*result = glCreateProgram();

	unsigned int vertex_shader;

	if ( LoadShaderType(vertex_filename, GL_VERTEX_SHADER, &vertex_shader) )
	{
		glAttachShader(*result, vertex_shader);
		glDeleteShader(vertex_shader);
	}
	else
	{
		printf("FAILED TO LOAD VERTEX SHADER\n");
	}

	if (geometry_filename != nullptr)
	{
		unsigned int geometry_shader;
		if (LoadShaderType(geometry_filename, GL_GEOMETRY_SHADER, &geometry_shader))
		{
			glAttachShader(*result, geometry_shader);
			glDeleteShader(geometry_shader);
		}
		else
		{
			printf("FAILED TO LOAD GEOMETRY SHADER\n");
		}
	}
	if (fragment_filename != nullptr)
	{
		unsigned int fragment_shader;
		if (LoadShaderType(fragment_filename, GL_FRAGMENT_SHADER, &fragment_shader))
		{
			glAttachShader(*result, fragment_shader);
			glDeleteShader(fragment_shader);
		}
		else
		{
			printf("FAILED TO LOAD FRAGMENT SHADER\n");
		}
	}

	glLinkProgram(*result);

	GLint success; 
	glGetProgramiv(*result, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint log_length;
		glGetProgramiv(*result, GL_INFO_LOG_LENGTH, &log_length);
		char* log = new char[log_length];
		glGetProgramInfoLog(*result, log_length, 0, log);

		printf("ERROR: STUFF DONE SCREWED UP IN UR SHADER BUDDY!\n\n");
		printf("%s", log);

		delete[] log;
		succeeded = false;
	}

	return succeeded;
}

OpenGLData generateQuad(float size)
{
	OpenGLData result = {};
	
	VertexTexCoord vertex_data[4];

	vertex_data[0].position = vec4(-size, 0, -size, 1);
	vertex_data[1].position = vec4(-size, 0, size, 1);
	vertex_data[2].position = vec4(size, 0, size, 1);
	vertex_data[3].position = vec4(size, 0, -size, 1);

	vertex_data[0].tex_coord = vec2(0, 0);
	vertex_data[1].tex_coord = vec2(0, 1);
	vertex_data[2].tex_coord = vec2(1, 1);
	vertex_data[3].tex_coord = vec2(1, 0);

	unsigned int index_data[6] = { 0, 2, 1, 0, 3, 2 };
	result.m_index_count = 6;

	glGenVertexArrays(1, &result.m_VAO);
	glGenBuffers(1, &result.m_VBO);
	glGenBuffers(1, &result.m_IBO);

	glBindVertexArray(result.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, result.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTexCoord) * 4, 
									vertex_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, 
										index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //tex coord

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 
									sizeof(VertexTexCoord), 0); //position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
					sizeof(VertexTexCoord), (void*)sizeof(vec4)); //tex coord

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return result;
}