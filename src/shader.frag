#version 330

out vec4 fragColor;
//out vec3 N;


in vec2 v_uv;
in vec3 v_normal;
in vec3 v_world_vertex_pos;

uniform vec3 u_light_color;
uniform vec3 u_cam_pos;
uniform float u_shiness;
uniform float u_ambient;

uniform vec3 u_color;
uniform sampler2D u_texture;
uniform vec3 u_light_dir;
uniform vec3 g_light_dir;


void main(void)
{




	//Ambient
	vec3 texture_color = texture(u_texture, v_uv).xyz;
	vec3 ambient_color = texture_color * u_ambient;


	//Diffuse
	vec3 N = normalize(v_normal);
	fragColor = vec4(N, 1.0f);
	vec3 L = normalize(u_light_dir);
	float NdotL = max(dot(N, L), 0.0);
	vec3 diffuse_color = texture_color * NdotL;


	//Specular
	vec3 R = reflect(L, N);
	vec3 E = normalize(u_cam_pos - v_world_vertex_pos);
	float RdotE = pow( max(dot(R, E), 0.0), u_shiness);
	vec3 specular = texture_color * u_light_color * RdotE;


	vec3 result =(ambient_color + diffuse_color + specular) * texture_color;
	//vec3 result = ambient_color * texture_color;
	//vec3 result = diffuse_color * texture_color;	
	//vec3 result = specular * texture_color;

	vec3 final_color =  result;
	fragColor = vec4(final_color, 1.0);

	
}
