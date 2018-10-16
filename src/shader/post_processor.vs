#version 330 core
layout (location = 0) in vec4 vertex;

out vec2 TexCoords;

uniform bool chaos;
uniform bool confuse;
uniform bool shake;
uniform float time;

void main()
{
	gl_Position = vec4(vertex.xy, 0.0f, 1.0f);
	vec2 texture = vertex.zw;
	
	if(chaos){
		float strenght = 0.3;
		vec2 pos = vec2(texture.x + sin(time) * strenght, texture.y + cos(time) * strenght);
		TexCoords = pos;
	}
	else if(confuse){
		TexCoords = vec2(1.0 - texture.x, 1.0 - texture.y);
	}
	else{
		TexCoords = texture;
	}

	if(shake){
		float strenght = 0.01;
		gl_Position.x += cos(time * 10) * strenght;
		gl_Position.y += cos(time * 15) * strenght;
	}	
}