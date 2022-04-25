#version 150

// Input variables
in mediump vec2 texc;

// Custom uniforms
uniform sampler2D plotTexture;

// Output variables
out vec4 fragColour;

void main(void)
{
   ivec2 tc = ivec2(floor(textureSize(plotTexture, 0) * texc));
   fragColour = texelFetch(plotTexture, tc, 0);
}
