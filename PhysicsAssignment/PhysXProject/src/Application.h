#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#define	GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

struct GLFWwindow;

const int	BUFFER_WIDTH = 1600;
const int	BUFFER_HEIGHT = 900;

class Application
{
public:
	Application();
	virtual	~Application();

	virtual	bool	startup();
	virtual	void	shutdown();
	virtual	bool	update();
	virtual	void	draw();
	GLFWwindow*		m_window;
};

#endif	//	_APPLICATION_H_
