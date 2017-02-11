#version 330
  
in vec2 vVertex; //object space vertex
in vec2 vTcoord; //object space vertex
//vertex shader output
smooth out vec2 vUV;	//texture coordinates for texture lookup in the fragment shader

void main()
{    
	//output the clipspace position
	gl_Position = vec4(vVertex,0,1);	 

	//set the input object space vertex position as texture coordinate
	vUV = vTcoord;
}