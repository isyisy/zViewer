#pragma once
#include "Rectangle.h"


CRectangle::CRectangle(void)
{
	color[0]=1;
	color[1]=0;
	color[2]=1;
	color[3]=1;

	vertices[0] = glm::vec3(0,0,0);
    vertices[1] = glm::vec3(0,0,0);
	vertices[2] = glm::vec3(0,0,0);
    vertices[3] = glm::vec3(0,0,0);

	//setup shader
	shader.LoadFromFile(GL_VERTEX_SHADER, "shaders/rectangle.vert");
	shader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/rectangle.frag");
	shader.CreateAndLinkProgram();
	shader.Use();	
		shader.AddAttribute("Vertex_rec");
		shader.AddUniform("MVP_rec"); 
		shader.AddUniform("color_rec");
	shader.UnUse();
 
	glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &vboVerticesID);

	//now allocate buffers
	glBindVertexArray(vaoID);	

	glBindBuffer (GL_ARRAY_BUFFER, vboVerticesID);
	glBufferData (GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(shader["Vertex_rec"]);
	glVertexAttribPointer(shader["Vertex_rec"], 3, GL_FLOAT, GL_FALSE,0,0);
		
	glBindVertexArray(0);
}


CRectangle::~CRectangle(void)
{
	shader.DeleteShaderProgram();
	glDeleteBuffers(1, &vboVerticesID);
	glDeleteVertexArrays(1, &vaoID);
}

void CRectangle::RenderRec(const float* MVP)
{
	shader.Use();				
		glUniformMatrix4fv(shader("MVP_rec"), 1, GL_FALSE, MVP);

		glUniform4fv(shader("color_rec"), 1,color);

		glBindVertexArray(vaoID);
		glBindBuffer (GL_ARRAY_BUFFER, vboVerticesID);
	    glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(vertices),vertices);
			glDrawArrays(GL_LINE_LOOP,0,4);
		glBindVertexArray(0);

	shader.UnUse();
}

void CRectangle::set_color(int r, int g, int b, int a)
{
	color[0]=r;
	color[1]=g;
	color[2]=b;
	color[3]=a;
}

void CRectangle::set_vertices(glm::vec3 * data)
{
	vertices[0] = data[0];
    vertices[1] = data[1];
	vertices[2] = data[2];
    vertices[3] = data[3];
}