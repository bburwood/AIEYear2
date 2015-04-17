#include "Utility.h"
#include "Vertex.h"
#include "tiny_obj_loader.h"
#include <cstdio>
#include <iostream>

#define	STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

OpenGLData LoadTexturedOBJ(char* a_szFileName)
{
	OpenGLData	result = {};
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

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
	vertexData.insert(vertexData.end(), mesh->texcoords.begin(), mesh->texcoords.end());

	glGenVertexArrays(1, &result.m_uiVAO);
	glBindVertexArray(result.m_uiVAO);

	glGenBuffers(1, &result.m_uiVBO);
	glGenBuffers(1, &result.m_uiIBO);

	glBindBuffer(GL_ARRAY_BUFFER, result.m_uiVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.m_uiIBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexData.size(), vertexData.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->indices.size(), mesh->indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	//	position
	glEnableVertexAttribArray(1);	//	normal
	glEnableVertexAttribArray(2);	//	texCoord
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * mesh->positions.size()));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * (mesh->positions.size() +  mesh->normals.size()) ) );

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	bool	bDebugInfo = true;
	bool	bExtraDebugInfo = false;
	if (bExtraDebugInfo)
	{
		std::cout << "mesh->texcoords.size(): " << mesh->texcoords.size() << '\n';
		for (unsigned int i = 0; i < mesh->texcoords.size(); ++i)
		{
			std::cout << mesh->texcoords[i] << "  ";
		}
		std::cout << '\n';
	}


	if (bDebugInfo)
	{
		//	extra debug output code from loading OBJ from the Tiny OBJ website: https://github.com/syoyo/tinyobjloader
		std::cout << "# of shapes    : " << shapes.size() << std::endl;
		std::cout << "# of materials : " << materials.size() << std::endl;
		for (size_t i = 0; i < shapes.size(); i++)
		{
			printf("shape[%ld].name = %s\n", i, shapes[i].name.c_str());
			printf("Size of shape[%ld].indices: %ld\n", i, shapes[i].mesh.indices.size());
			printf("Size of shape[%ld].material_ids: %ld\n", i, shapes[i].mesh.material_ids.size());
			assert((shapes[i].mesh.indices.size() % 3) == 0);
			if (bExtraDebugInfo)
			{
				for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++)
				{
					printf("  idx[%ld] = %d, %d, %d. mat_id = %d\n", f, shapes[i].mesh.indices[3 * f + 0], shapes[i].mesh.indices[3 * f + 1], shapes[i].mesh.indices[3 * f + 2], shapes[i].mesh.material_ids[f]);
				}
			}
			printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size());
			assert((shapes[i].mesh.positions.size() % 3) == 0);
			if (bExtraDebugInfo)
			{
				for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++)
				{
					printf("  v[%ld] = (%f, %f, %f)\n", v,
						shapes[i].mesh.positions[3 * v + 0],
						shapes[i].mesh.positions[3 * v + 1],
						shapes[i].mesh.positions[3 * v + 2]);
				}
			}
		}
		for (size_t i = 0; i < materials.size(); i++) {
			printf("material[%ld].name = %s\n", i, materials[i].name.c_str());
			printf("  material.Ka = (%f, %f ,%f)\n", materials[i].ambient[0], materials[i].ambient[1], materials[i].ambient[2]);
			printf("  material.Kd = (%f, %f ,%f)\n", materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2]);
			printf("  material.Ks = (%f, %f ,%f)\n", materials[i].specular[0], materials[i].specular[1], materials[i].specular[2]);
			printf("  material.Tr = (%f, %f ,%f)\n", materials[i].transmittance[0], materials[i].transmittance[1], materials[i].transmittance[2]);
			printf("  material.Ke = (%f, %f ,%f)\n", materials[i].emission[0], materials[i].emission[1], materials[i].emission[2]);
			printf("  material.Ns = %f\n", materials[i].shininess);
			printf("  material.Ni = %f\n", materials[i].ior);
			printf("  material.dissolve = %f\n", materials[i].dissolve);
			printf("  material.illum = %d\n", materials[i].illum);
			printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
			printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
			printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
			printf("  material.map_Ns = %s\n", materials[i].normal_texname.c_str());
			std::map<std::string, std::string>::const_iterator it(materials[i].unknown_parameter.begin());
			std::map<std::string, std::string>::const_iterator itEnd(materials[i].unknown_parameter.end());
			for (; it != itEnd; it++) {
				printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
			}
			printf("\n");
		}
	}
	return result;
}

void	LoadTexture(const char* a_szFileName, unsigned int &a_uiTextureID)
{
	int	iWidth;
	int	iHeight;
	int	iChannels;

	unsigned char*	data = stbi_load(a_szFileName, &iWidth, &iHeight, &iChannels, STBI_default);
	glGenTextures(1, &a_uiTextureID);
	glBindTexture(GL_TEXTURE_2D, a_uiTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iWidth, iHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
}

void	LoadAlphaTexture(const char* a_szFileName, unsigned int &a_uiTextureID)
{
	int	iWidth;
	int	iHeight;
	int	iChannels;

	unsigned char*	data = stbi_load(a_szFileName, &iWidth, &iHeight, &iChannels, STBI_default);
	glGenTextures(1, &a_uiTextureID);
	glBindTexture(GL_TEXTURE_2D, a_uiTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iWidth, iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
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
	TwEventMouseWheelGLFW((int)y);
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
