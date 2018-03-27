#version 330

in vec3 position;
in vec3 normal;

out vec3 nor;
out vec3 pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 Perspective;

void main() 
{
	nor = mat3(transpose(inverse(model))) * normal;
	pos = vec3(model * vec4(position, 1.0));
	gl_Position = Perspective * view * model * vec4(position, 1.0);
}
