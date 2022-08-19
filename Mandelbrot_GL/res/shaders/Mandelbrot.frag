#version 330 core

in vec2 v_pos;

out vec4 frag_colour;

uniform float u_aspect_ratio;
uniform float u_zoom;
uniform vec2 u_centre;

vec2 complex_mul(vec2 z0, vec2 z1)
{
	return vec2(z0.x * z1.x - z0.y * z1.y, z0.x * z1.y + z0.y * z1.x);
}

float squared_length(vec2 z)
{
	return z.x * z.x + z.y * z.y;
}

void main()
{
	int max_iterations = 1000;

	vec2 coord = u_centre + pow(2.0, -u_zoom) * vec2(u_aspect_ratio, 1.0) * v_pos;

	vec2 z = vec2(0.0, 0.0);

	int i = 0;
	for (;squared_length(z) < 2 * 2 && i < max_iterations;)
	{
		z = complex_mul(z, z) + coord;
		++i;
	}

	float intensity = (float(max_iterations) - i) / max_iterations;
	frag_colour = vec4(intensity, intensity, intensity, 1.0);
}