#include "Shadows.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include "Utility.h"
#include "tiny_obj_loader.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

Shadows::Shadows()
{
}
Shadows::~Shadows()
{
}

bool	Shadows::startup()
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


	LoadShader("shaders/diffuse_shadow_vertex.glsl", 0, "shaders/diffuse_shadowed_fragment.glsl", &m_uiDiffuseProgramID);
	LoadShader("shaders/shadowmap_vertex.glsl", 0, "shaders/shadowmap_fragment.glsl", &m_uiShadowMapProgramID);
	BuildMeshes();
	BuildShadowMap();

	return true;
}

void	Shadows::shutdown()
{
	//	now clean up
	Gizmos::destroy();
	TwDeleteAllBars();
	TwTerminate();
}

bool	Shadows::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	//	check if we need to reload the shaders
	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
	{
		//ReloadShader();
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




	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);
	return true;
}

void	Shadows::draw()
{
	Application::draw();

	glBindFramebuffer(GL_FRAMEBUFFER, m_uiFBO);
	glViewport(0, 0, 1024, 1024);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_uiShadowMapProgramID);
	int	iLightMatrixLocation = glGetUniformLocation(m_uiShadowMapProgramID, "light_matrix");

	vec3	lightDir = glm::normalize(vec3(-1, -2.5f, -1));

	mat4	lightProj = glm::ortho(-10, 10, -10, 10, -10, 100);
	mat4	lightView = glm::lookAt(-lightDir, vec3(0), vec3(0, 1, 0));

	mat4	lightMatrix = lightProj * lightView;

	glUniformMatrix4fv(iLightMatrixLocation, 1, GL_FALSE, (float*)&lightMatrix);

	glBindVertexArray(m_Bunny.m_uiVAO);
	glDrawElements(GL_TRIANGLES, m_Bunny.m_uiIndexCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(m_plane.m_uiVAO);
	glDrawElements(GL_TRIANGLES, m_plane.m_uiIndexCount, GL_UNSIGNED_INT, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_uiDiffuseProgramID);	//	check this program id

	mat4	offsetScale = mat4(
		0.5f,	0,	0,	0,
		0,	0.5f,	0,	0,
		0,	0,	0.5f,	0,
		0.5f,	0.5f,	0.5f,	1 );


	if (m_bDrawGizmos)
	{
		Gizmos::draw(m_FlyCamera.GetProjectionView());
	}

	glUseProgram(m_uiDiffuseProgramID);
	int	viewProjUniform = glGetUniformLocation(m_uiDiffuseProgramID, "view_projection");
	glUniformMatrix4fv(viewProjUniform, 1, GL_FALSE, (float*)&m_FlyCamera.GetProjectionView());

	int	iLightDirectionUniform = glGetUniformLocation(m_uiDiffuseProgramID, "light_dir");
	glUniform3fv(iLightDirectionUniform, 1, (float*)&lightDir);

	int iLightMatUniform = glGetUniformLocation(m_uiDiffuseProgramID, "light_matrix");
	glUniformMatrix4fv(iLightMatUniform, 1, GL_FALSE, (float*)&lightMatrix);

	int iShadowMapLocation = glGetUniformLocation(m_uiDiffuseProgramID, "shadow_map");
	glUniform1i(iShadowMapLocation, 0);

	glBindVertexArray(m_Bunny.m_uiVAO);
	glDrawElements(GL_TRIANGLES, m_Bunny.m_uiIndexCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(m_plane.m_uiVAO);
	glDrawElements(GL_TRIANGLES, m_plane.m_uiIndexCount, GL_UNSIGNED_INT, 0);



	TwDraw();
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void	Shadows::BuildMeshes()
{
	std::vector<tinyobj::shape_t>	shapes;
	std::vector<tinyobj::material_t>	materials;

	tinyobj::LoadObj(shapes, materials, "./models/stanford/bunny.obj");

	m_Bunny.m_uiIndexCount = shapes[0].mesh.indices.size();

	tinyobj::mesh_t*	mesh = &shapes[0].mesh;

	std::vector<float>	vertexData;
	vertexData.reserve(mesh->positions.size() + mesh->normals.size());

	vertexData.insert(vertexData.end(), mesh->positions.begin(), mesh->positions.end());
	vertexData.insert(vertexData.end(), mesh->normals.begin(), mesh->normals.end());

	glGenVertexArrays(1, &m_Bunny.m_uiVAO);
	glBindVertexArray(m_Bunny.m_uiVAO);

	glGenBuffers(1, &m_Bunny.m_uiVBO);
	glGenBuffers(1, &m_Bunny.m_uiIBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_Bunny.m_uiVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Bunny.m_uiIBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexData.size(), vertexData.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->indices.size(), mesh->indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	//	position
	glEnableVertexAttribArray(1);	//	normal
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * mesh->positions.size()));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//	plane
	glGenVertexArrays(1, &m_plane.m_uiVAO);
	glBindVertexArray(m_plane.m_uiVAO);
	glGenBuffers(1, &m_plane.m_uiVBO);
	glGenBuffers(1, &m_plane.m_uiIBO);

	float	planeVertexData[] =
	{
		-10, 0, -10, 1,	0, 1, 0, 0,
		10, 0, -10, 1,	0, 1, 0, 0,
		10, 0, 10, 1,	0, 1, 0, 0,
		-10, 0, 10, 1,	0, 1, 0, 0,
	};
	unsigned int	planeIndexData[] =
	{
		0, 1, 2, 0, 2, 3
	};
	m_plane.m_uiIndexCount = 6;

	glBindBuffer(GL_ARRAY_BUFFER, m_plane.m_uiVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_plane.m_uiIBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertexData), planeVertexData, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndexData), planeIndexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void	Shadows::BuildShadowMap()
{
	glGenFramebuffers(1, &m_uiFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_uiFBO);

	glGenTextures(1, &m_uiFBODepth);
	glBindTexture(GL_TEXTURE_2D, m_uiFBODepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_uiFBODepth, 0);

	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void	Shadows::ReloadShader()
{
	glDeleteProgram(m_uiProgramID);
	LoadShader("shaders/diffuse_shadow_vertex.glsl", 0, "shaders/diffuse_shadowed_fragment.glsl", &m_uiDiffuseProgramID);
	LoadShader("shaders/shadowmap_vertex.glsl", 0, "shaders/shadowmap_fragment.glsl", &m_uiShadowMapProgramID);
}
