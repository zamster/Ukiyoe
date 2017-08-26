#include <cstdio>
#include <ctime>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "scene.h"

Camera *camera;
Scene *scene;

static void error_callback(int error, const char *description)
{
    printf("%d:%s\n", error, description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    camera->move(key);

    if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS))
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if ((key == GLFW_KEY_SPACE) && (action == GLFW_PRESS))
    {
        scene -> reset();

        printf("camera:\t%f, %f, %f\n",
               camera->camera_position.x,
               camera->camera_position.y,
               camera->camera_position.z);

        printf("look:\t%f, %f, %f\n",
               camera->camera_look_at.x,
               camera->camera_look_at.y,
               camera->camera_look_at.z);
    }
}

static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    // if ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS))
    // {
    //     double x, y;
    //     glfwGetCursorPos(window, &x, &y);
    //     camera->enable_rotate(x, y);
    // }
    // else if ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_RELEASE))
    // {
    //     camera->disable_rotate();
    // }
    if ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_RELEASE))
    {
        scene -> reset();
    }

    if ((button == GLFW_MOUSE_BUTTON_RIGHT) && (action == GLFW_RELEASE))
    {
        scene -> toggleVisit();
    }

    if ((button == GLFW_MOUSE_BUTTON_MIDDLE) && (action == GLFW_RELEASE))
    {
        scene -> switchType();
        scene -> reset();
    }
}

static void mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    if (yoffset > 0)
    {
        scene -> incNum();
        scene -> reset();
    }
    else if (yoffset < 0)
    {
        scene -> decNum();
        scene -> reset();
    }
}

static void cursor_callback(GLFWwindow *window, double x, double y)
{
    camera->rotate(x, y);
}

void _update_fps_counter(GLFWwindow *window)
{
    static double previous_seconds = glfwGetTime();
    static int frame_count;
    double current_seconds = glfwGetTime();
    double elapsed_seconds = current_seconds - previous_seconds;

    if (elapsed_seconds > 1.0)
    {
        previous_seconds = current_seconds;
        double fps = (double)frame_count / elapsed_seconds;
        char tmp[128];
        sprintf(tmp, "Ukiyoe @ fps: %.2f § Voxel: %d", fps,  (int)scene->render_node.size());
        glfwSetWindowTitle(window, tmp);
        frame_count = 0;
    }

    frame_count++;
}

int main(int argc, char **argv)
{
    srand(time(0));

    GLFWwindow *window;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (!(window = glfwCreateWindow(1280, 800, "Ukiyoe", glfwGetPrimaryMonitor(), NULL)))
    {
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();

    if (GLEW_OK != err)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }

    printf("---------------------------------------------------\n");
    printf("GL:\t%s\n",   glGetString(GL_VERSION));
    printf("GLSL:\t%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("GPU:\t%s\n",  glGetString(GL_RENDERER));
    printf("GLEW:\t%s\n", glewGetString(GLEW_VERSION));
    printf("---------------------------------------------------\n\n");

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    camera                  = new Camera(width, height);
    camera->camera_position = glm::vec3(400.263855, 30.000000, 264.931793);
    camera->camera_look_at  = glm::vec3(0.000000, 60.000000, 0.000000);

    scene                   = new Scene();

    while (!glfwWindowShouldClose(window))
    {
        _update_fps_counter(window);

        camera->update();

        scene->update();
        scene->render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
