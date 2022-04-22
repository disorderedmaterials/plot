#version 150
in highp vec3 vertex;
in mediump vec2 texCoord;
out mediump vec2 texc;
uniform mediump mat4 matrix;

void main(void)
{
    gl_Position = matrix * vec4(vertex, 1.0);
    texc = texCoord;
}