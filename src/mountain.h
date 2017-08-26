#ifndef MOUNTAIN_H_
#define MOUNTAIN_H_

#include "voxel.h"
#include <vector>
using namespace std;

class Mountain
{
public:
    float xPos, yPos, zPos;
    vector<Voxel> voxels;

    Mountain(float _x, float _y, float _z);
    ~Mountain();
};

#endif
