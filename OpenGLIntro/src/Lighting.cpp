#include "Lighting.h"
#include "GLMHeader.h"
#include "Vertex.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include "Utility.h"
//#include "FBXFile.h"

#include "../deps/FBXLoader/FBXFile.h"

Lighting::Lighting()
: m_uiRows(0), m_uiCols(0), m_timer(0)
{
}

Lighting::~Lighting()
{
}

bool	Lighting::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSetTime(0.0f);
	Gizmos::create();

	LoadShader("./shaders/lighting_vertex.glsl", "./shaders/lighting_fragment.glsl", &m_uiProgramID);
	//GenerateGrid(10, 10);


	m_FlyCamera = FlyCamera(vec3(10, 10, 10), vec3(0, 0, 0), glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);

	m_FBXfile = new FBXFile();
	m_FBXfile->load("./models/stanford/Bunny.fbx");

	CreateOpenGlBuffers(m_FBXfile);

	return true;
}

void	Lighting::shutdown()
{
	//	now clean up
	Gizmos::destroy();
}

bool	Lighting::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	//	check is we need to reload the shaders
	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
	{
		ReloadShader();
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

void	Lighting::draw()
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

	for (unsigned int mesh_index = 0; mesh_index < m_FBXfile->getMeshCount(); ++mesh_index)
	{
		FBXMeshNode* mesh = m_FBXfile->getMeshByIndex(mesh_index);
		unsigned int*	gl_data = (unsigned int*)mesh->m_userData;

		glBindVertexArray(gl_data[0]);
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}



	Application::draw();
	Gizmos::draw(m_FlyCamera.GetProjectionView());
	glfwSwapBuffers(m_window);
	glfwPollEvents();



}

void	Lighting::CreateOpenGlBuffers(FBXFile* a_file)
{
	for (unsigned int uiMeshIndex = 0; uiMeshIndex < a_file->getMeshCount(); ++uiMeshIndex)
	{
		FBXMeshNode*	mesh = a_file->getMeshByIndex(uiMeshIndex);
		//contains [0] is VAO, [1] is VBO and [2] is IBO
		unsigned int*	gl_data = new unsigned int[3];
		glGenVertexArrays(1, &gl_data[0]);
		glGenBuffers(2, &gl_data[1]);
		glBindVertexArray(gl_data[0]);

		glBindBuffer(GL_ARRAY_BUFFER, gl_data[1]);
		glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(FBXVertex), mesh->m_vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_data[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(unsigned int), mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);	//	positions
		glEnableVertexAttribArray(1);	//	normals

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), (void*)(FBXVertex::NormalOffset));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = gl_data;
	}
}

void	Lighting::CleanUpOpenGLBuffers(FBXFile* a_file)
{
	for (unsigned int uiMeshIndex = 0; uiMeshIndex < a_file->getMeshCount(); ++uiMeshIndex)
	{
		FBXMeshNode*	mesh = a_file->getMeshByIndex(uiMeshIndex);

		unsigned int*	uigl_data = (unsigned int*)mesh->m_userData;

		glDeleteVertexArrays(1, &uigl_data[0]);
		glDeleteBuffers(2, &uigl_data[1]);
		delete[]	uigl_data;
	}
}

void	Lighting::ReloadShader()
{
	glDeleteProgram(m_uiProgramID);
	LoadShader("./shaders/lighting_vertex.glsl", "./shaders/lighting_fragment.glsl", &m_uiProgramID);
}

