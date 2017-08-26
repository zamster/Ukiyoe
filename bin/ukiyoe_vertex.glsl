#version 410

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;

layout(location = 2) in vec4 model_color;
layout(location = 3) in mat4 model_matrix;

out vec3 vertex_out;
out vec3 normal_out;
out vec4 color_out;

uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main(){
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vertex, 1);

    color_out = model_color;
    vertex_out = mat3(model_matrix) * vertex;
    normal_out = mat3(view_matrix * model_matrix) * normal;
}
