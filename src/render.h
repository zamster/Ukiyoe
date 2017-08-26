#ifndef RENDER_H_
#define RENDER_H_

#include <GL/glew.h>

#include "shader.hpp"

#include "voxel.h"
#include <vector>

class OGLRenderer
{
    ShaderProgram *ukiyoeShader;

    GLuint vao;

    GLuint ibo_cube_elements;
    GLuint vertices_buffer;
    GLuint normals_buffer;

    /*
        Per Instance
    */
    GLuint models_buffer;
    GLuint colors_buffer;

    // BG
    ShaderProgram *bgShader;
    GLuint bgTexture;
    GLuint bgVao;
    GLuint bg_vertices_buffer;
    GLuint bg_texture_buffer;

public:
    OGLRenderer();
    ~OGLRenderer();

    void update(std::vector<Voxel *> node);
    void render(std::vector<Voxel *> node);
};

#endif