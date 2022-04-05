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
uniform mat3 viewRotation;

void main()
{
  vec4 vertex4 = vec4(vertexPosition, 1.0);

  // Transform vertex data to world space
  worldPosition = vec3(modelMatrix * vertex4);
  worldNormal = modelNormalMatrix * vertexNormal;

  vec3 test = vec3(modelMatrix * vec4(339.5,0.0,0.0,1.0));

//  for (int i = 0; i < sectionsData.sectionsCount; ++i) {
  gl_ClipDistance[0] = dot(vec4(worldPosition - vec3(0.0,0,0), 1.0), vec4(1.0,0.0,0.0, 0.0));
    gl_ClipDistance[1] = dot(vec4(vertexPosition - vec3(0,336,0), 1.0), vec4(0.0, 1.0, 0.0, 336.0));
  gl_ClipDistance[2] = dot(vec4(vertexPosition - vec3(0,0,-339.5), 1.0), vec4(0.0, 0.0, -1.0, 339.5));
//  }

  // Output projected vertex position
  gl_Position = modelViewProjection * vertex4;
}
