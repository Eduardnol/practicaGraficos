#version 330

out vec4 fragColor;

in vec2 v_uv;
in vec3 v_normal;
in vec3 v_world_vertex_pos;

uniform vec3 u_light_color;
uniform vec3 u_cam_pos;
uniform float u_shiness;
uniform float u_ambient;

uniform vec3 u_color;
uniform sampler2D u_texture;
uniform sampler2D u_texture1;
uniform vec3 u_light_dir;
uniform vec3 g_light_dir;


mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv)
{
	// get edge vectors of the pixel triangle
	vec3 dp1 = dFdx( p );
	vec3 dp2 = dFdy( p );
	vec2 duv1 = dFdx( uv );
	vec2 duv2 = dFdy( uv );
	// solve the linear system
	vec3 dp2perp = cross( dp2, N );
	vec3 dp1perp = cross( N, dp1 );
	vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
	vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
			 
	// construct a scale-invariant frame 
	float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
	return mat3( T * invmax, B * invmax, N );
}



// assume N, the interpolated vertex normal and 
// V, the view vector (vertex to eye)
vec3 perturbNormal( vec3 N, vec3 V, vec2 texcoord, vec3 normal_pixel )
{

	normal_pixel = normal_pixel * 2.0 - 1.0;
	mat3 TBN = cotangent_frame(N, V, texcoord);
	return normalize(TBN * normal_pixel);
}

void main(void){


	/*--------------------------------------------------------
			EARTH
	--------------------------------------------------------*/

		vec3 texture_color = texture(u_texture, v_uv).xyz;
		vec3 texture_night = texture(u_texture1, v_uv).xyz;

	/*--------------------------------------------------------
			AMBIENT
	--------------------------------------------------------*/
	vec3 ambient_color = texture_color * u_ambient;
	vec3 night_color = texture_night * u_ambient * 20;


	/*--------------------------------------------------------
			DIFUSE
	--------------------------------------------------------*/
	vec3 N = normalize(v_normal);
	//vec3 N_orig = N;

	//N = perturbNormal(N, v_world_vertex_pos, v_uv, texture_color);
	//N = mix(N_orig, N, 1.0);

	vec3 L = normalize(u_light_dir);
	float NdotL = max(dot(N, L), 0.0);
	vec3 diffuse_color = texture_color * NdotL;
	vec3 diffuse_night = texture_night * NdotL;


	/*--------------------------------------------------------
			SPECULAR
	--------------------------------------------------------*/
		vec3 R = reflect(L, N);
		vec3 E = normalize(u_cam_pos - v_world_vertex_pos);
		float RdotE = pow( max(dot(R, E), 0.0), u_shiness);
		vec3 specular = u_light_color * RdotE * 2;


	/*--------------------------------------------------------
			PHONG
	--------------------------------------------------------*/

	vec3 result;

	if(NdotL < 0.1){
	
		result = (night_color + diffuse_night + specular) * night_color;
	
	}
	else{
	
		result = (ambient_color + diffuse_color + specular) * texture_color;
	
	}

	//vec3 result = ambient_color * texture_color;
	//vec3 result = diffuse_color * texture_color;	
	//vec3 result = specular * texture_color;

	fragColor = vec4(result, 1.0);

	
}

