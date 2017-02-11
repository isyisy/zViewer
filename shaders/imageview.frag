#version 330
 
out vec4 vFragColor;	//fragment shader output

//input from the vertex shader
smooth in vec2 vUV;		//2D texture coordinates

//shader uniform
uniform sampler2D textureMap;		//the image to display

uniform float minvalue;
uniform float maxvalue;
uniform float window_center;
uniform float window_width;

void main()
{
	vec4 temp = texture(textureMap, vUV).rrrr;
	float s = temp.x;
	float value0 = minvalue/32768;
	float value1 = maxvalue/32768;
	vFragColor =  (texture(textureMap, vUV).rrrr-value0)/(value1-value0);

	//vFragColor =  texture(textureMap, vUV).rrrr*10;
	//vFragColor =  (texture(textureMap, vUV).rrrr-minvalue)/(maxvalue-minvalue);
	//if(vFragColor.r<0.0)
	//	vFragColor = vec4(0,0,0,0);
	//else if(vFragColor.r>1.0f)
	//	vFragColor = vec4(1.0,1.0,1.0,1.0);
}