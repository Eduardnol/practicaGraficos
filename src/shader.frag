#version 330

out vec4 fragColor;

in vec2 v_uv;

void main(void)
{
	// We're just going to paint the interpolated colour from the vertex shader
	fragColor =  vec4(v_uv, 0.5 ,1.0);
}
