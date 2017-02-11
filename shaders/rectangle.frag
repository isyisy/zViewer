#version 330
 
out vec4 vFragColor_rec;
uniform vec4 color_rec;

void main()
{
    vFragColor_rec = color_rec;
}