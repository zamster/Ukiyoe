#ifndef VOXEL_H_
#define VOXEL_H_

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

typedef struct
{
    glm::vec3 pos;
    glm::vec4 color;
    float scale;
    float rotate;
} Voxel;

#endif