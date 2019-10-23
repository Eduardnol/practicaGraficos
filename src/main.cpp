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




//include the tiny object loader
#include <string.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"



std::string basepath = "assets/";									//path donde estara el objeto que vamos a abrir partiendo de la base del proyecto
std::string inputfile = basepath + "teapot_small.obj";				//Nombre del objeto que juntaremos con la path para pasarselo al compilador
std::vector< tinyobj::shape_t >shapes;								//Clasificamos el archivo obj y colocamos en el vector sus shapes
std::vector< tinyobj::material_t >materials;						//En este otro vector colocamos los materials

std::string err;													//Nos devolvera error si lo hay en el fichero
	

bool ret = tinyobj::LoadObj(shapes, materials, err, inputfile.c_str(), basepath.c_str());




//include some custom code files
#include "glfunctions.h"											 //include all OpenGL stuff
#include "Shader.h"													// class to compile shaders

using namespace std;
using namespace glm;

//global variables to help us do things
int g_ViewportWidth = 512; int g_ViewportHeight = 512;				// Default window size, in pixels
double mouse_x, mouse_y;											//variables storing mouse position
const vec3 g_backgroundColor(0.2f, 0.2f, 0.2f);						// background colour - a GLM 3-component vector



//Estructura donde vamos a guardar los campos que tiene el fichero obj
typedef struct {

	GLuint g_simpleShader = 0; //shader identifier
	GLuint g_Vao = 0; //vao
	GLuint g_NumTriangles = 0; //  Numbre of triangles we are painting.

}Obj;

Obj obj[2];														//Declaracion de la variable global tetera, la podemos convertir en un vector si hay muchas teteras



float x = 0, y = 0, z = 1;													//Definimos las coordenadas centro de la tetera
int n = 0;


// ------------------------------------------------------------------------------------------
// This function manually creates a square geometry (defined in the array vertices[])
// ------------------------------------------------------------------------------------------
void load()
{	


	//test it loaded correctly

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

	gl_createIndexBuffer(&(shapes[0].mesh.indices[0]), shapes[0].mesh.indices.size() * sizeof(unsigned int));

	//unbind everything
	gl_unbindVAO();


	//store number of triangles (use in draw())
	/////////////////////////////////////////g_NumTriangles = sizeof(indices) / (sizeof(GLuint) * 3);
	obj[n].g_NumTriangles = shapes[0].mesh.indices.size() / 3;
}



// ------------------------------------------------------------------------------------------
// This function actually draws to screen and called non-stop, in a loop
// ------------------------------------------------------------------------------------------
void draw()
{

		
	//clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1, 0.2, 0.2, 1);													//cambiamos el color de fondo de nuestra ventana

	// activate shader
	glUseProgram(obj[n].g_simpleShader);
	GLuint colorLoc = glGetUniformLocation(obj[n].g_simpleShader, "u_color");
	glUniform3f(colorLoc, 1.0, 0.0, 0.0);



	//bind the geometry
	/////////////////////////////////////////////////gl_bindVAO(g_Vao);
	glBindVertexArray(obj[n].g_Vao);

	// Draw to screen



	mat4 model = translate(mat4(1.0f), vec3(0, 0, 0));
	GLuint model_loc = glGetUniformLocation(obj[n].g_simpleShader, "u_model");
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

	//Para el movimiento de FPS vamos a usar las teclas WASD


	glm::vec3 camPosition(x, 0.f, z);
	glm::vec3 WorldUp(0.f, 1.f, 0.f);
	glm::vec3 camFront(0.f, 0.f, -1.f);
	glm::mat4 view_matrix(1.f);

	view_matrix = glm::lookAt(
		camPosition,//eye, // the position of your camera, in world space
		camPosition + camFront,//center, // where you want to look at, in world space
		WorldUp//up // probably glm::vec3(0,1,0)
	);
	GLuint view_loc = glGetUniformLocation(obj[n].g_simpleShader, "u_view");
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
	 

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





	gl_bindVAO(obj[n].g_Vao);

	glDrawElements(GL_TRIANGLES, 60 * obj[n].g_NumTriangles, GL_UNSIGNED_INT, 0);




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
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
		load();
	if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		x = x + 0.1;    
	}
	fflush(stdin);
	if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		x = x - 0.1;
	}
	fflush(stdin);
	if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		y = y + 0.1;
	}
	fflush(stdin);
	if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		y = y - 0.1;
	}
	fflush(stdin);
	if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		z = z - 0.2;
	}
	if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		z = z + 0.2;
	}
}

// ------------------------------------------------------------------------------------------
// This function is called every time you click the mouse
// ------------------------------------------------------------------------------------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        cout << "Left mouse down at" << mouse_x << ", " << mouse_y << endl;

    }
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
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

	//load all the resources
	load();

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
		draw();
        
        // Swap front and back buffers
        glfwSwapBuffers(window);
        
        // Poll for and process events
        glfwPollEvents();
        
        //mouse position must be tracked constantly (callbacks do not give accurate delta)
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
    }

    //terminate glfw and exit
    glfwTerminate();
    return 0;
}

