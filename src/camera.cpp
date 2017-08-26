#include "camera.h"

Camera::Camera(int width, int height)
{
    aspect                = (float)width / height;
    camera_heading        = 0.0f;
    camera_pitch          = 0.0f;
    camera_position_delta = glm::vec3(0, 0, 0);
    camera_speed          = 1.25f;
    camera_up             = glm::vec3(0, 1, 0);
    near_clip             = 0.1f;
    far_clip              = 1000.0f;
    field_of_view         = 45.0f;
    move_camera           = false;
}

Camera::~Camera()
{
}

void Camera::update()
{
    // need to set the matrix state. this is only important because lighting
    // doesn't work if this isn't done
    camera_direction = glm::normalize(camera_look_at - camera_position);

    // detmine axis for pitch rotation
    glm::vec3 axis = glm::cross(camera_direction, camera_up);

    // compute quaternion for pitch based on the camera pitch angle
    glm::quat pitch_quat = glm::angleAxis(camera_pitch, axis);

    // determine heading quaternion from the camera up vector and the heading
    // angle
    glm::quat heading_quat = glm::angleAxis(camera_heading, camera_up);

    // add the two quaternions
    glm::quat temp = glm::cross(pitch_quat, heading_quat);
    temp = glm::normalize(temp);

    // update the direction from the quaternion
    camera_direction = glm::rotate(temp, camera_direction);

    // add the camera delta
    camera_position += camera_position_delta;

    // set the look at to be infront of the camera
    camera_look_at = camera_position + camera_direction;

    // damping for smooth camera
    camera_heading       *= .25f;
    camera_pitch         *= .25f;
    camera_position_delta = camera_position_delta * .8f;

    // compute the projection, view matrix
    projection = glm::perspective(field_of_view, aspect, near_clip, far_clip);
    view       = glm::lookAt(camera_position, camera_look_at, camera_up);
}

void Camera::move(int key)
{
    switch (key)
    {
    case 69:
        camera_position_delta += camera_up * camera_speed;
        break;

    case 81:
        camera_position_delta -= camera_up * camera_speed;
        break;

    case 65:
        camera_position_delta -=
            glm::cross(camera_direction, camera_up) * camera_speed;
        break;

    case 68:
        camera_position_delta +=
            glm::cross(camera_direction, camera_up) * camera_speed;
        break;

    case 87:
        camera_position_delta += camera_direction * camera_speed;
        break;

    case 83:
        camera_position_delta -= camera_direction * camera_speed;
        break;

    default:
        break;
    }
}

void Camera::rotate(double x, double y)
{
    // if the camera is moving, meaning that the mouse was clicked and dragged,
    // change the pitch and heading
    if (move_camera)
    {
        // compute the mouse delta from the previous mouse position
        glm::vec3 mouse_delta = mouse_position - glm::vec3(x, y, 0);

        // This controls how the heading is changed if the camera is pointed
        // straight up or down
        // The heading delta direction changes
        if (((camera_pitch >  90) && (camera_pitch <  270)) ||
            ((camera_pitch < -90) && (camera_pitch > -270)))
        {
            camera_heading -= .001f * mouse_delta.x;
        }
        else
        {
            camera_heading += .001f * mouse_delta.x;
        }

        camera_heading = fmod(camera_heading, 360.f);
        camera_pitch  += .001f * mouse_delta.y;
        camera_pitch   = fmod(camera_pitch, 360.f);
        mouse_position = glm::vec3(x, y, 0);
    }
}

void Camera::enable_rotate(double x, double y)
{
    mouse_position = glm::vec3(x, y, 0);
    move_camera    = true;
}

void Camera::disable_rotate()
{
    move_camera = false;
}
