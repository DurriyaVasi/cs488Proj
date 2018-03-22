#version 330

struct LightSource {
    vec3 position;
    vec3 rgbIntensity;
};

in VsOutFsIn {
	vec3 position_ES; // Eye-space position
	vec3 normal_ES;   // Eye-space normal
	LightSource light;
	vec2 textureCoord;
	mat3 TBN;
} fs_in;


out vec4 fragColour;

struct Material {
    vec3 kd;
    vec3 ks;
    float shininess;
    float alpha;
};
uniform Material material;

// Ambient light intensity for each RGB component.
uniform vec3 ambientIntensity;

uniform int pickingMode;
uniform vec3 colour;

uniform int hasTexture;
uniform int hasBumps;
uniform sampler2D textureData;
uniform sampler2D textureNormals;

vec3 phongModel(vec3 l, vec3 v, vec3 fragNormal, vec3 kd) {
	LightSource light = fs_in.light;

	float n_dot_l = max(dot(fragNormal, l), 0.0);

        vec3 diffuse;
        diffuse = kd * n_dot_l;

    vec3 specular = vec3(0.0);

    if (n_dot_l > 0.0) {
                // Halfway vector.
                vec3 h = normalize(v + l);
        float n_dot_h = max(dot(fragNormal, h), 0.0);

        specular = material.ks * pow(n_dot_h, material.shininess);
    }

    return ambientIntensity + light.rgbIntensity * (diffuse + specular);
}

void main() {
	if (pickingMode == 1) {
		fragColour = vec4(colour, 1.0);
	}
	else if (pickingMode == 0) {
		vec3 l = normalize(fs_in.light.position - fs_in.position_ES);
		vec3 v = normalize(-fs_in.position_ES.xyz);
		if (hasTexture == 1 && hasBumps == 0) {
			vec3 kd = vec3(texture(textureData, fs_in.textureCoord));
			fragColour = vec4(phongModel(l, v, fs_in.normal_ES, kd), 1.0);
		}
		else if (hasTexture == 1 && hasBumps == 1) {
			vec3 normal = vec3(texture(textureNormals, fs_in.textureCoord));
			normal = normalize(normal * 2.0 - 1.0);
			vec3 kd = vec3(texture(textureData, fs_in.textureCoord));
			l = fs_in.TBN * l;
			v = fs_in.TBN * v;
			fragColour = vec4(phongModel(l, v, normal, kd), 1.0);	
		}
		else {
			fragColour = vec4(phongModel(l , v, fs_in.normal_ES, material.kd), material.alpha);
		}
	}
}
