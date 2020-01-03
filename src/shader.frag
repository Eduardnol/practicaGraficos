#version 330

out vec4 fragColor;
//out vec3 N;


in vec2 v_uv;
in vec3 v_normal;
in vec3 v_world_vertex_pos;

uniform vec3 u_light_color;
uniform vec3 u_cam_pos;
uniform float u_shiness;

uniform vec3 u_color;
uniform sampler2D u_texture;
uniform vec3 u_light_dir;
uniform vec3 g_light_dir;


void main(void)
{

	vec4 texture_color = texture(u_texture, v_uv);
	fragColor =  vec4(texture_color.xyz,1.0);

	vec3 N = normalize(v_normal);
	fragColor = vec4(N, 1.0f);

	vec3 L = normalize(u_light_dir);
	float NdotL = max(dot(N, L), 0.0);

	vec3 R = reflect(L, N);
	vec3 E = normalize(u_cam_pos - v_world_vertex_pos);

	float RdotE = pow( max(dot(R, -E), 0.0), u_shiness);

	vec3 result = u_light_color * RdotE;

	//vec3 final_color = texture_color.xyz * result;

	vec3 final_color = texture_color.xyz * NdotL;
	fragColor = vec4(final_color, 1.0);

	
}
