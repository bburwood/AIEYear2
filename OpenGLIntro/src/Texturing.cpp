#include "Texturing.h"
#include "GLMHeader.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include "Utility.h"
#include "Vertex.h"

//#define	STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texturing::Texturing()
{
}

Texturing::~Texturing()
{
}

bool	Texturing::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSetTime(0.0f);
	Gizmos::create();

	LoadTexture("./textures/crate.png");
	GenerateQuad(5.0f);
	LoadShader("shaders/textured_vertex.glsl", "shaders/textured_fragment.glsl", &m_uiProgramID);

	m_FlyCamera = FlyCamera(vec3(10, 10, 10), vec3(0, 0, 0), glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
	return true;
}

void	Texturing::shutdown()
{
	Gizmos::destroy();
}

bool	Texturing::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	float	dT = (float)glfwGetTime();
	glfwSetTime(0.0f);

	///////////////////////////////////
	//	now we get to the fun stuff
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::clear();
	Gizmos::addTransform(mat4(1));

	m_timer += dT;
	m_FlyCamera.update(dT);



	return true;
}

void	Texturing::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_uiProgramID);

	//	draw the grid
	vec4	white(1);
	vec4	black(0, 0, 0, 1);
	vec4	blue(0, 0, 1, 1);
	vec4	yellow(1, 1, 0, 1);
	vec4	green(0, 1, 0, 1);
	vec4	red(1, 0, 0, 1);
	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, -0.01f, -10), vec3(-10 + i, -0.01f, 10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, -0.01f, -10 + i), vec3(10, -0.01f, -10 + i),
			i == 10 ? white : black);
	}

	int	iViewProjUniform = glGetUniformLocation(m_uiProgramID, "projection_view");
	glUniformMatrix4fv(iViewProjUniform, 1, GL_FALSE, (float*)&m_FlyCamera.GetProjectionView());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uiTextureID);
	int	iDiffuseLocation = glGetUniformLocation(m_uiProgramID, "diffuse");
	glUniform1i(iDiffuseLocation, 0);

	glBindVertexArray(m_uiVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	Application::draw();
	Gizmos::draw(m_FlyCamera.GetProjectionView());
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void	Texturing::GenerateGrid(unsigned int a_uiRows, unsigned int a_uiCols)
{
}

void	Texturing::LoadTexture(const char* a_szFileName)
{
	int	iWidth;
	int	iHeight;
	int	iChannels;

	unsigned char*	data = stbi_load(a_szFileName, &iWidth, &iHeight, &iChannels, STBI_default);
	glGenTextures(1, &m_uiTextureID);
	glBindTexture(GL_TEXTURE_2D, m_uiTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iWidth, iHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
}

void	Texturing::GenerateQuad(float a_fSize)
{
	VertexTexCoord	vertex_data[4];
	a_fSize *= 0.5f;	//	halve the size so that the quad is actual size passed in
	//	now generate the data
	vertex_data[0].position = vec4(-a_fSize, 0, -a_fSize, 1);
	vertex_data[1].position = vec4(-a_fSize, 0, a_fSize, 1);
	vertex_data[2].position = vec4(a_fSize, 0, a_fSize, 1);
	vertex_data[3].position = vec4(a_fSize, 0, -a_fSize, 1);

	vertex_data[0].tex_coord = vec2(0, 0);
	vertex_data[1].tex_coord = vec2(0, 1);
	vertex_data[2].tex_coord = vec2(1, 1);
	vertex_data[3].tex_coord = vec2(1, 0);

	unsigned int	index_data[6] = { 0, 2, 1, 0, 3, 2 };

	glGenVertexArrays(1, &m_uiVAO);
	glGenBuffers(1, &m_uiIBO);
	glGenBuffers(1, &m_uiVBO);

	glBindVertexArray(m_uiVAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTexCoord)* 4, vertex_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6, index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	//	position
	glEnableVertexAttribArray(1);	//	tex coord

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexTexCoord), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTexCoord), (void*)sizeof(vec4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}
