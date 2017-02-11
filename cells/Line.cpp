#pragma once
#include "Line.h"


CLine::CLine()
{
	color[0]=0;
	color[1]=1;
	color[2]=0;
	color[3]=1;

	renderEnds = false;
	renderSeg = true;

	//setup shader
	shader.LoadFromFile(GL_VERTEX_SHADER, "shaders/line.vert");
	shader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/line.frag");
	shader.CreateAndLinkProgram();
	shader.Use();	
		shader.AddAttribute("Vertex_line");
		shader.AddUniform("MVP_line"); 
		shader.AddUniform("color_line");
	shader.UnUse();

	glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &vboVerticesID);

	//now allocate buffers
	glBindVertexArray(vaoID);	

	glBindBuffer (GL_ARRAY_BUFFER, vboVerticesID);
	glBufferData (GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(shader["Vertex_line"]);
	glVertexAttribPointer(shader["Vertex_line"], 3, GL_FLOAT, GL_FALSE,0,0);
	
	glBindVertexArray(0);
}


CLine::~CLine(void)
{
	shader.DeleteShaderProgram();
	glDeleteBuffers(1, &vboVerticesID);
	glDeleteVertexArrays(1, &vaoID);
} 

void CLine::RenderLine(const float* MVP)
{
	glBindVertexArray(vaoID);
	glEnable(GL_POINT_SMOOTH);
	
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shader.Use();				
	glUniformMatrix4fv(shader("MVP_line"), 1, GL_FALSE, MVP);
	glUniform4fv(shader("color_line"), 1,color);
	
	glBindBuffer (GL_ARRAY_BUFFER, vboVerticesID);
	glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(vertices),vertices);
	if(renderEnds)
		glDrawArrays(GL_POINTS,0,2);
	if(renderSeg)
		glDrawArrays(GL_LINES,0,2);
	
	shader.UnUse();
	glBindVertexArray(0);

	//glDisable(GL_BLEND);

	renderEnds = false;
}

void CLine::set_color(float r, float g, float b, float a)
{
	color[0]=r;
	color[1]=g;
	color[2]=b;
	color[3]=a;
}

void CLine::set_vertices(glm::vec3 * data)
{
	vertices[0] = data[0];
    vertices[1] = data[1];
}

void CLine::set_endVisible(bool visible)
{
	renderEnds = visible;
}

void CLine::set_segVisible(bool visible)
{
	renderSeg = visible;
}