#include "AdvancedTexturing.h"
#include "GLMHeader.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include "Utility.h"
#include "stb_image.h"
#include <vector>

AdvancedTexturing::AdvancedTexturing()
{
}

AdvancedTexturing::~AdvancedTexturing()
{
}

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
	TwEventMouseWheelGLFW(y);
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


bool	AdvancedTexturing::startup()
{
	if (Application::startup() == false)
	{
		return false;
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
	glEnable(GL_DEPTH_TEST);
	glfwSetTime(0.0f);
	Gizmos::create();
	m_bDrawGizmos = true;

	LoadShader("./shaders/normal_mapped_vertex.glsl", nullptr, "./shaders/normal_mapped_fragment.glsl", &m_uiProgramID);
	GenerateQuad(5.0f);
	//GenerateCircle(2.0f, 10, 10);	//	not working yet ...
	m_ambient_light = vec3(0.1f, 0.1f, 0.1f);
	m_light_dir = vec3(0, -1, 0);
	m_light_colour = vec3(0.1f, 0.50f, 0.7f);
	m_material_colour = vec3(1.0f);
	m_fSpecular_power = 15.0f;

	m_FlyCamera = FlyCamera(vec3(10, 10, 10), vec3(0, 0, 0), glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
	LoadTextures();

	m_bar = TwNewBar("Amazing new AntTweakBar!!");
	TwAddSeparator(m_bar, "Light Data", "");
	TwAddVarRW(m_bar, "Light Direction", TW_TYPE_DIR3F, &m_light_dir, "Group Light");
	TwAddVarRW(m_bar, "Light Colour", TW_TYPE_COLOR3F, &m_light_colour, "Group Light");
	TwAddVarRW(m_bar, "Spec Power", TW_TYPE_FLOAT, &m_fSpecular_power, "Group Light min=0.05 max=100 step=0.05");
	TwAddSeparator(m_bar, "Misc Data", "");
	TwAddVarRW(m_bar, "Clear Colour", TW_TYPE_COLOR4F, &m_BackgroundColour, "");
	TwAddVarRW(m_bar, "Draw Gizmos", TW_TYPE_BOOL8, &m_bDrawGizmos, "");
	TwAddVarRO(m_bar, "FPS", TW_TYPE_FLOAT, &m_fFPS, "");

	return true;
}

void	AdvancedTexturing::shutdown()
{
	Gizmos::destroy();
	TwDeleteAllBars();
	TwTerminate();
	glDeleteProgram(m_uiProgramID);
}

bool	AdvancedTexturing::update()
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

	///////////////////////////////////
	//	now we get to the fun stuff
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::clear();
	Gizmos::addTransform(mat4(1));

	m_timer += dT;
	//m_light_dir = (glm::rotate(dT, vec3(0, 1, 0)) * vec4(m_light_dir, 0)).xyz;
	m_FlyCamera.update(dT);

	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);

	return true;
}

void	AdvancedTexturing::draw()
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

	glUniform3fv(ambient_uniform, 1, (float*)&m_ambient_light);
	glUniform3fv(light_colour_uniform, 1, (float*)&m_light_colour);
	glUniform3fv(light_dir_uniform, 1, (float*)&m_light_dir);
	glUniform3fv(material_colour_uniform, 1, (float*)&m_material_colour);

	vec3	camera_pos = m_FlyCamera.m_worldTransform[3].xyz;
	glUniform3fv(eye_pos_uniform, 1, (float*)&camera_pos);
	glUniform1f(specular_uniform, m_fSpecular_power);
	glUniform1f(timer_uniform, m_timer);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uiDiffuseTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_uiNormalTexture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_uiSpecularTexture);

	int	diffuse_location = glGetUniformLocation(m_uiProgramID, "diffuse_tex");
	int	normal_location = glGetUniformLocation(m_uiProgramID, "normal_tex");
	int	specular_location = glGetUniformLocation(m_uiProgramID, "specular_tex");

	glUniform1i(diffuse_location, 0);
	glUniform1i(normal_location, 1);
	glUniform1i(specular_location, 2);

	//	uncomment the following line for wireframe ...
	//	else
	//	{
	//		glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);
	//	}

	glBindVertexArray(m_quad.m_uiVAO);
//	if (glfwGetKey(m_window, GLFW_KEY_M) == GLFW_PRESS)
//	{
//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	}
//	else
//	{
//		//ReloadShader();
//	}
	glDrawElements(GL_TRIANGLES, m_quad.m_uiIndexCount, GL_UNSIGNED_INT, 0);

	Application::draw();
	if (m_bDrawGizmos)
	{
		Gizmos::draw(m_FlyCamera.GetProjectionView());
	}
	TwDraw();
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void	AdvancedTexturing::LoadTextures()
{
	int	width, height, channels;
	unsigned char*	data;

	data = stbi_load("./textures/rock_diffuse.tga", &width, &height, &channels, STBI_default);
	glGenTextures(1, &m_uiDiffuseTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiDiffuseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);

	data = stbi_load("./textures/rock_normal.tga", &width, &height, &channels, STBI_default);
	glGenTextures(1, &m_uiNormalTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiNormalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);

	data = stbi_load("./textures/rock_specular.tga", &width, &height, &channels, STBI_default);
	glGenTextures(1, &m_uiSpecularTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiSpecularTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);

}

