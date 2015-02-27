#ifndef _UTILITY_H_
#define	_UTILITY_H_

typedef	unsigned int	GLuint;

bool	LoadShader(char* vertex_filename, char* geometry_filename, char* fragment_filename, GLuint* result);
bool	LoadShaderType(char* fileName, GLenum shaderType, unsigned int* output);

//	to remove fpoen error, define
//	_CRT_SECURE_NO_WARNINGS

#endif // !_UTILITY_H_
