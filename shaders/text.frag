#version 330
in vec2 texpos;
out vec4 vFragColor;
uniform sampler2D tex;
uniform vec4 color;
uniform int option;
 
void main() {
	if(option==1)
		vFragColor = vec4(1, 1, 1, texture(tex, texpos).a) * color;
	else
		vFragColor = vec4(0,0,0,0);
}
