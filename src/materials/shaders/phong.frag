#version 150 core

// Input variables
in vec3 worldNormal;
in vec3 worldPosition;

// Uniform variables per-primitive
// -- Colour components
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
// -- Reflection coefficients (0.0 - 1.0)
uniform float ka;
uniform float kd;
uniform float ks;
// -- Shininess coefficient (0.0 - 1.0)
uniform float shininess;
// -- Light position
uniform vec3 lightPosition;

// Output variables
out vec4 fragColour;

void main() {
  // Calculate normalised surface normal and light vector
  vec3 N = normalize(worldNormal);
  vec3 L = normalize(lightPosition - worldPosition);

  // Lambert's cosine law
  float lambertian = max(dot(N, L), 0.0);
  float specPow = 0.0;
  if (lambertian > 0.0)
  {
    // Calculate reflected light vector and vector to viewer
    vec3 R = reflect(-L, N);
    vec3 V = normalize(-worldPosition);

    // Compute the specular term
    float specAngle = max(dot(R, V), 0.0);
    specPow = pow(specAngle, shininess*128);
  }

  // Construct final colour
  fragColour = vec4(ka * ambient +
                      kd * lambertian * diffuse +
                      ks * specPow * specular, 1.0);
}