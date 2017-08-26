#include "sakura.h"

Sakura::Sakura(float _x, float _y, float _z, int _t)
{
    xPos = _x;
    yPos = _y;
    zPos = _z;
    type = _t;

    voxels.clear();
    branch(glm::vec3(0, 0, 0), 60.0f, 30.0f);
}

Sakura::~Sakura()
{
}

bool Sakura::exist(float x, float y, float z)
{
    for (int i = 0; i < voxels.size(); ++i)
    {
        float dist = (voxels[i].pos.x - x) * (voxels[i].pos.x - x) +
                     (voxels[i].pos.y - y) * (voxels[i].pos.y - y) +
                     (voxels[i].pos.z - z) * (voxels[i].pos.z - z);

        if (dist < 1.415)
        {
            return true;
        }
    }

    return false;
}

void Sakura::branch(glm::vec3 pos, float _angel, float _size)
{
    int bHeight = cos(_angel * 3.14 / 180) * _size;
    int offset = rand() % 360;

    for (int i = 0; i <= bHeight; i++)
    {
        int x = sin(offset * 3.14 / 180) * i;
        int z = cos(offset * 3.14 / 180) * i;

        Voxel tmp;
        tmp.scale = 0.3;

        tmp.pos = glm::vec3(xPos + x + pos.x, yPos + i + pos.y, zPos + z + pos.z);
        tmp.color = glm::vec4(103, 36, 34, 0) / 255.0f;

        if (!Sakura::exist(xPos + x + pos.x, yPos + i + pos.y, zPos + z + pos.z))
        {
            voxels.push_back(tmp);
        }
    }

    glm::vec3 tpos = pos + glm::vec3(sin(offset * 3.14 / 180) * bHeight,
                                     bHeight,
                                     cos(offset * 3.14 / 180) * bHeight);

    if (_size >= 4)
    {
        float d;
        d = (rand() % 10) / 10 - 0.5;
        d = _angel + d * _angel;
        branch(tpos, 45.0f, bHeight);
        branch(tpos, 60.0f, bHeight);
        branch(tpos, d, bHeight);
    }
    else
    {
        for (int i = 0; i < 16; ++i)
        {
            int a = rand() % 360;
            int x = sin(a * 3.14 / 180) * 8;
            int y = rand() % 16 - 2;
            int z = cos(a * 3.14 / 180) * 8;

            Voxel tmp;
            tmp.scale = 0.5;
            tmp.pos = glm::vec3(xPos + x + tpos.x, yPos + i + tpos.y, zPos + z + tpos.z);

            if (type == 0) {
                tmp.color = glm::vec4(219, 90, 107, 0) / 255.0f;
            }

            if (type == 1) {
                tmp.color = glm::vec4(243, 243, 242, 0) / 255.0f;
            }

            if (!Sakura::exist(xPos + x + tpos.x, yPos + i + tpos.y, zPos + z + tpos.z))
            {
                voxels.push_back(tmp);
            }
        }
    }
}