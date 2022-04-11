#version 150 core

// Input variables
in vec3 vertexPosition;
in vec3 vertexNormal;

// Output variables
out vec3 worldPosition;
out vec3 worldNormal;

// Standard uniform variables per-primitive
uniform mat4 modelMatrix;
uniform mat3 modelNormalMatrix;
uniform mat4 modelViewProjection;

void main()
{
  // Convert vertex position to a vec4
  vec4 vertexPosition4 = vec4(vertexPosition, 1.0);

  // Transform vertex data to world space
  worldPosition = vec3(modelMatrix * vertexPosition4);
  worldNormal = modelNormalMatrix * vertexNormal;

  // Unclip vertices
  // -- X axis
  gl_ClipDistance[0] = 1.0;
  gl_ClipDistance[1] = 1.0;
  // -- Y axis
  gl_ClipDistance[2] = 1.0;
  gl_ClipDistance[3] = 1.0;
  // -- Z axis
  gl_ClipDistance[4] = 1.0;
  gl_ClipDistance[5] = 1.0;

  // Output projected vertex position
  gl_Position = modelViewProjection * vertexPosition4;
}
