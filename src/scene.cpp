#include "scene.h"
#include "camera.h"

extern Camera *camera;

Scene::Scene()
{
    isVisit = false;
    degree = 0;
    numSakura = 8;
    typeSakura = 1;

    node.clear();

    Mountain *mountain = new Mountain(0, 0, 0);
    node.push_back(&mountain->voxels);

    for (int i = 0; i < 360; i += 45)
    {
        Sakura *sakura = new Sakura(sin(i * 3.14 / 180) * 260, 0, cos(i * 3.14 / 180) * 260, typeSakura);
        node.push_back(&sakura->voxels);
    }

    // wave = new Wave(200, 0, 300);
    // node.push_back(&wave->voxels);

    renderer = new OGLRenderer();
}

void Scene::reset()
{
    node.clear();

    Mountain *mountain = new Mountain(0, 0, 0);
    node.push_back(&mountain->voxels);

    int num = numSakura;
    num = 360 / num;

    for (int i = 0; i < 360; i += num)
    {
        Sakura *sakura = new Sakura(sin(i * 3.14 / 180) * 260, 0, cos(i * 3.14 / 180) * 260, typeSakura);
        node.push_back(&sakura->voxels);
    }
}

void Scene::toggleVisit()
{
    isVisit = !isVisit;
}

void Scene::switchType()
{
    typeSakura = !typeSakura;
}

void Scene::incNum()
{
    numSakura = numSakura + 1;

    if (numSakura >= 12)
    {
        numSakura = 12;
    }
}

void Scene::decNum()
{
    numSakura = numSakura - 1;

    if (numSakura <= 1)
    {
        numSakura = 1;
    }
}

void Scene::update()
{
    // Update camera
    if (isVisit == true)
    {
        float x = 480 * sin(degree * 0.0174532925);
        float y = 480 * cos(degree * 0.0174532925);

        camera->camera_position = glm::vec3(x, 30, y);
        camera->camera_look_at  = glm::vec3(0, 30, 0);
        degree += 0.1;
    }

    // wave -> update();

    render_node.clear();

    // fov culling
    glm::mat4 pv = camera -> projection * camera -> view;

    for (int i = 0; i < node.size(); ++i)
    {
        vector<Voxel> *nodev = node[i];

        for (int j = 0; j < nodev -> size(); ++j)
        {
            Voxel *tmp = &nodev -> at(j);

            glm::vec4 coords = pv * glm::vec4(tmp->pos, 1);

            coords.x /= coords.w;
            coords.y /= coords.w;

            if (!(coords.x < -1 || coords.x > 1 || coords.y < -1 || coords.y > 1 || coords.z < 0))
            {
                render_node.push_back(tmp);
            }
        }
    }

    renderer -> update(render_node);
}

void Scene::render()
{
    renderer -> render(render_node);
}

Scene::~Scene()
{
}