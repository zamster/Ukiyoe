#version 410

in vec2 texcoord_out;

uniform sampler2D tex;

out vec4 color;

void main(){
    vec2 flipped_texcoord = vec2(texcoord_out.x, texcoord_out.y);
    color = texture(tex, flipped_texcoord);
}
