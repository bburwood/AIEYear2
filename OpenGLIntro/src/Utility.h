#ifndef _UTILITY_H_
#define	_UTILITY_H_

typedef	unsigned int	GLuint;

bool	LoadShaders(char* vertex_filename, char* fragment_filename, GLuint* result);
//	to remove fpoen error, define
//	_CRT_SECURE_NO_WARNINGS

#endif // !_UTILITY_H_
