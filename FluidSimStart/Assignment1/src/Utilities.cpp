#include "Utilities.h"
#include <cstdio>

bool
LoadShaderType(char* filename,
GLenum shader_type,
unsigned int* output)
{
    //we want to be able to return if we succeded
    bool succeeded = true;

    //open the shader file
    FILE* shader_file;
    fopen_s(&shader_file, filename, "r");

    //did it open successfully
    if (shader_file == 0)
    {
        succeeded = false;
    }
    else
    {
        //find out how long the file is
        fseek(shader_file, 0, SEEK_END);
        int shader_file_length = ftell(shader_file);
        fseek(shader_file, 0, SEEK_SET);

        //allocate enough space for the file
        char *shader_source = new char[shader_file_length];

        //read the file and update the length to be accurate
        shader_file_length = (int)fread(shader_source, 1, shader_file_length, shader_file);

        //create the shader based on the type that got passed in
        unsigned int shader_handle = glCreateShader(shader_type);

        //compile the shader
        glShaderSource(shader_handle, 1, &shader_source, &shader_file_length);
        glCompileShader(shader_handle);

        //chech the shader for errors
        int success = GL_FALSE;
        glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE)
        {
            int log_length = 0;
            glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &log_length);
            char* log = new char[log_length];
            glGetShaderInfoLog(shader_handle, log_length, NULL, log);
            printf("%s\n", log);
            delete[] log;
            succeeded = false;
        }
        //only give the result to the caller if we succeeded
        if (succeeded)
        {
            *output = shader_handle;
        }

        //clean up the stuff we allocated
        delete[] shader_source;
        fclose(shader_file);
    }

    return succeeded;
}

bool
LoadShader(
char* vertex_filename,
char* geometry_filename,
char* fragment_filename,
GLuint* result)
{
    bool succeeded = true;

    *result = glCreateProgram();

    unsigned int vertex_shader;

    if (LoadShaderType(vertex_filename, GL_VERTEX_SHADER, &vertex_shader))
    {
        glAttachShader(*result, vertex_shader);
        glDeleteShader(vertex_shader);
    }
    else
    {
        printf("FAILED TO LOAD VERTEX SHADER\n");
    }

    if (geometry_filename != nullptr)
    {
        unsigned int geometry_shader;
        if (LoadShaderType(geometry_filename, GL_GEOMETRY_SHADER, &geometry_shader))
        {
            glAttachShader(*result, geometry_shader);
            glDeleteShader(geometry_shader);
        }
        else
        {
            printf("FAILED TO LOAD GEOMETRY SHADER\n");
        }
    }
    if (fragment_filename != nullptr)
    {
        unsigned int fragment_shader;
        if (LoadShaderType(fragment_filename, GL_FRAGMENT_SHADER, &fragment_shader))
        {
            glAttachShader(*result, fragment_shader);
            glDeleteShader(fragment_shader);
        }
        else
        {
            printf("FAILED TO LOAD FRAGMENT SHADER\n");
        }
    }

    glLinkProgram(*result);

    GLint success;
    glGetProgramiv(*result, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint log_length;
        glGetProgramiv(*result, GL_INFO_LOG_LENGTH, &log_length);
        char* log = new char[log_length];
        glGetProgramInfoLog(*result, log_length, 0, log);

        printf("ERROR: STUFF DONE SCREWED UP IN UR SHADER BUDDY!\n\n");
        printf("%s", log);

        delete[] log;
        succeeded = false;
    }

    return succeeded;
}

unsigned int
BuildQuadGLVAO(float size)
{
    BasicVertex verts[4] =
    {
        { glm::vec3(size, size, 0), glm::vec2(1, 1), glm::vec3(0, 1, 0) },
        { glm::vec3(size, -size, 0), glm::vec2(1, 0), glm::vec3(0, 1, 0) },
        { glm::vec3(-size, size, 0), glm::vec2(0, 1), glm::vec3(0, 1, 0) },
        { glm::vec3(-size, -size, 0), glm::vec2(0, 0), glm::vec3(0, 1, 0) },
    };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        sizeof(BasicVertex), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
        sizeof(BasicVertex), (void*)sizeof(glm::vec3));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vao;
}

void
RenderQuad(unsigned int  quad, unsigned int  shader, unsigned int  texture, glm::mat4 mvp)
{
    glUseProgram(shader);

    glm::mat4 ident(1);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&ident);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view_proj"), 1, GL_FALSE, (float*)&mvp);

    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(quad);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    glBindVertexArray(0);
}


unsigned int
CreateGLTextureBasic(unsigned char* data, int width, int height, int channels)
{
    unsigned int tex_handle;
    glGenTextures(1, (GLuint*)&tex_handle);
    glBindTexture(GL_TEXTURE_2D, tex_handle);

    GLenum format = 0;
    switch (channels)
    {
    case 1:
    {
        format = GL_RED;
    }break;
    case 2:
    {
        format = GL_RG;
    }break;
    case 3:
    {
        format = GL_RGB;
    }break;
    case 4:
    {
        format = GL_RGBA;
    }break;
    default:
        break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    return tex_handle;
}
