#ifndef SCENE_H_
#define SCENE_H_

#include "mountain.h"
#include "sakura.h"
#include "wave.h"

#include "render.h"

#include <sys/time.h>

class Scene
{
public:

    OGLRenderer *renderer;

    vector<vector<Voxel> *> node;
    vector<Voxel *> render_node;

    Wave *wave;

    bool isVisit;
    float degree;

    int numSakura;
    int typeSakura;

    Scene();
    ~Scene();

    void update();
    void render();

    void reset();

    void toggleVisit();

    void incNum();
    void decNum();
    void switchType();
};

#endif
