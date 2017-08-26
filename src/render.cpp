/*
    OpenGL 4 Renderer, MAX 512 * 512 Voxels
*/

#include "render.h"
#include "util.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "camera.h"
extern Camera *camera;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static const GLfloat squareVertices[] = {
    -1.0f, -1.0f,
     1.0f, -1.0f,
    -1.0f,  1.0f,
     1.0f,  1.0f,
};

static const GLfloat textureVertices[] = {
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
};

OGLRenderer::OGLRenderer()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.8, 0.8, 0.8, 1);

    /*
        Init Ukiyoe Shader and bind address
    */

    /* VAO */
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Set up vertex shader
    Shader vertexShader(GL_VERTEX_SHADER);
    vertexShader.loadFromFile("ukiyoe_vertex.glsl");
    vertexShader.compile();

    // Set up fragment shader
    Shader fragmentShader(GL_FRAGMENT_SHADER);
    fragmentShader.loadFromFile("ukiyoe_fragment.glsl");
    fragmentShader.compile();

    // Set up shader program
    ukiyoeShader = new ShaderProgram();
    ukiyoeShader->attachShader(vertexShader);
    ukiyoeShader->attachShader(fragmentShader);
    ukiyoeShader->linkProgram();

    ukiyoeShader->addAttribute("vertex");
    ukiyoeShader->addAttribute("normal");
    ukiyoeShader->addAttribute("model_color");
    ukiyoeShader->addAttribute("model_matrix");

    ukiyoeShader->addUniform("view_matrix");
    ukiyoeShader->addUniform("projection_matrix");

    /* IBO */
    glGenBuffers(1, &ibo_cube_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

    /* VBO */
    glGenBuffers(1, &vertices_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(ukiyoeShader->attribute("vertex"));
    glVertexAttribPointer(ukiyoeShader->attribute("vertex"), 3, GL_FLOAT, GL_FALSE, 0, NULL);

    /* VBO */
    glGenBuffers(1, &normals_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, normals_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(ukiyoeShader->attribute("normal"));
    glVertexAttribPointer(ukiyoeShader->attribute("normal"), 3, GL_FLOAT, GL_FALSE, 0, NULL);

    /*
        Instance Rendering Data Buffer
     */

    /* VBO */
    glGenBuffers(1, &colors_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, colors_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * 512 * 512, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(ukiyoeShader->attribute("model_color"));
    glVertexAttribPointer(ukiyoeShader->attribute("model_color"), 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribDivisor(ukiyoeShader->attribute("model_color"), 1);

    /* VBO */
    glGenBuffers(1, &models_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, models_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * 512 * 512, NULL, GL_DYNAMIC_DRAW);

    // Loop over each column of the matrix...
    for (int i = 0; i < 4; i++)
    {
        glEnableVertexAttribArray(ukiyoeShader->attribute("model_matrix") + i);
        glVertexAttribPointer(ukiyoeShader->attribute("model_matrix") + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(ukiyoeShader->attribute("model_matrix") + i, 1);
    }

    /* VAO */
    glGenVertexArrays(1, &bgVao);
    glBindVertexArray(bgVao);

    // Set up vertex shader
    Shader bg_vertexShader(GL_VERTEX_SHADER);
    bg_vertexShader.loadFromFile("bg_vertex.glsl");
    bg_vertexShader.compile();

    // Set up fragment shader
    Shader bg_fragmentShader(GL_FRAGMENT_SHADER);
    bg_fragmentShader.loadFromFile("bg_fragment.glsl");
    bg_fragmentShader.compile();

    // Set up shader program
    bgShader = new ShaderProgram();
    bgShader->attachShader(bg_vertexShader);
    bgShader->attachShader(bg_fragmentShader);
    bgShader->linkProgram();

    bgShader->addAttribute("vertex");
    bgShader->addAttribute("texcoord");
    bgShader->addUniform("tex");

    /* VBO */
    glGenBuffers(1, &bg_vertices_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, bg_vertices_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(bgShader->attribute("vertex"));
    glVertexAttribPointer(bgShader->attribute("vertex"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    /* VBO */
    glGenBuffers(1, &bg_texture_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, bg_texture_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureVertices), textureVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(bgShader->attribute("texcoord"));
    glVertexAttribPointer(bgShader->attribute("texcoord"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    /*
        Load Texture
    */

    int comp, sizex, sizey;
    unsigned char *buff = stbi_load("bg.png", &sizex, &sizey, &comp, 4 );

    if (buff == NULL)
        cout << "Error, buff was null";

    glGenTextures(1, &bgTexture);
    glBindTexture(GL_TEXTURE_2D, bgTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sizex, sizey, 0, GL_RGBA, GL_UNSIGNED_BYTE, buff);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bgTexture);
    glUniform1i(bgShader->uniform("tex"), 0);

    stbi_image_free(buff);
}

OGLRenderer::~OGLRenderer()
{
}

void OGLRenderer::update(std::vector<Voxel *> node)
{
    ukiyoeShader->use();

    /*
        Update projection view matrix
    */
    glUniformMatrix4fv(ukiyoeShader->uniform("view_matrix"), 1, GL_FALSE, glm::value_ptr(camera -> view));
    glUniformMatrix4fv(ukiyoeShader->uniform("projection_matrix"), 1, GL_FALSE, glm::value_ptr(camera -> projection));

    /*
        Update Instance model matrix
    */
    glBindBuffer(GL_ARRAY_BUFFER, models_buffer);
    glm::mat4 *matrices = (glm::mat4 *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    for (int n = 0; n < node.size(); n++)
    {
        Voxel *tmp = node[n];
        matrices[n] = glm::translate(glm::mat4(1.0f), tmp->pos);
        matrices[n] = glm::scale(matrices[n], glm::vec3(tmp->scale));
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    /*
        Update Instance model color
    */
    glBindBuffer(GL_ARRAY_BUFFER, colors_buffer);
    glm::vec4 *colors = (glm::vec4 *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    for (int n = 0; n < node.size(); n++)
    {
        Voxel *tmp = node[n];
        if (tmp->color.a < 1.0)
        {
            tmp->color.a = tmp->color.a + 0.01;
        }
        colors[n] = tmp->color;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    ukiyoeShader -> disable();
}

void OGLRenderer::render(std::vector<Voxel *> node)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    bgShader -> use();
    glBindVertexArray(bgVao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    bgShader -> disable();

    glClear(GL_DEPTH_BUFFER_BIT);
    ukiyoeShader -> use();
    glBindVertexArray(vao);
    glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0, node.size());
    ukiyoeShader -> disable();
}