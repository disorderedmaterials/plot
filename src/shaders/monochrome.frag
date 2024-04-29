#version 450 core

layout(std140, binding = 2) uniform qt3d_custom_uniforms {
  vec3 ambient;
};

// Output variables
layout(location = 0) out vec4 fragColour;

void main() {
  fragColour = vec4(ambient, 1.0);
}
