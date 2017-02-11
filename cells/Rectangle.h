#pragma once

#include <glm/glm.hpp>
#include "logic/GLSLShader.h"

class CRectangle
{
public:
    CRectangle(void);
    ~CRectangle(void);
	void RenderRec(const float* MVP);
	void set_color(int r, int g, int b, int a);
	void set_vertices(glm::vec3 * data);
private:
    GLfloat color[4];
	GLuint vaoID;
	GLuint vboVerticesID;
	GLSLShader shader;
	glm::vec3 vertices[4];
};

