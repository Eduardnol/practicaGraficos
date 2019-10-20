#version 330
in vec3 a_vertex;
uniform mat4 u_model;
uniform mat4 u_projection;

void main(){

// translate vertex position using u_model transform matrix; is a 4x4 * 4x1 matrix operation

//vec3 position = ( u_model * vec4(a_vertex,1.0)).xyz; //vectors are columns!

gl_Position = u_projection * u_model * vec4( a_vertex , 1.0 );

}