void	AdvancedTexturing::GenerateQuad(float a_fSize)
{
	VertexNormal	vertex_data[4];
	a_fSize *= 0.5f;	//	halve the size so that the quad is actual size passed in
	//	now generate the data
	vertex_data[0].position = vec4(-a_fSize, 0, -a_fSize, 1);
	vertex_data[1].position = vec4(-a_fSize, 0, a_fSize, 1);
	vertex_data[2].position = vec4(a_fSize, 0, a_fSize, 1);
	vertex_data[3].position = vec4(a_fSize, 0, -a_fSize, 1);

	vertex_data[0].normal = vec4(0, 1, 0, 0);
	vertex_data[1].normal = vec4(0, 1, 0, 0);
	vertex_data[2].normal = vec4(0, 1, 0, 0);
	vertex_data[3].normal = vec4(0, 1, 0, 0);

	vertex_data[0].tangent = vec4(1, 0, 0, 0);
	vertex_data[1].tangent = vec4(1, 0, 0, 0);
	vertex_data[2].tangent = vec4(1, 0, 0, 0);
	vertex_data[3].tangent = vec4(1, 0, 0, 0);

	vertex_data[0].tex_coord = vec2(0, 0);
	vertex_data[1].tex_coord = vec2(0, 1);
	vertex_data[2].tex_coord = vec2(1, 1);
	vertex_data[3].tex_coord = vec2(1, 0);

	unsigned int	index_data[6] = { 0, 2, 1, 0, 3, 2 };
	m_quad.m_uiIndexCount = 6;

	glGenVertexArrays(1, &m_quad.m_uiVAO);
	glGenBuffers(1, &m_quad.m_uiIBO);
	glGenBuffers(1, &m_quad.m_uiVBO);

	glBindVertexArray(m_quad.m_uiVAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_quad.m_uiVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexNormal)* 4, vertex_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quad.m_uiIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6, index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	//	position
	glEnableVertexAttribArray(1);	//	normal
	glEnableVertexAttribArray(2);	//	tangent
	glEnableVertexAttribArray(3);	//	texture coord

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), 0);	//	position
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(VertexNormal), (void*)(sizeof(vec4)));	//	normal
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(VertexNormal), (void*)(sizeof(vec4) * 2));	//	tangent
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (void*)(sizeof(vec4) * 3));	//	texture coord

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void	AdvancedTexturing::ReloadShader()
{
	glDeleteProgram(m_uiProgramID);
	LoadShader("./shaders/normal_mapped_vertex.glsl", nullptr, "./shaders/normal_mapped_fragment.glsl", &m_uiProgramID);
}

void	AdvancedTexturing::GenerateCircle(float radius, int rows, int cols)
{
	OpenGLData	result = {};
	VertexTexCoord*	verts = new VertexTexCoord[cols + 1];
	verts[0].position = vec4(0, 0, 0, 0);
	verts[0].tex_coord = vec2(0.5, 0.5);

	for (unsigned int i = 0; i < cols; ++i)
	{
		verts[i + 1].position = vec4((i / (float)cols) * 2.0f * 3.141592654, 0,
									(i / (float)cols) * 2.0f * 3.141592654, 1);
		verts[i + 1].tex_coord = vec2(verts[i + 1].position.x + 0.5f, verts[i + 1].position.y + 0.5f);
	}
	unsigned int*	indices = new unsigned int[3 * cols];
	for (unsigned int i = 0; i < cols; ++i)
	{
		indices[i * 3] = 0;
		indices[i * 3 + 1] = i + 1;
		indices[i * 3 + 2] = i + 2;
	}

	glGenVertexArrays(1, &m_quad.m_uiVAO);
	glGenBuffers(1, &m_quad.m_uiIBO);
	glGenBuffers(1, &m_quad.m_uiVBO);

	glBindVertexArray(m_quad.m_uiVAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_quad.m_uiVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexNormal)* 4, verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quad.m_uiIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6, indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	//	position
	glEnableVertexAttribArray(1);	//	normal
	glEnableVertexAttribArray(2);	//	tangent
	glEnableVertexAttribArray(3);	//	texture coord

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), 0);	//	position
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(VertexNormal), (void*)(sizeof(vec4)));	//	normal
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(VertexNormal), (void*)(sizeof(vec4)* 2));	//	tangent
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (void*)(sizeof(vec4)* 3));	//	texture coord

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
