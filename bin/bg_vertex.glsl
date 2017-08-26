#version 410

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 texcoord;

out vec2 texcoord_out;

void main(){
    gl_Position = vertex;
    texcoord_out = texcoord.xy;
}
