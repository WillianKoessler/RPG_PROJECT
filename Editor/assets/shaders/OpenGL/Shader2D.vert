#version 440 core

layout(location = 0) in vec3  a_Position;
layout(location = 1) in vec4  a_Color;
layout(location = 2) in vec2  a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in int a_ID;

uniform mat4 u_ViewProjection;

out vec4 v_Color;
out vec2 v_texCoord;
out flat float v_TexIndex;
out flat int v_ID;


void main()
{
	v_Color = a_Color;
	v_texCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
	v_ID = a_ID;
}
