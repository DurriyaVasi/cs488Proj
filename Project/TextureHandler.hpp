#pragma once
#include <map>
#include <string>
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include "stb_image.h"
#include <iostream>

using namespace std;

class TextureHandler {
	map<string, GLuint> fileTextures;
	map<string, GLuint> fileTextureNormals;

	public:
	void addTexture(string filename) {
		GLuint texture;
		glGenTextures(1, &texture);
		CHECK_GL_ERRORS;

		glBindTexture(GL_TEXTURE_2D, texture);
                CHECK_GL_ERRORS;

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                CHECK_GL_ERRORS;
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                CHECK_GL_ERRORS;
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                CHECK_GL_ERRORS;
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                CHECK_GL_ERRORS;

		int width, height, nrChannels;
                unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
                CHECK_GL_ERRORS;
                
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);
		fileTextures.insert(std::pair<string, GLuint>(filename, texture));
	}

	void addTextureNormal(string filename) {
                GLuint texture;
                glGenTextures(1, &texture);
                CHECK_GL_ERRORS;

                glBindTexture(GL_TEXTURE_2D, texture);
                CHECK_GL_ERRORS;

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                CHECK_GL_ERRORS;
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                CHECK_GL_ERRORS;
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                CHECK_GL_ERRORS;
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                CHECK_GL_ERRORS;

                int width, height, nrChannels;
                unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                CHECK_GL_ERRORS;

                glBindTexture(GL_TEXTURE_2D, 0);
                stbi_image_free(data);
                fileTextureNormals.insert(std::pair<string, GLuint>(filename, texture));
        }


	GLuint getTexture(string filename) {
		if (fileTextures.find(filename) == fileTextures.end()) {
			cerr << "Could not find texture " << filename << endl;
		}
		return fileTextures.at(filename);	
	}

	GLuint getTextureNormal(string filename) {
		if (fileTextureNormals.find(filename) == fileTextureNormals.end()) {
			cerr << "Could nto find texture normal " << filename << endl;
		}
		return fileTextureNormals.at(filename);
	}
	
};

