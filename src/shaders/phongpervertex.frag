#version 450 core

// Input variables
layout(location = 0) in worldData
{
    vec3 position;
    vec3 normal;
    vec4 color;
}
frag;

layout(std140, binding = 2) uniform qt3d_custom_uniforms
{
    // -- Colour components
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    // -- Reflection coefficients (0.0 - 1.0)
    float ka;
    float kd;
    float ks;
    // -- Shininess coefficient (0.0 - 1.0)
    float shininess;
    // -- Light position
    vec3 lightPosition;
};

// Output variables
layout(location = 0) out vec4 fragColour;

void main()
{
    // Calculate normalised surface normal and light vector
    vec3 N = normalize(frag.normal);
    vec3 L = normalize(lightPosition - frag.position);

    // Lambert's cosine law
    float lambertian = max(dot(N, L), 0.0);
    float specPow = 0.0;
    if (lambertian > 0.0)
    {
        // Calculate reflected light vector and vector to viewer
        vec3 R = reflect(-L, N);
        vec3 V = normalize(-frag.position);

        // Compute the specular term
        float specAngle = max(dot(R, V), 0.0);
        specPow = pow(specAngle, shininess * 128);
    }

    // Construct final colour
    fragColour = vec4(ka * vec3(frag.color) + kd * lambertian * diffuse + ks * specPow * specular, 1.0);
}
