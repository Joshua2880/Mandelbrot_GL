#version 460 core

in vec2 v_pos;

out vec4 frag_colour;

uniform double u_aspect_ratio;
uniform double u_zoom;
uniform dvec2 u_centre;

void main()
{
	int max_iterations = 1000;

	dvec2 coord = u_centre + u_zoom * dvec2(u_aspect_ratio, 1.0) * v_pos;

	dvec2 z1 = dvec2(0.0, 0.0);
	dvec2 z2 = dvec2(0.0, 0.0);

	int i = 0;
	for (;z2.x + z2.y < 4 && i < max_iterations; ++i)
	{
		z1 = dvec2(z2.x - z2.y, 2 * z1.x * z1.y) + coord;
		z2 = dvec2(z1.x * z1.x, z1.y * z1.y);
	}

	float intensity = (float(max_iterations) - i) / max_iterations;
	frag_colour = vec4(intensity, intensity, intensity, 1.0);
}