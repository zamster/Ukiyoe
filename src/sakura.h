#ifndef SAKURA_H_
#define SAKURA_H_

#include "voxel.h"
#include <vector>
using namespace std;

class Sakura
{
public:
    int type;
    float xPos, yPos, zPos;
    vector<Voxel> voxels;

    Sakura(float _x, float _y, float _z, int _t);
    ~Sakura();

    bool exist(float x, float y, float z);
    void branch(glm::vec3 pos, float _angel, float _size);
};

#endif