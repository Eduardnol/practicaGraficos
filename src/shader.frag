#version 330

out vec4 fragColor;
//out vec3 N;


in vec2 v_uv;
in vec3 v_normal;



uniform vec3 u_color;
uniform sampler2D u_texture;
uniform vec3 u_light_dir;


void main(void)
{
	// We're just going to paint the interpolated colour from the vertex shader

	//vec4 texture_color = texture(u_texture, v_uv);
	//sfragColor =  vec4(texture_color.xyz,1.0);


	vec3 N = normalize(v_normal);
	fragColor = vec4(N, 1.0f);

	
}
