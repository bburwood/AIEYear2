#include "AdvancedNavigation.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include "Utility.h"
#include "tiny_obj_loader.h"
#include <iostream>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

AdvancedNavigation::AdvancedNavigation()
{
}
AdvancedNavigation::~AdvancedNavigation()
{
}

bool	AdvancedNavigation::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	//	check if we need to reload the shaders
	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
	{
		ReloadShader();
	}

	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);
	//	setup callbacks to send info to AntTweakBar
	glfwSetMouseButtonCallback(m_window, OnMouseButton);
	glfwSetCursorPosCallback(m_window, OnMousePosition);
	glfwSetScrollCallback(m_window, OnMouseScroll);
	glfwSetKeyCallback(m_window, OnKey);
	glfwSetCharCallback(m_window, OnChar);
	glfwSetWindowSizeCallback(m_window, OnWindowResize);

	m_BackgroundColour = vec4(0.3f, 0.3f, 0.3f, 1.0f);
	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);
	//	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSetTime(0.0f);

	Gizmos::create();
	m_bDrawGizmos = true;

	//	now initialise the FlyCamera
	m_FlyCamera = FlyCamera(vec3(10, 10, 10), vec3(0, 0, 0), glm::radians(50.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);

	//	initialise basic AntTweakBar info
	m_bar = TwNewBar("Amazing new AntTweakBar!!");
	TwAddSeparator(m_bar, "Light Data", "");
	//	TwAddVarRW(m_bar, "Light Direction", TW_TYPE_DIR3F, &m_light_dir, "label='Group Light'");
	//	TwAddVarRW(m_bar, "Light Colour", TW_TYPE_COLOR3F, &m_light_colour, "label='Group Light'");
	//	TwAddVarRW(m_bar, "Spec Power", TW_TYPE_FLOAT, &m_fSpecular_power, "label='Group Light' min=0.05 max=100 step=0.05");
	TwAddSeparator(m_bar, "Misc Data", "");
	TwAddVarRW(m_bar, "Clear Colour", TW_TYPE_COLOR4F, &m_BackgroundColour, "");
	TwAddVarRW(m_bar, "Draw Gizmos", TW_TYPE_BOOL8, &m_bDrawGizmos, "");
	TwAddVarRO(m_bar, "FPS", TW_TYPE_FLOAT, &m_fFPS, "");

	m_SponzaMesh = LoadOBJ("./models/SponzaSimple.obj");
	//BuildNavMesh();
	LoadShader("shaders/navmesh_vert.glsl", 0, "shaders/navmesh_frag.glsl", &m_uiProgramID);
	return true;
}

void	AdvancedNavigation::shutdown()
{
	//	now clean up
	Gizmos::destroy();
	TwDeleteAllBars();
	TwTerminate();
}

bool	AdvancedNavigation::update()
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
	m_fFPS = (float)(1.0 / dT);
	//	now we get to the fun stuff
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::clear();
	Gizmos::addTransform(mat4(1));

	m_timer += dT;
	m_FlyCamera.update(dT);

	vec4	white(1);
	vec4	black(0, 0, 0, 1);
	vec4	blue(0, 0, 1, 1);
	vec4	yellow(1, 1, 0, 1);
	vec4	green(0, 1, 0, 1);
	vec4	red(1, 0, 0, 1);
	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
			i == 10 ? white : black);
	}

	//	debug draw the nav mesh
//	vec3 vRaise = vec3(0.0f, 0.05f, 0.0f);
//	for (unsigned int iNodeIndex = 0; iNodeIndex < m_aNavNodes.size(); ++iNodeIndex)
//	{
//		Gizmos::addAABB(m_aNavNodes[iNodeIndex].corners[0], vec3(0.1f), vec4(1, 0, 0, 1));
//		Gizmos::addAABB(m_aNavNodes[iNodeIndex].corners[1], vec3(0.1f), vec4(1, 0, 0, 1));
//		Gizmos::addAABB(m_aNavNodes[iNodeIndex].corners[2], vec3(0.1f), vec4(1, 0, 0, 1));
//
//		Gizmos::addAABB(m_aNavNodes[iNodeIndex].position, vec3(0.1f), vec4(0, 1, 0, 1));
//
//		Gizmos::addLine(m_aNavNodes[iNodeIndex].corners[0], m_aNavNodes[iNodeIndex].corners[1], vec4(1, 1, 0, 1));
//		Gizmos::addLine(m_aNavNodes[iNodeIndex].corners[1], m_aNavNodes[iNodeIndex].corners[2], vec4(1, 1, 0, 1));
//		Gizmos::addLine(m_aNavNodes[iNodeIndex].corners[2], m_aNavNodes[iNodeIndex].corners[0], vec4(1, 1, 0, 1));
//
//		for (unsigned int i = 0; i < 3; ++i)
//		{
//			if (m_aNavNodes[iNodeIndex].edges[i] != 0)
//			{
//				Gizmos::addLine(m_aNavNodes[iNodeIndex].position + vRaise, m_aNavNodes[iNodeIndex].edges[i]->position + vRaise, vec4(0, 0, 1, 1));
//			}
//		}
//	}


	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);
	return true;
}

