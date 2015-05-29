#ifndef SOUND_PROGRAMMING_H_
#define SOUND_PROGRAMMING_H_

#include "Application.h"
#include "Camera.h"
#include "heatmap.h"
#include "Utility.h"

class HeatmapProject : public Application
{
public:
    virtual bool startup();
    virtual void shutdown();
    virtual bool update();
    virtual void draw();

    FlyCamera m_camera;

	vec3	m_vPlayerPosition;
	heatmap_t*	m_pPositionHeatmap;
	float	m_fPlayerSpeed;
	unsigned int	m_uiHeatmapWidth;
	unsigned int	m_uiHeatmapHeight;
	unsigned int	m_uiTextureHandle;
	unsigned int	m_uiQuadVAO;

	unsigned char*	m_pPixelData;
	unsigned int	m_uiProgram;
	OpenGLData		m_Quad;

};

#endif //CAM_PROJ_H_