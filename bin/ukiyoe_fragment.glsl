// http://www.cgl.uwaterloo.ca/~ecdfourq/courses/cs798/assign1.html
// https://medium.com/@mattdesl/generative-impressionism-afa98ccb97da

#version 410

in vec4 color_out;

in vec3 vertex_out;
in vec3 normal_out;

out vec4 color;

void main(){
    //calculate the vector from this pixels surface to the light source
    vec3 surfaceToLight = vec3(50, 100, 50) - vertex_out;

    // //calculate the cosine of the angle of incidence
    float brightness = dot(normal_out, surfaceToLight) / (length(surfaceToLight) * length(normal_out));
    brightness = clamp(brightness, 0, 1);

    // color = normalize(color_out * vec4(normalize(normal_out), 1));
    color = color_out * 1.2;
}
