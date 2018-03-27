#version 330

out vec4 fragColour;

in vec3 nor;
in vec3 pos;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main() 
{
	vec3 I = normalize(pos - cameraPos);
	vec3 R = reflect(I, normalize(nor));
	fragColour = vec4(texture(skybox, R).rgb, 1.0);
}
