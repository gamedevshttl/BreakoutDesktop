#version 330 core

in vec2 TexCoord;
in vec4 ParticleColor;

out vec4 color;

uniform sampler2D image;

void main()
{
	color = ParticleColor * texture(image, TexCoord);
	//color = texture(image, TexCoord);
}
