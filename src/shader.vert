#version 330
in vec3 a_vertex;
in vec2 a_uv;
in vec3 a_normal;

out vec2 v_uv;
out vec3 v_normal;

uniform mat4 u_model;
uniform mat4 u_projection;
uniform mat4 u_view;

void main(){

// translate vertex position using u_model transform matrix; is a 4x4 * 4x1 matrix operation


gl_Position = u_projection * u_model * u_view* vec4( a_vertex , 1.0 );

v_uv = a_uv;
v_normal = a_normal;

}