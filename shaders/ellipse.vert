#version 330
  
in vec3 Vertex_circle;
uniform mat4 MVP_circle;
void main()
{    
    gl_Position = MVP_circle * vec4(Vertex_circle,1);
}