#version 420 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture_diffuse1;

uniform bool is_selected;
uniform bool is_valid;

void main()
{    
	color = vec4(texture(texture_diffuse1, TexCoords));

	if(!is_valid)
	{
		color = mix(color, vec4(1.0f, 0.0f, 0.0f, 1.0f), 0.2f);
	}

	if(is_selected)
	{
		color = mix(color, vec4(1.0f, 1.0f, 0.0f, 1.0f), 0.2f);
	}
}