#version 330

// Model-Space coordinates
in vec3 position;

out vec3 texCoords;

uniform mat4 View;
uniform mat4 Perspective;

void main() {
	texCoords = position;
	vec4 pos = Perspective * View * vec4( position, 1.0 );
	gl_Position = pos.xyww;
}
