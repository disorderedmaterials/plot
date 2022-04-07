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

// Custom uniforms
uniform mat4 sceneDataTransformInverse;
uniform mat4 sceneDataAxes;
uniform vec3 sceneDataAxesExtents;

void main()
{
  // Convert vertex position to a vec4
  vec4 vertexPosition4 = vec4(vertexPosition, 1.0);

  // Transform vertex data to world space
  worldPosition = vec3(modelMatrix * vertexPosition4);
  worldNormal = modelNormalMatrix * vertexNormal;

  // Transform vertex into "plain" data space
  vec4 dataPosition = sceneDataTransformInverse * vec4(worldPosition, 1.0);

  // Clip vertices to data volume
  // -- X axis
  gl_ClipDistance[0] = dot(dataPosition, sceneDataAxes[0].xyzw);
  gl_ClipDistance[1] = dot(dataPosition, vec4(-sceneDataAxes[0].xyz, sceneDataAxesExtents.x));
  // -- Y axis
  gl_ClipDistance[2] = dot(dataPosition, sceneDataAxes[1].xyzw);
  gl_ClipDistance[3] = dot(dataPosition, vec4(-sceneDataAxes[1].xyz, sceneDataAxesExtents.y));
  // -- Z axis
  gl_ClipDistance[4] = dot(dataPosition, sceneDataAxes[2].xyzw);
  gl_ClipDistance[5] = dot(dataPosition, vec4(-sceneDataAxes[2].xyz, sceneDataAxesExtents.z));

  // Output projected vertex position
  gl_Position = modelViewProjection * vertexPosition4;
}
