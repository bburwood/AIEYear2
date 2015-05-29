#include "HeatmapProject.h"
#include <algorithm>
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"


bool HeatmapProject::startup()
{
    if (Application::startup() == false)
    {
        return false;
    }

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    Gizmos::create();

    m_camera = FlyCamera(1280.0f / 720.0f, 10.0f);
    m_camera.setLookAt(vec3(0, 15, 10), vec3(0, 0, 2), vec3(0, 1, 0));
    m_camera.sensitivity = 3;

	m_fPlayerSpeed = 5.0f;

	m_vPlayerPosition = vec3(0.0f , 0.6f, 0.0f);

	m_uiHeatmapHeight = m_uiHeatmapWidth = 512;
	m_pPositionHeatmap = heatmap_new(m_uiHeatmapWidth, m_uiHeatmapHeight);
	m_pPixelData = new unsigned char[m_uiHeatmapWidth * m_uiHeatmapHeight * 4];
	glGenBuffers(1, &m_uiTextureHandle);

	m_Quad = generateQuad(10.0f);

	LoadShader("./data/shaders/textured_vertex.glsl", 0, "./data/shaders/textured_fragment.glsl", &m_uiProgram);

    return true;
}

void HeatmapProject::shutdown()
{
    Gizmos::destroy();

    Application::shutdown();
}

bool HeatmapProject::update()
{
    if (Application::update() == false)
    {
        return false;
    }

    Gizmos::clear();

    float dt = (float)glfwGetTime();
    glfwSetTime(0.0);

    vec4 white(1);
    vec4 black(0, 0, 0, 1);
    for (int i = 0; i <= 20; ++i)
    {
        Gizmos::addLine(vec3(-10 + i, 0.01, -10), vec3(-10 + i, 0.01, 10),
            i == 10 ? white : black);
        Gizmos::addLine(vec3(-10, 0.01, -10 + i), vec3(10, 0.01, -10 + i),
            i == 10 ? white : black);
    }

    m_camera.update(dt);
	
	if (glfwGetKey(m_window, GLFW_KEY_I) == GLFW_PRESS)
	{
		m_vPlayerPosition.z -= m_fPlayerSpeed * dt;
	}
	if (glfwGetKey(m_window, GLFW_KEY_K) == GLFW_PRESS)
	{
		m_vPlayerPosition.z += m_fPlayerSpeed * dt;
	}
	if (glfwGetKey(m_window, GLFW_KEY_J) == GLFW_PRESS)
	{
		m_vPlayerPosition.x -= m_fPlayerSpeed * dt;
	}
	if (glfwGetKey(m_window, GLFW_KEY_L) == GLFW_PRESS)
	{
		m_vPlayerPosition.x += m_fPlayerSpeed * dt;
	}
	//	now constrain the player to the grid ...
	if (m_vPlayerPosition.z < -10.0f)
	{
		m_vPlayerPosition.z = -10.0f;
	}
	else if (m_vPlayerPosition.z > 10.0f)
	{
		m_vPlayerPosition.z = 10.0f;
	}
	if (m_vPlayerPosition.x < -10.0f)
	{
		m_vPlayerPosition.x = -10.0f;
	}
	else if (m_vPlayerPosition.x > 10.0f)
	{
		m_vPlayerPosition.x = 10.0f;
	}

	unsigned int	uiXPixel = (unsigned int)(((m_vPlayerPosition.x + 10) * m_uiHeatmapWidth - 0.5f) / 20.0f);
	unsigned int	uiZPixel = (unsigned int)(((m_vPlayerPosition.z + 10) * m_uiHeatmapHeight - 0.5f) / 20.0f);
	heatmap_add_point(m_pPositionHeatmap, uiXPixel, uiZPixel);
   
    return true;
}

void HeatmapProject::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	heatmap_render_default_to(m_pPositionHeatmap, m_pPixelData);
	glBindTexture(GL_TEXTURE_2D, m_uiTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_uiHeatmapWidth, m_uiHeatmapHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pPixelData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glUseProgram(m_uiProgram);
	glBindVertexArray(m_Quad.m_VAO);

	GLint	iProjViewUniform = glGetUniformLocation(m_uiProgram, "projection_view");
	GLint	iDiffuseUniform = glGetUniformLocation(m_uiProgram, "diffuse");
	glUniformMatrix4fv(iProjViewUniform, 1, GL_FALSE, (float*)&m_camera.view_proj);
	glUniform1i(iDiffuseUniform, 0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawElements(GL_TRIANGLES, m_Quad.m_index_count, GL_UNSIGNED_INT, 0);
	glDisable(GL_BLEND);

	Gizmos::addSphere(m_vPlayerPosition, 0.5f, 10, 10, vec4(0, 0, 1, 1));

	Gizmos::draw(m_camera.proj, m_camera.view);


	glfwSwapBuffers(m_window);
    glfwPollEvents();
}
