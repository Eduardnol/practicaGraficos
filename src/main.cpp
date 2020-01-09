/*  by Alun Evans 2016 LaSalle (aevanss@salleurl.edu) */

//include some standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

//include OpenGL libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <math.h>


//include the tiny object loader
#include <string.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "imageloader.h"
#define SPEED 0.03f

std::string basepath = "assets/";									//path donde estara el objeto que vamos a abrir partiendo de la base del proyecto
												//Nos devolvera error si lo hay en el fichero
	





//include some custom code files
#include "glfunctions.h"											 //include all OpenGL stuff
#include "Shader.h"													// class to compile shaders

using namespace std;
using namespace glm;

//global variables to help us do things
int g_ViewportWidth = 512; int g_ViewportHeight = 512;		// Default window size, in pixels
double mouse_x, mouse_y;											//variables storing mouse position
const vec3 g_backgroundColor(0.2f, 0.2f, 0.2f);						// background colour - a GLM 3-component vector



//Estructura donde vamos a guardar los campos que tiene el fichero obj
typedef struct {

	GLuint g_simpleShader = 0; //shader identifier
	GLuint g_Vao = 0; //vao
	GLuint g_NumTriangles = 0; //  Numbre of triangles we are painting.

}Obj;

Obj obj[2];														//Declaracion de la variable global tetera, la podemos convertir en un vector si hay muchas teteras

float elapsed_time = 0;										//Definimos las coordenadas del movimiento con el raton
float x = 0, z = 1;													//Definimos las coordenadas centro de la tetera
int n = 0;
float y = 0, p = 0, r = -1 , y_camara = 1;
float inicio_x, inicio_y, final_x, final_y;

GLuint texture_id;
GLuint texture_id1;
GLuint texture_id3;
GLuint texture_id4;

vec3 g_light_dir(100.0f, 100.0f, 100.0f);



// ------------------------------------------------------------------------------------------
// This function manually creates a square geometry (defined in the array vertices[])
// ------------------------------------------------------------------------------------------
void load()
{	
	

		string objeto;


			objeto = "sphere.obj";
		

		std::string inputfile = basepath + objeto;				//Nombre del objeto que juntaremos con la path para pasarselo al compilador
		std::vector< tinyobj::shape_t >shapes;								//Clasificamos el archivo obj y colocamos en el vector sus shapes
		std::vector< tinyobj::material_t >materials;						//En este otro vector colocamos los materials

		std::string err;
		//test it loaded correctly

		bool ret = tinyobj::LoadObj(shapes, materials, err, inputfile.c_str(), basepath.c_str());

		if (!err.empty()) {												//Comprueba si hay errores en la variable que hemos definido en la linea 30
			std::cerr << err << endl;
		}

		//print out number of meshes described in file
		std::cout << "# of shapes: " << shapes.size() << std::endl;

		//**********************
		// CODE TO LOAD EVERYTHING INTO MEMORY
		//**********************

		//load the shader
		Shader simpleShader("src/shader.vert", "src/shader.frag");

		obj[n].g_simpleShader = simpleShader.program;

		// Create the VAO where we store all geometry (stored in g_Vao)
		obj[n].g_Vao = gl_createAndBindVAO();



		gl_createAndBindAttribute(&(shapes[0].mesh.positions[0]), shapes[0].mesh.positions.size() * sizeof(float), obj[n].g_simpleShader, "a_vertex", 3);
		
		
		GLfloat* uvs = &(shapes[0].mesh.texcoords[0]);
		GLuint uvs_size = shapes[0].mesh.texcoords.size() * sizeof(GLfloat);

		gl_createAndBindAttribute(uvs, uvs_size, obj[n].g_simpleShader, "a_uv", 2);
		gl_createIndexBuffer(&(shapes[0].mesh.indices[0]), shapes[0].mesh.indices.size() * sizeof(unsigned int));



		GLfloat* normals = &(shapes[0].mesh.normals[0]);
		GLuint normals_size = shapes[0].mesh.normals.size() * sizeof(GLfloat);

		gl_createAndBindAttribute(normals, normals_size, obj[n].g_simpleShader, "a_normal", 3);
		//gl_createIndexBuffer(&(shapes[0].mesh.indices[0]), shapes[0].mesh.indices.size() * sizeof(unsigned int));



		//unbind everything
		gl_unbindVAO();


		//store number of triangles (use in draw())
		/////////////////////////////////////////g_NumTriangles = sizeof(indices) / (sizeof(GLuint) * 3);
		obj[n].g_NumTriangles = shapes[0].mesh.indices.size() / 3;

		



		/*--------------------------------------------------------
			PARA CARGAR LA TEXTURA DE LA TIERRA EN MEMORIA
		--------------------------------------------------------*/


		Image* image = loadBMP("assets/earthmap1k.bmp");

		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGB,
			image->width,
			image->height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			image->pixels);


		Image* image2 = loadBMP("assets/earthlights1k.bmp");

		glGenTextures(1, &texture_id1);
		glBindTexture(GL_TEXTURE_2D, texture_id1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGB,
			image2->width,
			image2->height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			image2->pixels);


		Image* image3 = loadBMP("assets/earthnormal.bmp");

		glGenTextures(1, &texture_id3);
		glBindTexture(GL_TEXTURE_2D, texture_id3);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGB,
			image3->width,
			image3->height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			image3->pixels);

		Image* image4 = loadBMP("assets/earthspec.bmp");

		glGenTextures(1, &texture_id4);
		glBindTexture(GL_TEXTURE_2D, texture_id4);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGB,
			image4->width,
			image4->height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			image4->pixels);



	


		


}



