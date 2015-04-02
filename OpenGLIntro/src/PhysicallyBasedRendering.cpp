#include "PhysicallyBasedRendering.h"
#include "GLMHeader.h"
#include "Vertex.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include "Utility.h"
//#include "FBXFile.h"

//#include "../deps/FBXLoader/FBXFile.h"

PhysicallyBasedRendering::PhysicallyBasedRendering()
: m_uiRows(0), m_uiCols(0), m_timer(0)
{
}

PhysicallyBasedRendering::~PhysicallyBasedRendering()
{
}

bool	PhysicallyBasedRendering::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSetTime(0.0f);
	Gizmos::create();

	LoadShader("./shaders/BRDF_vertex.glsl", nullptr, "./shaders/BRDF_fragment.glsl", &m_uiProgramID);
	//GenerateGrid(10, 10);


	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	//std::string err = tinyobj::LoadObj(shapes, materials, "./models/stanford/Bunny.obj");
	//std::string err = tinyobj::LoadObj(shapes, materials, "./models/stanford/buddha.obj");
	//std::string err = tinyobj::LoadObj(shapes, materials, "./models/stanford/dragon.obj");
	std::string err = tinyobj::LoadObj(shapes, materials, "./models/soulspear/soulspear.obj");
	if (err.size() != 0)
	{
		return false;
	}

	m_FlyCamera = FlyCamera(vec3(10, 10, 10), vec3(0, 0, 0), glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);

	CreateOpenGlBuffers(shapes);

	ambient_light = vec3(0.1f, 0.1f, 0.1f);
	light_dir = vec3(0, -1, 0);
	light_colour = vec3(0.1f, 0.50f, 0.7f);
	material_colour = vec3(1.0f);
	specular_power = 15.0f;

	return true;
}

void	PhysicallyBasedRendering::shutdown()
{
	//	now clean up
	CleanUpOpenGLBuffers();
	Gizmos::destroy();
}

bool	PhysicallyBasedRendering::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	//	check if we need to reload the shaders
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

void	PhysicallyBasedRendering::draw()
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

	int	ambient_uniform = glGetUniformLocation(m_uiProgramID, "ambient_light");
	int	light_colour_uniform = glGetUniformLocation(m_uiProgramID, "light_colour");
	int	light_dir_uniform = glGetUniformLocation(m_uiProgramID, "light_dir");
	int	material_colour_uniform = glGetUniformLocation(m_uiProgramID, "material_colour");
	int	eye_pos_uniform = glGetUniformLocation(m_uiProgramID, "eye_pos");
	int	specular_uniform = glGetUniformLocation(m_uiProgramID, "specular_power");
	int	timer_uniform = glGetUniformLocation(m_uiProgramID, "timer");

	glUniform3fv(ambient_uniform, 1, (float*)&ambient_light);
	glUniform3fv(light_colour_uniform, 1, (float*)&light_colour);
	glUniform3fv(light_dir_uniform, 1, (float*)&light_dir);
	glUniform3fv(material_colour_uniform, 1, (float*)&material_colour);

	vec3	camera_pos = m_FlyCamera.m_worldTransform[3].xyz;
	glUniform3fv(eye_pos_uniform, 1, (float*)&camera_pos);
	glUniform1f(specular_uniform, specular_power);
	glUniform1f(timer_uniform, m_timer);

	//	uncomment the following line for wireframe ...
	//	else
	//	{
	//		glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);
	//	}

	for (unsigned int mesh_index = 0; mesh_index < m_GLData.size(); ++mesh_index)
	{
		glBindVertexArray(m_GLData[mesh_index].m_uiVAO);
		if (glfwGetKey(m_window, GLFW_KEY_M) == GLFW_PRESS)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		glDrawElements(GL_TRIANGLES, m_GLData[mesh_index].m_uiIndexCount, GL_UNSIGNED_INT, 0);
	}

	Application::draw();
	Gizmos::draw(m_FlyCamera.GetProjectionView());
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}
/*
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
*/

void	PhysicallyBasedRendering::CreateOpenGlBuffers(std::vector<tinyobj::shape_t> & a_shapes)
{
	m_GLData.resize(a_shapes.size());
	for (unsigned int uiShapeIndex = 0; uiShapeIndex < a_shapes.size(); ++uiShapeIndex)
	{
		std::vector<float>	vertexData;

		unsigned int float_count = a_shapes[uiShapeIndex].mesh.positions.size();
		float_count += a_shapes[uiShapeIndex].mesh.normals.size();
		vertexData.reserve(float_count);
		vertexData.insert(vertexData.end(), a_shapes[uiShapeIndex].mesh.positions.begin(), a_shapes[uiShapeIndex].mesh.positions.end());
		vertexData.insert(vertexData.end(), a_shapes[uiShapeIndex].mesh.normals.begin(), a_shapes[uiShapeIndex].mesh.normals.end());
		m_GLData[uiShapeIndex].m_uiIndexCount = a_shapes[uiShapeIndex].mesh.indices.size();

		glGenVertexArrays(1, &m_GLData[uiShapeIndex].m_uiVAO);
		glGenBuffers(1, &m_GLData[uiShapeIndex].m_uiVBO);
		glGenBuffers(1, &m_GLData[uiShapeIndex].m_uiIBO);

		glBindVertexArray(m_GLData[uiShapeIndex].m_uiVAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_GLData[uiShapeIndex].m_uiVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)* float_count, vertexData.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_GLData[uiShapeIndex].m_uiIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, a_shapes[uiShapeIndex].mesh.indices.size() * sizeof(unsigned int), a_shapes[uiShapeIndex].mesh.indices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);	//	positions
		glEnableVertexAttribArray(1);	//	normals

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, (void*)(sizeof(float)* a_shapes[uiShapeIndex].mesh.positions.size()));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

/*
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
*/

void	PhysicallyBasedRendering::CleanUpOpenGLBuffers()
{
	glDeleteProgram(m_uiProgramID);

	for (unsigned int i = 0; i < m_GLData.size(); ++i)
	{
		glDeleteVertexArrays(1, &m_GLData[i].m_uiVAO);
		glDeleteBuffers(1, &m_GLData[i].m_uiVBO);
		glDeleteBuffers(1, &m_GLData[i].m_uiIBO);
	}
}

void	PhysicallyBasedRendering::ReloadShader()
{
	glDeleteProgram(m_uiProgramID);
	LoadShader("./shaders/BRDF_vertex.glsl", nullptr, "./shaders/BRDF_fragment.glsl", &m_uiProgramID);
}

