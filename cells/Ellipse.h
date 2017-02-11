#pragma once

#include <glm/glm.hpp>
#include "logic/GLSLShader.h"

class CEllipse
{
public:
	CEllipse(void);
	~CEllipse(void);
	void RenderCircle(const float* MVP, int line=false);
	void set_color(float r, float g, float b, float a);
	void set_center(glm::vec3 data);
	void set_Radius(float a, float b);
	void computeCirclePoints();
private:
    GLfloat color[4];
	GLuint vaoID;
	GLuint vboVerticesID;
	GLSLShader shader;
	glm::vec3 vertices[100];
	glm::vec3 center;
	float radius_a, radius_b;
	int pointCount;
};

