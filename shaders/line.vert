#version 330
  
in vec3 Vertex_line;
uniform mat4 MVP_line;
void main()
{    
    gl_Position = MVP_line * vec4(Vertex_line,1);
}