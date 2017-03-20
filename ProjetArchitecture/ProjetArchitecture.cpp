#include <iostream>
#include <ctime>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Texture.h"
#include "Objet.h"

//Objects
Objet cible;

//Buffers
GLuint vao;
GLuint vbo_vertex;
GLuint ebo;

//Properties
const GLuint WIDTH = 800, HEIGHT = 600;

//Shaders
Shader * shader;

//Textures
Texture textureCible;

//Déplacement aléatoire
float timeBeginMove = -1;
float moveTime;
float xOffset = 0;
float yOffset = 0;
float xOffsetTemp;
float yOffsetTemp;

//Vision nocturne
bool nightVision = false;

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
	glGenBuffers(1, &vbo_vertex);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	//Position
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex);
	glBufferData(GL_ARRAY_BUFFER, cible.nbsommets * 5 * sizeof(float), (float *)cible.lpoints, GL_STATIC_DRAW);
	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//Texcoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	//Element Buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cible.nbfaces * 3 * sizeof(unsigned int), (unsigned int *)cible.lfaces, GL_STATIC_DRAW);

	glBindVertexArray(0);

	//Chargement de la texture
	bool load_success = textureCible.load("texture.jpg");
	if (!load_success)
		std::cerr << "ERROR::TEXTURE::LOADING_FAILURE" << std::endl;
	textureCible.define_filtering(GL_LINEAR,GL_LINEAR);
	textureCible.define_looping(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);
	textureCible.unuse();
}

void render()
{
	glActiveTexture(GL_TEXTURE0);
	textureCible.use();
	glUniform1i(glGetUniformLocation(shader->Program, "textureCible"), 0);

	if (!nightVision)
	{
		nightShader->use();
	}
	else
	{
		shader->use();
	}

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 view;
	view = glm::lookAt(glm::vec3(0.0f, 0.0f, 250.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection;
	projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 300.0f);

	GLint modelLoc = glGetUniformLocation(shader->Program, "model");
	GLint viewLoc = glGetUniformLocation(shader->Program, "view");
	GLint projectionLoc = glGetUniformLocation(shader->Program, "projection");

	float elapsedTime = glfwGetTime();
	if (timeBeginMove == -1)
	{
		timeBeginMove = elapsedTime;
		moveTime = (rand() % 1000) / 100.0;
		xOffsetTemp = ((rand() % 1000) / 5.0) - 100;
		yOffsetTemp = ((rand() % 1000) / 5.0) - 100;
	}
	else
	{
		if (elapsedTime - timeBeginMove > moveTime)
		{
			timeBeginMove = elapsedTime;
			moveTime = (rand() % 1000) / 500.0;
			xOffset += xOffsetTemp;
			yOffset += yOffsetTemp;
			xOffsetTemp = ((rand() % 1000) / 5.0) - 100;
			yOffsetTemp = ((rand() % 1000) / 5.0) - 100;
			if (((xOffset + xOffsetTemp) > 140) || ((xOffset + xOffsetTemp) < -140))
			{
				xOffsetTemp = -xOffsetTemp;
			}
			if (((yOffset + yOffsetTemp) > 100) || ((yOffset + yOffsetTemp) < -100))
			{
				yOffsetTemp = -yOffsetTemp;
			}
		}
		else
		{
			GLint xOffsetLoc = glGetUniformLocation(shader->Program, "xOffset");
			GLint yOffsetLoc = glGetUniformLocation(shader->Program, "yOffset");

			glUniform1f(xOffsetLoc, xOffset + (((elapsedTime - timeBeginMove) / moveTime) * xOffsetTemp));
			glUniform1f(yOffsetLoc, yOffset + (((elapsedTime - timeBeginMove) / moveTime) * yOffsetTemp));
		}
	}

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, cible.nbfaces * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

int main()
{
	srand(time(NULL));

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Projet Architecture", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cerr << "ERROR::GLFW::WINDOW_CREATION_FAILURE" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "ERROR::GLEW::INIT_FAILURE" << std::endl;
		return -1;
	}

	glViewport(0, 0, WIDTH, HEIGHT);

	//OpenGL Enables
	glEnable(GL_DEPTH_TEST);

	GeomInit();

	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

	shader = new Shader("vertexShader.glsl", "fragmentShader.glsl");
	nightShader = new Shader("vertexShader.glsl", "nightFragmentShader.glsl");

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwPollEvents();
		render();
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo_vertex);
	glDeleteBuffers(1, &ebo);

	glfwTerminate();

	return 0;
}