void drawObjects() {


	int a = 0;
	int b = 0;
	int c = 0;
	int n = 0;
	int escala = 1;



		// activate shader

		/*--------------------------------------------------------
			PARA HACER APARECER LA TETERA
		--------------------------------------------------------*/
		glUseProgram(obj[n].g_simpleShader);
		GLuint colorLoc = glGetUniformLocation(obj[n].g_simpleShader, "u_color");
		glUniform3f(colorLoc, 1.0, 0.0, 0.0);



		//bind the geometry
		glBindVertexArray(obj[n].g_Vao);


		// Draw to screen
		/*--------------------------------------------------------
			PARA LA POSICION DLE OBJETO EN EL ESPACIO
		--------------------------------------------------------*/
		mat4 model = translate(mat4(1.0f), vec3(a, b, c));
		GLuint model_loc = glGetUniformLocation(obj[n].g_simpleShader, "u_model");
		glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));


		/*--------------------------------------------------------
			PARA EL MOVIMIENTO DE LA CAMARA
		--------------------------------------------------------*/

		glm::vec3 camPosition(x, y_camara, z);
		glm::vec3 WorldUp(0.f, 1.f, 0.f);
		glm::vec3 camFront(y, p, r);
		glm::mat4 view_matrix(1.f);

		view_matrix = glm::lookAt(
			camPosition,//eye, // the position of your camera, in world space
			camFront,// where you want to look at, in world space
			WorldUp//up // probably glm::vec3(0,1,0)
		);

		GLuint view_loc = glGetUniformLocation(obj[n].g_simpleShader, "u_view");
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));


		/*--------------------------------------------------------
			PARA LA PERSPECTIVA DE LEJANIA
		--------------------------------------------------------*/



		//Ahora mismo la camara y la tetera estan en 0,0,0, debemos mover la tetera en profundidad para poder verla al completo, el FOV no se debe tocar para evitar deofrmaciones
		//create projection matrix and pass to shader
		mat4 projection_matrix = perspective(
			65.0f, // Field of view
			1.0f, // Aspect ratio
			0.1f, // near plane (distance from camera)
			50.0f // Far plane (distance from camera)
		);
		GLuint projection_loc = glGetUniformLocation(obj[n].g_simpleShader, "u_projection");
		glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));



		/*--------------------------------------------------------
			PARA LA TEXTURA DEL PLANETA
		--------------------------------------------------------*/


		GLuint u_texture = glGetUniformLocation(obj[n].g_simpleShader, "u_texture");

		glUniform1i(u_texture, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_id);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture_id1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texture_id3);

		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, texture_id4);



		GLuint u_texture1 = glGetUniformLocation(obj[n].g_simpleShader, "u_texture1");
		glUniform1i(u_texture1, 1);

		GLuint u_texture2 = glGetUniformLocation(obj[n].g_simpleShader, "u_texture3");
		glUniform1i(u_texture2, 2);

		/*GLuint u_texture3 = glGetUniformLocation(obj[n].g_simpleShader, "u_texture4");
		glUniform1i(u_texture3, 3);*/



		/*--------------------------------------------------------
			PARA LA NORMALIZACION
		--------------------------------------------------------*/
		GLuint u_normalize = glGetUniformLocation(obj[n].g_simpleShader, "u_normal");

		glUniform1i(u_normalize, 0);


		/*--------------------------------------------------------
			PARA LA LUZ QUE TOCA AL PLANETA
		--------------------------------------------------------*/

		GLuint u_light_dir = glGetUniformLocation(obj[n].g_simpleShader, "u_light_dir");
