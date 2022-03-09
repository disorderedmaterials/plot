#version 150 core

// Input variables
in vec3 vertexPosition;
in vec3 vertexNormal;

// Output variables
out vec3 worldPosition;
out vec3 worldNormal;

// Uniform variables per-primitive
uniform mat4 modelMatrix;
uniform mat3 modelNormalMatrix;
uniform mat4 modelViewProjection;

void main()
{
  vec4 vertex4 = vec4(vertexPosition, 1.0);

  // Transform vertex data to world space
  worldPosition = vec3(modelMatrix * vertex4);
  worldNormal = modelNormalMatrix * vertexNormal;

  // Output projected vertexgit c position
  gl_Position = modelViewProjection * vertex4;
}
