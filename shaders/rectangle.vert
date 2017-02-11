#version 330
  
in vec3 Vertex_rec;
uniform mat4 MVP_rec;
void main()
{    
    gl_Position = MVP_rec * vec4(Vertex_rec,1);
}