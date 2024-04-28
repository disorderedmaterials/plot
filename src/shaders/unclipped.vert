#version 450 core

// Input variables
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec4 vertexColor;

// Output Vertex Data
layout(location = 0) out worldData
{
  vec3 position;
  vec3 normal;
  vec4 color;
} world;

layout(std140, binding = 1) uniform qt3d_command_uniforms {
  mat4 modelMatrix;
  mat4 inverseModelMatrix;
  mat4 modelViewMatrix;
  mat3 modelNormalMatrix;
  mat4 inverseModelViewMatrix;
  mat4 modelViewProjection;
  mat4 inverseModelViewProjectionMatrix;
};

void main()
{
  // Convert vertex position to a vec4
  vec4 vertexPosition4 = vec4(vertexPosition, 1.0);

  // Transform vertex data to world space
  world.position = vec3(modelMatrix * vertexPosition4);
  world.normal = modelNormalMatrix * vertexNormal;
  world.color = vertexColor;

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
