#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Shader.h"
#include "Objet.h"

//Objects

Objet cible;

//Buffers

GLuint vao;
GLuint vbo_vertex;
GLuint vbo_color;
GLuint ebo;

void key_callback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void GeomInit()
{
	//On charge la cible
	cible.charge_OFF("100x100pointsUV.off");

	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);

	//Position
	glGenBuffers(1, &vbo_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex);
	glBufferData(GL_ARRAY_BUFFER, cible.nbsommets * 3 * sizeof(float), (float *)cible.lpoints, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Couleur
	float* couleur = new float[cible.nbsommets*4];
	for (unsigned int i = 0; i < cible.nbsommets * 4; i+=4)
	{
		couleur[i] = 1.0f;
		couleur[i + 1] = 0.0f;
		couleur[i + 2] = 0.0f;
		couleur[i + 3] = 1.0f;
	}

	glGenBuffers(1, &vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, cible.nbsommets * 4 * sizeof(float), couleur, GL_STATIC_DRAW);
	delete[] couleur;

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//Element Buffer
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cible.nbfaces * 3 * sizeof(unsigned int), (unsigned int *)cible.lfaces, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);


}

void render()
{
	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, cible.nbfaces * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(848, 480, "Projet Architecture", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed" << std::endl;
		return -1;
	}


	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glfwSetKeyCallback(window, key_callback);
	
	Shader shader("vertexShader.glsl", "fragmentShader.glsl");

	GeomInit();

	

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();
		shader.use();
		render();
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}
