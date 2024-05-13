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

layout(std140, binding = 2) uniform qt3d_custom_uniforms {
    mat4 sceneDataTransformInverse;
    mat4 sceneDataAxes;
    vec3 sceneDataAxesExtents;
    vec3 sceneDataAxesOrigin;
};

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
