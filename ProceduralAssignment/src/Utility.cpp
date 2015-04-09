#include "Utility.h"
#include "Vertex.h"
#include "tiny_obj_loader.h"
#include <cstdio>
#include <iostream>

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

OpenGLData LoadOBJ(char* a_szFileName)
{
	OpenGLData	result = {};

	std::vector<tinyobj::shape_t>	shapes;
	std::vector<tinyobj::material_t>	materials;

	std::string err = tinyobj::LoadObj(shapes, materials, a_szFileName);

	if (err.size() != 0)
	{
		std::cout << err << '\n';
	}

	result.m_uiIndexCount = shapes[0].mesh.indices.size();

	tinyobj::mesh_t*	mesh = &shapes[0].mesh;

	std::vector<float>	vertexData;
	vertexData.reserve(mesh->positions.size() + mesh->normals.size());

	vertexData.insert(vertexData.end(), mesh->positions.begin(), mesh->positions.end());
	vertexData.insert(vertexData.end(), mesh->normals.begin(), mesh->normals.end());

	glGenVertexArrays(1, &result.m_uiVAO);
	glBindVertexArray(result.m_uiVAO);

	glGenBuffers(1, &result.m_uiVBO);
	glGenBuffers(1, &result.m_uiIBO);

	glBindBuffer(GL_ARRAY_BUFFER, result.m_uiVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.m_uiIBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* vertexData.size(), vertexData.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* mesh->indices.size(), mesh->indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	//	position
	glEnableVertexAttribArray(1);	//	normal
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)* mesh->positions.size()));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return result;
}

//	AntTweakBar helper functions
void	OnMouseButton(GLFWwindow* window, int button, int pressed, int mod_keys)
{
	TwEventMouseButtonGLFW(button, pressed);
}

void	OnMousePosition(GLFWwindow* window, double x, double y)
{
	TwEventMousePosGLFW((int)x, (int)y);
}

void	OnMouseScroll(GLFWwindow* window, double x, double y)
{
	TwEventMouseWheelGLFW(y);
}

void	OnKey(GLFWwindow* window, int key, int scancode, int pressed, int mod_keys)
{
	TwEventKeyGLFW(key, pressed);
}

void	OnChar(GLFWwindow* window, unsigned int c)
{
	TwEventCharGLFW(c, GLFW_PRESS);
}

void	OnWindowResize(GLFWwindow* window, int width, int height)
{
	TwWindowSize(width, height);
	glViewport(0, 0, width, height);
}
