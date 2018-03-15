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

vec3 phongModel(vec3 fragPosition, vec3 fragNormal) {
	LightSource light = fs_in.light;

    // Direction from fragment to light source.
    vec3 l = normalize(light.position - fragPosition);

    // Direction from fragment to viewer (origin - fragPosition).
    vec3 v = normalize(-fragPosition.xyz);

    float n_dot_l = max(dot(fragNormal, l), 0.0);

	vec3 diffuse;
	diffuse = material.kd * n_dot_l;

    vec3 specular = vec3(0.0);

    if (n_dot_l > 0.0) {
		// Halfway vector.
		vec3 h = normalize(v + l);
        float n_dot_h = max(dot(fragNormal, h), 0.0);

        specular = material.ks * pow(n_dot_h, material.shininess);
    }

    return ambientIntensity + light.rgbIntensity * (diffuse + specular);
}

vec3 otherPhongModel(vec3 fragPosition, vec3 fragNormal, vec3 kd) {
	LightSource light = fs_in.light;
	vec3 l = fs_in.TBN * normalize(light.position - fragPosition);
	vec3 v = fs_in.TBN * normalize(-fragPosition.xyz);
	
	float n_dot_l =max(dot(fragNormal, l), 0.0);

	vec3 diffuse;
	diffuse = kd * n_dot_l;

	vec3 specular = vec3(0.0);

	if (n_dot_l > 0.0) {
		vec3 h = normalize(v + l);
		float n_dot_h =  max(dot(fragNormal, h), 0.0);
		specular = material.ks * pow(n_dot_h, material.shininess);
	}
	
	return ambientIntensity + light.rgbIntensity * (diffuse + specular);
}

vec3 anotherPhongModel(vec3 fragPosition, vec3 fragNormal, vec3 kd) {
        LightSource light = fs_in.light;
        vec3 l = normalize(light.position - fragPosition);
	vec3 v = normalize(-fragPosition.xyz);
        
        float n_dot_l =max(dot(fragNormal, l), 0.0);

        vec3 diffuse;
        diffuse = kd * n_dot_l;

        vec3 specular = vec3(0.0);

        if (n_dot_l > 0.0) {
                vec3 h = normalize(v + l);
                float n_dot_h =  max(dot(fragNormal, h), 0.0);
                specular = material.ks * pow(n_dot_h, material.shininess);
        }

        return ambientIntensity + light.rgbIntensity * (diffuse + specular);
}


void main() {
	if (pickingMode == 1) {
		fragColour = vec4(colour, 1.0);
	}
	else if (pickingMode == 0) {
		if (hasTexture == 1 && hasBumps == 0) {
			vec3 kd = vec3(texture(textureData, fs_in.textureCoord));
			fragColour = vec4(anotherPhongModel(fs_in.position_ES, fs_in.normal_ES, kd), 1.0);
		}
		else if (hasTexture == 1 && hasBumps == 1) {
			vec3 normal = vec3(texture(textureNormals, fs_in.textureCoord));
			normal = normalize(normal * 2.0 - 1.0);
			vec3 kd = vec3(texture(textureData, fs_in.textureCoord));
			fragColour = vec4(otherPhongModel(fs_in.position_ES, normal, kd), 1.0);	
		}
		else {
			fragColour = vec4(phongModel(fs_in.position_ES, fs_in.normal_ES), 1.0);
		}
	}
}
