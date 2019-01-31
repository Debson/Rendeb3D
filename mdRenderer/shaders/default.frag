#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;


uniform sampler2D texture_diffuse;

void main()
{
	vec4 tex = texture(texture_diffuse, TexCoords);

	FragColor = tex * vec4(1.0);
}