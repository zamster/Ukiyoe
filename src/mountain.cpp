#include "mountain.h"

Mountain::Mountain(float _x, float _y, float _z)
{
    xPos = _x;
    yPos = _y;
    zPos = _z;
    voxels.clear();

    for (int i = 0; i < 360; ++i)
    {
        for (int j = 0; j < 52; ++j)
        {
            int   x    = sin(i * 3.14 / 180) * j;
            int   z    = cos(i * 3.14 / 180) * j;

            float tmpx = x / 26.0;
            float tmpz = z / 26.0;
            float tmpy = pow(M_E, -tmpx * tmpx - tmpz * tmpz) * 96;

            Voxel tmp;
            tmp.scale = 0.65;

            tmp.color = glm::vec4(31, 71, 136, 0) / 255.0f;

            if ((tmpy > 32) && (rand() % 2 == 1))
                tmp.color = glm::vec4(235, 246, 247, 0) / 255.0f;

            if (tmpy > 64)
                tmp.color = glm::vec4(235, 246, 247, 0) / 255.0f;

            if (tmpy >= 88)
                tmp.color = glm::vec4(198, 194, 182, 0) / 255.0f;

            if (tmpy >= 92)
                continue;

            tmp.pos = glm::vec3(xPos + x * 2.5, yPos + tmpy, zPos + z * 2.5);
            voxels.push_back(tmp);
        }
    }
}

Mountain::~Mountain()
{
}
