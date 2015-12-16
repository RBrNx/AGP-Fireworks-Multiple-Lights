#version 330 core
in vec3 in_Position;
in vec3 in_Normal;
in vec2 in_TexCoord;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(in_Position, 1.0f);
    Position = vec3(model * vec4(in_Position, 1.0f));
    Normal = mat3(transpose(inverse(model))) * in_Normal;
    TexCoords = in_TexCoord;
}