//		float g_light_distance = 3.0f, g_light_angle = 45.0f;
		glUniform3f(u_light_dir, g_light_dir.x, g_light_dir.y, g_light_dir.z);

		/*--------------------------------------------------------
			PARA EL COLOR DE LA LUZ QUE TOCA AL PLANETA
		--------------------------------------------------------*/

		GLuint u_light_color = glGetUniformLocation(obj[n].g_simpleShader, "u_light_color");
		glUniform3f(u_light_color, 1.0, 1.0, 1.0);


		/*--------------------------------------------------------
			PARA LA LUZ DIFUSA
		--------------------------------------------------------*/

		GLuint u_shiness = glGetUniformLocation(obj[n].g_simpleShader, "u_shiness");
		GLuint u_ambient = glGetUniformLocation(obj[n].g_simpleShader, "u_ambient");
		
		glUniform1f(u_shiness, 100.0);
		glUniform1f(u_ambient, 0.1);




		/*--------------------------------------------------------
			PARA ACABAR Y DIBUJAR
		--------------------------------------------------------*/


		gl_bindVAO(obj[n].g_Vao);

		glDrawElements(GL_TRIANGLES, 60 * obj[n].g_NumTriangles, GL_UNSIGNED_INT, 0);



}




// ------------------------------------------------------------------------------------------
// This function actually draws to screen and called non-stop, in a loop
// ------------------------------------------------------------------------------------------
void draw(GLFWwindow* window)
{
	

		//clear the screen

	 glClearColor(0.1, 0.2, 0.2, 1);
	 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 glEnable(GL_DEPTH_TEST);

	 glEnable(GL_CULL_FACE);
	 glCullFace(GL_BACK);
	 glLoadIdentity();
	 //glTranslatef(-13, 0, -45);
	 //glRotatef(40, 1, 1, 0);
													//cambiamos el color de fondo de nuestra ventana



	 //drawGrid();
	 drawObjects();

	 // Swap front and back buffers
	 glfwSwapBuffers(window);
	 //glutSwapBuffers();
		


	


}



/*
MOVIMIENTO DE AVANZAR Y RETROCEDER CAMARA
*/



void Move_Camera(float speed)
{
	glm::vec3 camPosition(x, y_camara, z);
	glm::vec3 camFront(y, p, r);
	glm::vec3 vVector = camFront - camPosition;	// Get the view vector



	// forward positive cameraspeed and backward negative -cameraspeed.
	x = x + vVector.x * speed;
	z = z + vVector.z * speed;
	y = y + vVector.x * speed;
	r = r + vVector.z * speed;
}

