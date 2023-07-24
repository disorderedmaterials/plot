#version 330 core

layout (location = 0) in vec3 position;
out vec4 color;

uniform float line_width;
uniform float line_stipple_factor;
uniform float line_stipple_pattern;


void main() {
    float step = 1.0 / line_stipple_factor;
    float pattern = mod(position.x * step, line_stipple_pattern);
    if (pattern > 0.5) {
        discard;
    }
    gl_Position = vec4(position, 1.0);
    color = vec4(1.0, 0.0, 0.0, 1.0);
}

// Compile the shader
GLuint shader = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(shader,1, &shader_source, NULL);
glCompileShader(shader);

// Bind the shader to the current OpenGl context
glUseProgram(shader);

// Set the uniforms
glUniform1f(glGetUniformLocation(shader, "line_width"), 1.0f);
glUniform1i(glGetUniformLocation(shader, "line_stipple_factor"), 1);
glUniform1ui(glGetUniformLocation(shader, "line_stipple_pattern"), 0xFFFF);

// Draw the line
glDrawArrays(GL_LINE_STRIP, 0, 4);
