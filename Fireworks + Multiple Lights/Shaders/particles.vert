// Vertex Shader – file "particles.vert"

#version 330

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Color;
out vec4 ex_Color;

uniform float particleLifetime;
uniform mat4 mvpMat;

void main(void)
{
	float alpha = particleLifetime/100;
	ex_Color = vec4(in_Color, alpha);
	gl_Position = mvpMat * vec4(in_Position, 1.0);
}