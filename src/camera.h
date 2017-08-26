#ifndef CAMERA_H
#define CAMERA_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

class Camera
{
public:

    float aspect;
    float field_of_view;
    float near_clip;
    float far_clip;

    float camera_speed;
    float camera_heading;
    float camera_pitch;

    glm::vec3 camera_position;
    glm::vec3 camera_look_at;
    glm::vec3 camera_up;

    glm::mat4 projection;
    glm::mat4 view;

    glm::vec3 camera_position_delta;
    glm::vec3 camera_direction;

    bool move_camera;
    glm::vec3 mouse_position;

    Camera(int width, int height);
    ~Camera();

    void update();

    // camera move
    void move(int key);

    // camera retate
    void rotate(double x, double y);
    void enable_rotate(double x, double y);
    void disable_rotate();
};
#endif
