#version 450 core

in vec2 v_pos;

out vec4 frag_colour;

uniform float u_aspect_ratio;
uniform float u_zoom;
uniform vec2 u_centre;

void main()
{
	int max_iterations = 1000;

	vec2 coord = u_centre + pow(2.0, -u_zoom) * vec2(u_aspect_ratio, 1.0) * v_pos;

	vec2 z1 = vec2(0.0, 0.0);
	vec2 z2 = vec2(0.0, 0.0);

	int i = 0;
	for (;z2.x + z2.y < 4 && i < max_iterations; ++i)
	{
		z1 = vec2(z2.x - z2.y, 2 * z1.x * z1.y) + coord;
		z2 = vec2(z1.x * z1.x, z1.y * z1.y);
	}

	float intensity = (float(max_iterations) - i) / max_iterations;
	frag_colour = vec4(intensity, intensity, intensity, 1.0);
}