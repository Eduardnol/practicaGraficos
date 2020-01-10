#version 330

uniform vec3 u_color;

out vec4 fragColor;
in vec3 v_color;
in vec2 v_uv;
in vec3 v_normal;

uniform vec3 g_light_dir;
uniform vec3 u_light_dir;
uniform sampler2D u_texture;


void main(void){


	vec4 texture_color = texture(u_texture, v_uv);
	fragColor =  vec4(texture_color.xyz, 1.0);

}