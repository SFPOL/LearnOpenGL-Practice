#ifndef GLSLOBJECTS_H
#define GLSLOBJECTS_H

#include <vector>
#include <cmath>
#include <glad/glad.h>

#pragma once
#include "stb_image.h"

class VBO {
public:
	unsigned int ID;
	std::vector<int> batches;
	unsigned int elem_total;

	VBO(auto* vertices,
		unsigned int sizeVertices,
		const std::vector<int>& batches,
		unsigned int elem_total)
		: batches(batches), elem_total(elem_total)
	{
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ARRAY_BUFFER, ID);
		glBufferData(GL_ARRAY_BUFFER, sizeVertices, vertices, GL_STATIC_DRAW);
	}

	void bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, ID);
	}

	void unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void del() const{
		glDeleteBuffers(1, &ID);
	}
};

class EBO {
public:
	unsigned int ID;

	EBO(unsigned int* indices,
		unsigned int sizeIndices)
	{
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndices, indices, GL_STATIC_DRAW);
	}

	void bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	}

	void unbind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void del() const {
		glDeleteBuffers(1, &ID);
	}
};
	

class VAO {
private:
	unsigned int rendererID;
	

public:
	unsigned int attributeIndex;

	VAO() : attributeIndex(0) {
		glGenVertexArrays(1, &rendererID);
	}

	void del() {
		glDeleteVertexArrays(1, &rendererID);
	}

	void bind() const {
		glBindVertexArray(rendererID);
	}

	void unbind() const {
		glBindVertexArray(0);
	}

	// divisor = 0 -> per vertex
	// divisor = 1 -> per instance
	void addBuffer(const VBO& vbo, GLuint divisor = 0) {
		bind();
		vbo.bind();

		unsigned int current_offset = 0;

		for (int batch : vbo.batches) {
			if (batch < 0) {
				current_offset += std::abs(batch);
			}
			else {
				glVertexAttribPointer(
					attributeIndex,
					batch,
					GL_FLOAT,
					GL_FALSE,
					vbo.elem_total * sizeof(float),
					(void*)(current_offset * sizeof(float))
				);

				glEnableVertexAttribArray(attributeIndex);
				glVertexAttribDivisor(attributeIndex, divisor);

				current_offset += batch;
				attributeIndex++;
			}
		}
	}
};

class Texture {
public:
	unsigned int ID;

	Texture(const std::string path) {
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

		if (data)
		{
			GLenum format = GL_RGB;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 4)
				format = GL_RGBA;

			// texture upload and mipmaps
			glGenTextures(1, &ID);
			glBindTexture(GL_TEXTURE_2D, ID);
			if (format == GL_RGBA) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			else {
				// set the texture wrapping parameters
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}
			// set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// load image, create texture and generate mipmaps

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}

	// This instance is for the FBOs
	// Here we are basicaly "reserving" the space for the FBO in form of a texture, but we are not uploading any data to it.
	Texture(unsigned int width, unsigned int height, GLenum format = GL_DEPTH_COMPONENT) {
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);
		glTexImage2D(GL_TEXTURE_2D, 0, format,
			width, height, 0, format, GL_FLOAT, NULL);

		// Base parameters. We can change it by binding then calling glTexParameteri. 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	}

	void bind(unsigned int unit) {
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, ID);
	}

	void unbind(unsigned int unit) {
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

};

#endif // GLSLOBJECTS_H
