#version 330

layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

// Input Vertex Data
in worldData
{
    vec3 position;
    vec3 normal;
    vec4 color;
} vertices[];

// Output Fragment Data
out fragData
{
    vec3 normal;
    vec4 color;
} frag;

uniform vec2 viewportSize;
uniform float lineWidth = 1.5;

// Copy clip distances for specified input vertex
void applyClipping(int inVertexID)
{
    for (int i=0; i < 6; ++i)
    gl_ClipDistance[i] = gl_in[inVertexID].gl_ClipDistance[i];
}

void main()
{
    vec4 p1 = gl_in[0].gl_Position;
    vec4 p2 = gl_in[1].gl_Position;

    vec2 dir    = normalize((p2.xy - p1.xy) * viewportSize);
    vec2 offset = vec2(-dir.y, dir.x) * lineWidth / viewportSize;

    // Emit the four corners of our two triangles
    gl_Position = p1 + vec4(offset.xy * p1.w, 0.0, 0.0);
    applyClipping(0);
    frag.color = vertices[0].color;
    frag.normal = vertices[1].normal;
    EmitVertex();
    gl_Position = p1 - vec4(offset.xy * p1.w, 0.0, 0.0);
    applyClipping(0);
    frag.color = vertices[0].color;
    frag.normal = vertices[0].normal;
    EmitVertex();
    gl_Position = p2 + vec4(offset.xy * p2.w, 0.0, 0.0);
    applyClipping(1);
    frag.color = vertices[1].color;
    frag.normal = vertices[1].normal;
    EmitVertex();
    gl_Position = p2 - vec4(offset.xy * p2.w, 0.0, 0.0);
    applyClipping(1);
    frag.color = vertices[1].color;
    frag.normal = vertices[1].normal;
    EmitVertex();

    EndPrimitive();
}