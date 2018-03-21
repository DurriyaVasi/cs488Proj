#version 330

// Model-Space coordinates
in vec3 position;

out vec3 texCoords;

uniform mat4 View;
uniform mat4 Perspective;

void main() {
	texCoords = position;
	gl_Position = Perspective * View * vec4( position, 1.0 );
}
