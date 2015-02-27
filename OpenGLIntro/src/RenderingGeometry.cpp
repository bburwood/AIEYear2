#include "RenderingGeometry.h"
#include "GLMHeader.h"
#include "Vertex.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include "Utility.h"

RenderingGeometry::RenderingGeometry()
: m_uiRows(0), m_uiCols(0), m_timer(0)
{
}

RenderingGeometry::~RenderingGeometry()
{
}

bool	RenderingGeometry::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSetTime(0.0f);
	Gizmos::create();

	LoadShader("./shaders/basic_vertex.glsl", nullptr, "./shaders/basic_fragment.glsl", &m_uiProgramID);
	GenerateGrid(10, 10);


	m_FlyCamera = FlyCamera(vec3(10, 10, 10), vec3(0, 0, 0), glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
	return true;
}

void	RenderingGeometry::shutdown()
{
	//	now clean up
	Gizmos::destroy();
}

bool	RenderingGeometry::update()
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
	//	messes with the view
	//m_camera_x = sinf(0.3f * m_timer) * 10;
	//m_camera_z = cosf(0.2f * m_timer) * 10;
	//
	//m_view = glm::lookAt(vec3(m_camera_x, 10, m_camera_z), vec3(0, 0, 0), vec3(0, 1, 0));
	m_FlyCamera.update(dT);

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

	///////////////////////////////////

	return true;
}

void	RenderingGeometry::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_uiProgramID);

	int	iProjViewHandle = glGetUniformLocation(m_uiProgramID, "projection_view");
	if (iProjViewHandle >= 0)
	{
		glUniformMatrix4fv(iProjViewHandle, 1, false, (float*)&m_FlyCamera.GetProjectionView());
	}
	int	iTimerHandle = glGetUniformLocation(m_uiProgramID, "timer");
	if (iTimerHandle >= 0)
	{
		glUniform1f(iTimerHandle, m_timer);
	}

	glBindVertexArray(m_uiVAO);
	unsigned int indexCount = (m_uiRows) * (m_uiCols) * 6;
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	//	uncomment the following to make it just draw the triangles unfilled
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	Application::draw();
	Gizmos::draw(m_FlyCamera.GetProjectionView());
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void	RenderingGeometry::GenerateGrid(unsigned int a_uiRows, unsigned int a_uiCols)
{
	m_uiRows = a_uiRows;
	m_uiCols = a_uiCols;

	Vertex*	vertex_array = new Vertex[(a_uiRows + 1) * (a_uiCols + 1)];
	for (unsigned int r = 0; r < a_uiRows + 1; ++r)
	{
		for (unsigned int c = 0; c < a_uiCols +  1; ++c)
		{
			vertex_array[c + (r * (a_uiCols + 1))].position = vec4((float)c, 0, (float)r, 1);
			vertex_array[c + (r * (a_uiCols + 1))].colour = vec4((float)c / (a_uiCols + 1), 0, (float)r / (a_uiRows + 1), 1);
		}
	}

	unsigned int*	index_array = new unsigned int[a_uiRows * a_uiCols * 6];
	int	index_location = 0;
	for (unsigned int r = 0; r < a_uiRows; ++r)
	{
		for (unsigned int c = 0; c < a_uiCols; ++c)
		{
			index_array[index_location] = c + (r * (a_uiCols + 1));
			index_array[index_location + 1] = c + ((r + 1) * (a_uiCols + 1));
			index_array[index_location + 2] = c + 1 + (r * (a_uiCols + 1));
			index_array[index_location + 3] = c + 1 + (r * (a_uiCols + 1));
			index_array[index_location + 4] = c + ((r + 1) * (a_uiCols + 1));
			index_array[index_location + 5] = c + ((r + 1) * (a_uiCols + 1)) + 1;

			index_location += 6;
		}
	}

	m_uiIndexCount = a_uiRows * a_uiCols * 6;

	glGenBuffers(1, &m_uiVBO);
	glGenBuffers(1, &m_uiIBO);
	glGenVertexArrays(1, &m_uiVAO);
	glBindVertexArray(m_uiVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);
	glBufferData(GL_ARRAY_BUFFER, (a_uiCols + 1) * (a_uiRows + 1) * sizeof(Vertex), vertex_array, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);	//	position
	glEnableVertexAttribArray(1);	//	colour
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec4));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, a_uiRows * a_uiCols * 6 * sizeof(unsigned int), index_array, GL_STATIC_DRAW);
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] vertex_array;
	delete[] index_array;
}

void	RenderingGeometry::GenerateShader()
{
	//	define our shaders
	const char*	vs_source =
		"#version 410\n"
		"layout(location = 0) in vec4 position;\n"
		"layout(location = 1) in vec4 colour;\n"
		"out vec4 out_colour;\n"
		"uniform mat4 projection_view;\n"
		"uniform float time;\n"
		"void main()\n"
		"{\n"
		"	out_colour = colour;\n"
		"	vec4 pos = position;\n"
		"	pos.y += sin(time + pos.x) * 0.5f;\n"
		";\n"
		"	gl_Position = projection_view * pos;\n"
		"};\n";
	const char*	fs_source = 
		"#version 410\n"
		"in vec4 out_colour;\n"
		"out vec4 frag_colour;\n"
		"void main()\n"
		"{\n"
		"	frag_colour = out_colour;\n"
		"};\n";

	unsigned int	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertex_shader, 1, &vs_source, 0);
	glCompileShader(vertex_shader);
	glShaderSource(fragment_shader, 1, &fs_source, 0);
	glCompileShader(fragment_shader);
	m_uiProgramID = glCreateProgram();
	glAttachShader(m_uiProgramID, vertex_shader);
	glAttachShader(m_uiProgramID, fragment_shader);
	glLinkProgram(m_uiProgramID);

	//	error checking now ...
	int	success = GL_FALSE;
	glGetProgramiv(m_uiProgramID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		//	then we had a problem
		int	log_length = 0;
		glGetProgramiv(m_uiProgramID, GL_INFO_LOG_LENGTH, &log_length);
		char*	log = new char[log_length];
		glGetProgramInfoLog(m_uiProgramID, log_length, 0, log);

		printf("ERROR: Shader compile failed!!\nError Log:\n");
		printf("%s", log);
		delete[] log;
	}

	glDeleteShader(fragment_shader);
	glDeleteShader(vertex_shader);
}
