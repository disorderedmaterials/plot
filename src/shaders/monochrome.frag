#version 450 core

// Uniform variables per-primitive
// -- Colour components
layout(binding = 0) uniform material_uniforms{ vec3 ambient; };

// Output variables
layout(location = 0) out vec4 fragColour;

void main() {
  fragColour = vec4(1.0,0.0,0.0, 1.0);
}
