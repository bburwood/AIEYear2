#ifndef _UTILITY_H_
#define	_UTILITY_H_

#include "GLMHeader.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>

#include "AntTweakBar.h"

//typedef	unsigned int	GLuint;

bool	LoadShader(char* vertex_filename, char* geometry_filename, char* fragment_filename, GLuint* result);
bool	LoadShaderType(char* fileName, GLenum shaderType, unsigned int* output);

//	AntTweakBar helper functions
void	OnMouseButton(GLFWwindow* window, int button, int pressed, int mod_keys);
void	OnMousePosition(GLFWwindow* window, double x, double y);
void	OnMouseScroll(GLFWwindow* window, double x, double y);
void	OnKey(GLFWwindow* window, int key, int scancode, int pressed, int mod_keys);
void	OnChar(GLFWwindow* window, unsigned int c);
void	OnWindowResize(GLFWwindow* window, int width, int height);

//	to remove fpoen error, define
//	_CRT_SECURE_NO_WARNINGS

#endif // !_UTILITY_H_
