#version 330

out vec4 fragColor;

in vec2 v_uv;


uniform vec3 u_color;
uniform sampler2D u_texture;

void main(void)
{
	// We're just going to paint the interpolated colour from the vertex shader

	vec4 texture_color = texture(u_texture, v_uv);
	fragColor =  vec4(texture_color.xyz,1.0);
}
