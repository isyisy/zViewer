#version 330
 
//out vec4 vFragColor;	//fragment shader output

//input from the vertex shader
smooth in vec2 vUV;		//2D texture coordinates

//shader uniform
uniform isampler2D textureMap;		//the image to display

uniform float minvalue;
uniform float maxvalue;

void main()
{
	//sample the textureMap at the given 2D texture coodinates to obntain the colour
	/*vFragColor =   texture(textureMap, vUV);*/
	gl_FragColor =   (texture(textureMap, vUV).rrrr-minvalue)/(maxvalue-minvalue);
}