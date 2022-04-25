#version 150

// Input variables
in mediump vec2 texc;

// Custom uniforms
uniform sampler2DMS plotTexture;
uniform int samples;

// Output variables
out vec4 fragColour;

void main(void)
{
   ivec2 tc = ivec2(floor(textureSize(plotTexture) * texc));
   vec4 color = vec4(0.0);
   for (int i = 0; i < samples; ++i)
       color += texelFetch(plotTexture, tc, i);
    fragColour = color / float(samples);
}
