#pragma once
#include "glm\glm.hpp"
#include "gl_core_4_4.h"

bool
LoadShaderType(char* filename,GLenum shader_type,unsigned int* output);

bool
LoadShader(char* vertex_filename,char* geometry_filename,char* fragment_filename,GLuint* result);

struct BasicVertex
{
    glm::vec3 position;
    glm::vec2 texcoord;
    glm::vec3 normal;
};


unsigned int
BuildQuadGLVAO(float size);

void
RenderQuad(unsigned int  quad, unsigned int  shader, unsigned int  texture, glm::mat4 mvp);

unsigned int
CreateGLTextureBasic(unsigned char* data, int width, int height, int channels);