#version 150 core

// Input variables
in vec3 vertexPosition;
in vec3 vertexNormal;
in vec4 vertexColor;

// Output Vertex Data
out worldData
{
  vec3 position;
  vec3 normal;
  vec4 color;
} world;

// Standard uniform variables per-primitive
uniform mat4 modelMatrix;
uniform mat3 modelNormalMatrix;
uniform mat4 modelViewProjection;

// Custom uniforms
uniform mat4 sceneDataTransformInverse;
uniform mat4 sceneDataAxes;
uniform vec3 sceneDataAxesExtents;
uniform vec3 sceneDataAxesOrigin;

void main()
{
  // Convert vertex position to a vec4
  vec4 vertexPosition4 = vec4(vertexPosition, 1.0);

  // Transform vertex data to world space
  world.position = vec3(modelMatrix * vertexPosition4);
  world.normal = modelNormalMatrix * vertexNormal;
  world.color = vertexColor;

  // Transform vertex into "plain" data space
  vec4 dataPosition = sceneDataTransformInverse * vec4(world.position, 1.0);
  dataPosition.xyz -= sceneDataAxesOrigin;

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
