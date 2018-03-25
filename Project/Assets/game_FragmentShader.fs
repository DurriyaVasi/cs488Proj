#version 330

out vec4 fragColour;

uniform vec3 colour;

uniform int hasTexture;
uniform int hasBumps;
uniform sampler2D textureData;
uniform sampler2D textureNormals;

void main() {
	fragColour = vec4(colour, 1.0);
}
