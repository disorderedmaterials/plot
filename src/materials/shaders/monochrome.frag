#version 150 core

// Uniform variables per-primitive
// -- Colour components
uniform vec3 ambient;

// Output variables
out vec4 fragColour;

void main() {
  fragColour = vec4(ambient, 1.0);
}
