#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "logic/GLSLShader.h"

class CLine
{
public:
	CLine();
	virtual ~CLine(); 
	void RenderLine(const float* MVP);
	void set_color(float r, float g, float b, float a);
	void set_vertices(glm::vec3 * data);
	void set_endVisible(bool visible);
	void set_segVisible(bool visible);
private:
    GLfloat color[4];
	GLuint vaoID;
	GLuint vboVerticesID;
	GLuint vboIndices;
	GLSLShader shader;
	glm::vec3 vertices[2];
	bool renderEnds;
	bool renderSeg;
};