void	AdvancedNavigation::draw()
{
	Application::draw();
	if (m_bDrawGizmos)
	{
		Gizmos::draw(m_FlyCamera.GetProjectionView());
	}

	glUseProgram(m_uiProgramID);
	int	iProjViewUniform = glGetUniformLocation(m_uiProgramID, "proj_view");
	glUniformMatrix4fv(iProjViewUniform, 1, GL_FALSE, (float*)&m_FlyCamera.m_projectionViewTransform);

	glBindVertexArray(m_SponzaMesh.m_uiVAO);
	glDrawElements(GL_TRIANGLES, m_SponzaMesh.m_uiIndexCount, GL_UNSIGNED_INT,  0);


	TwDraw();
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void	AdvancedNavigation::BuildNavMesh()
{
	//	Load our Nav mesh from the obj file
	std::vector<tinyobj::shape_t>	shapes;
	std::vector<tinyobj::material_t>	materials;

	std::string err = tinyobj::LoadObj(shapes, materials, "./models/SponzaSimpleNavMesh.obj");

	if (err.size() != 0)
	{
		std::cout << err << '\n';
		return;
	}
	if (shapes.size() != 1)
	{
		std::cout << "Obj did not have only a single mesh in it!\n";
		return;
	}

	tinyobj::mesh_t*	mesh = &shapes[0].mesh;
	//	resize our NavNodes vector to hold the right number of nodes
	unsigned int	uiIndexCount = mesh->indices.size();
	m_aNavNodes.resize(uiIndexCount / 3);

	//	loop through all the indices and build our nodes
	//	set the centres and corners
	for (unsigned int i = 0; i < uiIndexCount; i += 3)
	{
		unsigned int uiNavIndex = i / 3;
		//	tinyobj mesh positions are stored as a float array - with floats for x, y, and z stored one after the other
		//	first get the corners / vertices of the node
		for (unsigned int uiCorner = 0; uiCorner < 3; ++uiCorner)
		{
			unsigned int uiVertexIndex = mesh->indices[i + uiCorner];
			m_aNavNodes[uiNavIndex].corners[uiCorner].x = mesh->positions[uiVertexIndex * 3];
			m_aNavNodes[uiNavIndex].corners[uiCorner].y = mesh->positions[uiVertexIndex * 3 + 1];
			m_aNavNodes[uiNavIndex].corners[uiCorner].z = mesh->positions[uiVertexIndex * 3 + 2];
		}
		//	now set the centre point of the node
		m_aNavNodes[uiNavIndex].position = (m_aNavNodes[uiNavIndex].corners[0] +
											m_aNavNodes[uiNavIndex].corners[1] +
											m_aNavNodes[uiNavIndex].corners[2]) / 3.0f;
	}

	//	loop through all our nodes
		//	loop through all of them again
		//	look for matching edges
	for (auto& first : m_aNavNodes)
	{
		unsigned int	uiCurrEdge = 0;

		first.edges[0] = 0;
		first.edges[1] = 0;
		first.edges[2] = 0;
		for (auto& second : m_aNavNodes)
		{
			if (&first == &second)
			{
				continue;	//	we have the same node
			}
			for (unsigned int uiEI = 0; uiEI < 3; ++uiEI)
			{
				if ((first.corners[uiEI] == second.corners[0]) && (first.corners[(uiEI + 1) % 3] == second.corners[1]) ||
					(first.corners[uiEI] == second.corners[1]) && (first.corners[(uiEI + 1) % 3] == second.corners[2]) ||
					(first.corners[uiEI] == second.corners[2]) && (first.corners[(uiEI + 1) % 3] == second.corners[0]) ||

					(first.corners[uiEI] == second.corners[1]) && (first.corners[(uiEI + 1) % 3] == second.corners[0]) ||
					(first.corners[uiEI] == second.corners[2]) && (first.corners[(uiEI + 1) % 3] == second.corners[1]) ||
					(first.corners[uiEI] == second.corners[0]) && (first.corners[(uiEI + 1) % 3] == second.corners[2])
					)
				{
					//	make an edge from first to second
					first.edges[uiCurrEdge] = &second;
					first.edge_costs[uiCurrEdge] = glm::distance(second.position, first.position);
					++uiCurrEdge;
					break;
				}
			}
			if (uiCurrEdge == 3)
			{
				break;
			}
		}
	}
}

void	AdvancedNavigation::ReloadShader()
{
	glDeleteProgram(m_uiProgramID);
	LoadShader("shaders/navmesh_vert.glsl", 0, "shaders/navmesh_frag.glsl", &m_uiProgramID);
}
