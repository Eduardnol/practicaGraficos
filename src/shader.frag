#version 330

out vec4 fragColor;
//out vec3 N;


in vec2 v_uv;
in vec3 v_normal;



uniform vec3 u_color;
uniform sampler2D u_texture;
uniform vec3 u_light_dir;
//uniform vec3 g_light_dir;


void main(void)
{
	// We're just going to paint the interpolated colour from the vertex shader

	vec4 texture_color = texture(u_texture, v_uv);
	fragColor =  vec4(texture_color.xyz,1.0);


	vec3 N = normalize(v_normal);
	fragColor = vec4(N, 1.0f);


	vec3 L = normalize(u_light_dir);
	float NdotL = max(dot(N, L), 0.0);

	vec3 final_color = texture_color.xyz * NdotL;
	fragColor = vec4(final_color, 1.0);

	
}