void Strafe_Camera(float speed) {

	glm::vec3 camPosition_1(x, y_camara, z);
	glm::vec3 camFront_1(y, p, r);


	//Calculamos el vector y que esta a 90 grados del movimiento de las z, un vector ortogonal
	glm::vec3 viewVector = camFront_1 - camPosition_1;
	glm::vec3 ortogVector;


	ortogVector.x = -viewVector.z;
	ortogVector.z = viewVector.x;

	

	x = x + ortogVector.x * speed;
	z = z + ortogVector.z * speed;
	y = y + ortogVector.x * speed;
	r = r + ortogVector.z * speed;

}



// ------------------------------------------------------------------------------------------
// This function is called every time you press a screen
// Se encargara de de detectar si se presiona alguna tecla de forma que se mueva la tetera para donde toque
// ------------------------------------------------------------------------------------------

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    //quit
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
	//reload
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		load();


		x = 0; z = 1; y_camara = 1;													//Definimos las coordenadas centro de la tetera
		y = 0; p = 0; r = -1;
	}


	if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		Strafe_Camera(SPEED);
	}
	fflush(stdin);
	if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		Strafe_Camera(-SPEED);
	}
	fflush(stdin);
	if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		Move_Camera(SPEED);
	}
	if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		Move_Camera(-SPEED);
	}
}

// ------------------------------------------------------------------------------------------
// This function is called every time you click the mouse
// ------------------------------------------------------------------------------------------


void movimientoRaton(GLFWwindow* window) {
	float angle_y = 0.0f;
	float angle_z = 0.0f;
	int mid_x = 0;
	int mid_y = 0;
	int width, height;

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwGetWindowSize(window, &width, &height);//Obtiene el tamano en pixeles de la ventana
		mid_x = width/2;
		mid_y = height/2;

		//mose_x, mouse_y
		glfwGetCursorPos(window, &mouse_x, &mouse_y);
		cout << "Left mouse down at" << mouse_x << ", " << mouse_y << endl;

		if ((mouse_x == mid_x) && (mouse_y == mid_y)) return;

		
		glfwSetCursorPos(window, width / 2, height / 2);

		angle_y = (float)(mid_x - mouse_x) / 1000;
		angle_z = (float)(mid_y - mouse_y) / 1000;

		//El valor sera la velocidad con la que la camara se mueve alrededor
		p += angle_z * 2;
		



		//limitar la rotacion en el eje x
		if ((p - y_camara) > 8) p = y_camara + 8;
		if ((p - y_camara) <-8) p = y_camara - 8;



		glm::vec3 camPosition_1(x, y_camara, z);
		glm::vec3 camFront_1(y, p, r);

		glm::vec3 viewVector = camFront_1 - camPosition_1;


		r = (float)(z + sin(-angle_y) * viewVector.x + cos(-angle_y) * viewVector.z);
		y = (float)(x + cos(-angle_y) * viewVector.x - sin(-angle_y) * viewVector.z);

		
}



int main(void)
{

		
	//setup window and boring stuff, defined in glfunctions.cpp
	GLFWwindow* window;
	if (!glfwInit())return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(g_ViewportWidth, g_ViewportHeight, "Hello OpenGL!", NULL, NULL);
	if (!window) {glfwTerminate();	return -1;}
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;

	glewInit();

	//input callbacks
	

	
	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	//load all the resources
	load();

 

	float time = glfwGetTime();
	float last = time;
	float current = time;
	time = 0;

	int frame_count = 0;



	while (!glfwWindowShouldClose(window))
	{


		//Hay que multiplicar las unidades que nos movemos por las unidades de la diferencia de nuestro elapsed time

	
		current = glfwGetTime();
		elapsed_time = current - last;
		last = current;
		time += elapsed_time;


		draw(window);
		glfwSetKeyCallback(window, key_callback);
		movimientoRaton(window);
		

        
        // Swap front and back buffers
       // glfwSwapBuffers(window);
        
        // Poll for and process events
        glfwPollEvents();
        
        //mouse position must be tracked constantly (callbacks do not give accurate delta)
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
    }

    //terminate glfw and exit
    glfwTerminate();
    return 0;
}

