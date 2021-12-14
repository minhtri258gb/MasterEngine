#version 330 core

in vec2 f_texcoord;

out vec4 FragColor;

uniform sampler2D ourTexture;

void main()
{
	FragColor = texture(ourTexture, f_texcoord);
}
