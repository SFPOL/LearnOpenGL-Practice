#ifndef GLSLOBJECTS_H
#define GLSLOBJECTS_H

#include <vector>
#include <cmath>
#include <glad/glad.h>

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

#endif // GLSLOBJECTS_H
