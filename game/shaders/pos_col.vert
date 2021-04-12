#version 440 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

out VS_OUT
{
	layout (location = 0) vec4 color;
} vs_out;

void main(void)
{
	vs_out.color = color;
	gl_Position = position;
}

/* end of file */