#include "Ellipse.h"


CEllipse::CEllipse(void)
{
	color[0]=1;
	color[1]=0;
	color[2]=0;
	color[3]=1;

	//setup shader
	shader.LoadFromFile(GL_VERTEX_SHADER, "shaders/ellipse.vert");
	shader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/ellipse.frag");
	shader.CreateAndLinkProgram();
	shader.Use();	
		shader.AddAttribute("Vertex_circle");
		shader.AddUniform("MVP_circle"); 
		shader.AddUniform("color_circle");
	shader.UnUse();
 
	glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &vboVerticesID);

	//now allocate buffers
	glBindVertexArray(vaoID);	

	glBindBuffer (GL_ARRAY_BUFFER, vboVerticesID);
	glBufferData (GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(shader["Vertex_circle"]);
	glVertexAttribPointer(shader["Vertex_circle"], 3, GL_FLOAT, GL_FALSE,0,0);
		
	glBindVertexArray(0);
}


CEllipse::~CEllipse(void)
{
	shader.DeleteShaderProgram();
	glDeleteBuffers(1, &vboVerticesID);
	glDeleteVertexArrays(1, &vaoID);
}

void CEllipse::RenderCircle(const float* MVP,int line)
{
	shader.Use();				
		glUniformMatrix4fv(shader("MVP_circle"), 1, GL_FALSE, MVP);
		glUniform4fv(shader("color_circle"), 1,color);

		glBindVertexArray(vaoID);
		glBindBuffer (GL_ARRAY_BUFFER, vboVerticesID);
	    glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(vertices),vertices);
	
		if(line)
			glDrawArrays(GL_LINE_LOOP,1,pointCount-2);
		else
			glDrawArrays(GL_TRIANGLE_FAN,0,pointCount);
		glBindVertexArray(0);

	shader.UnUse();
}

void CEllipse::set_color(float r, float g, float b, float a)
{
	color[0]=r;
	color[1]=g;
	color[2]=b;
	color[3]=a;
}

void CEllipse::set_center(glm::vec3 data)
{
	center = data;
}

void CEllipse::set_Radius(float a, float b)
{
	radius_a = a;
	radius_b = b;
}

void CEllipse::computeCirclePoints()
{
	vertices[0] = center;
	
	float delta = 2*3.1415926/100;
	for(int i=0;i<100;i++)
	{
		vertices[1+i].x = center.x + radius_a*cos(i*delta);
		vertices[1+i].y = center.y + radius_b*sin(i*delta);
		vertices[1+i].z = 0;
	}
	pointCount = 100;
	
	/*else if(radius<0.25)
	{
	float delta = 2*3.1415926/20;
	for(int i=0;i<20;i++)
	{
	vertices[1+i].x = center.x + radius*cos(i*delta);
	vertices[1+i].y = center.y + radius*sin(i*delta);
	vertices[1+i].z = 0;
	}
	vertices[21] = vertices[1];
	pointCount = 22;
	}*/
}