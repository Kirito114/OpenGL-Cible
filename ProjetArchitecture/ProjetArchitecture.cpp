#include <iostream>
#include <ctime>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stddef.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Texture.h"
#include "Objet.h"

//Objects
Objet cible;
Objet projectile;

//Buffers
GLuint vao_target;
GLuint vbo_target;
GLuint ebo_target;
GLuint vao_projectile;
GLuint vbo_projectile;
GLuint ebo_projectile;

//Properties
const GLuint WIDTH = 800, HEIGHT = 600;
GLdouble MOUSE_X = 0, MOUSE_Y = 0, MOUSE_PRESSED_X = 0, MOUSE_PRESSED_Y = 0;

//Shaders
Shader * normalShader;
Shader * nightGreenShader;
Shader * nightRedShader;
Shader * shaderUVLess;
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
enum VisionMode {red, green, normal};
VisionMode visionMode = VisionMode::normal;

//Projectile movement
bool shoot_trigered = false;
float trigger_time = 0;

void key_callback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	else if (key == GLFW_KEY_N && action == GLFW_PRESS)
	{
		if (visionMode == VisionMode::red)
		{
			visionMode = VisionMode::green;
		}
		else if (visionMode == VisionMode::green)
		{
			visionMode = VisionMode::normal;
		}
		else
		{
			visionMode = VisionMode::red;
		}
	}
}

void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	//std::cerr << "GLFW::MOUSE::POSITION " << xpos << " " << ypos << std::endl;
	MOUSE_X = xpos;
	MOUSE_Y = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		//std::cerr << "GLFW::MOUSE::BUTTON_PRESSED::LEFT_BUTTON" << std::endl;
		std::cerr << "GLFW::MOUSE::POSITION " << MOUSE_X << " " << MOUSE_Y << std::endl;
		shoot_trigered = true;
		trigger_time = glfwGetTime();
		MOUSE_PRESSED_X = MOUSE_X;
		MOUSE_PRESSED_Y = MOUSE_Y;
	}
}

void GeomInit()
{
	//On charge la cible
	cible.loadOFF("100x100pointsUV.off",true);
	projectile.loadOFF("sphere.off", false);

	glGenVertexArrays(1,&vao_target);
	glGenBuffers(1, &vbo_target);
	glGenBuffers(1, &ebo_target);
	//Target Vertex Array Object
	glBindVertexArray(vao_target);

	//Position
	glBindBuffer(GL_ARRAY_BUFFER, vbo_target);
	glBufferData(GL_ARRAY_BUFFER, cible.nbsommets * 5 * sizeof(float), (float *)cible.lpoints, GL_STATIC_DRAW);
	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//Texcoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	//Element Buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_target);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cible.nbfaces * 3 * sizeof(unsigned int), (unsigned int *)cible.lfaces, GL_STATIC_DRAW);

	glBindVertexArray(0);
	
	glGenVertexArrays(1, &vao_projectile);
	glGenBuffers(1, &vbo_projectile);
	glGenBuffers(1, &ebo_projectile);

	//Projectile Vertex Array Object
	glBindVertexArray(vao_projectile);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_projectile);
	
	glBufferData(GL_ARRAY_BUFFER, projectile.nbsommets * 5 * sizeof(float), (float *)projectile.lpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_projectile);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, projectile.nbfaces * 3 * sizeof(unsigned int), (unsigned int *)projectile.lfaces, GL_STATIC_DRAW);
	
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

	glm::mat4 model;
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 250.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 300.0f);

	if (visionMode == VisionMode::green)
	{
		shader = nightGreenShader;
	}
	else if (visionMode == VisionMode::red)
	{
		shader = nightRedShader;
	}
	else
	{
		shader = normalShader;
	}

	glActiveTexture(GL_TEXTURE0);
	textureCible.use();
	glUniform1i(glGetUniformLocation(shader->Program, "textureCible"), 0);

	shader->use();

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	view = glm::lookAt(glm::vec3(0.0f, 0.0f, 250.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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

	//Render target
	glBindVertexArray(vao_target);

	glDrawElements(GL_TRIANGLES, cible.nbfaces * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	//A ameliorer
	if (shoot_trigered)
	{
		float elapsed_time = glfwGetTime() - trigger_time;
		if (200 - 120.0*elapsed_time < 0)
			shoot_trigered = false;
		model = glm::mat4();
		model = glm::translate(model, glm::vec3((MOUSE_PRESSED_X-WIDTH/2.0)/5.5*elapsed_time, (HEIGHT/2.0 - MOUSE_PRESSED_Y)/5.5*elapsed_time, 200 - 100.0f*elapsed_time));

		shader = shaderUVLess;
		glm::vec3 color(1.0f, 0.0f, 0.0f);
		shader->use();

		glUniformMatrix4fv(glGetUniformLocation(shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(shader->Program, "color"), color.x, color.y, color.z);

		//Render projectile
		glBindVertexArray(vao_projectile);

		glDrawElements(GL_TRIANGLES, projectile.nbfaces * 3, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
	}
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
	//Callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_move_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

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

	//glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glClearColor(0.2, 0.2, 0.2, 1);

	normalShader = new Shader("vertexShader.glsl", "fragmentShader.glsl");
	nightGreenShader = new Shader("vertexShader.glsl", "nightGreenFragmentShader.glsl");
	nightRedShader = new Shader("vertexShader.glsl", "nightRedFragmentShader.glsl");
	shaderUVLess = new Shader("vertexShaderUVLess.glsl", "fragmentShaderUVLess.glsl");

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwPollEvents();
		render();
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &vao_target);
	glDeleteBuffers(1, &vbo_target);
	glDeleteBuffers(1, &ebo_target);

	glfwTerminate();

	return 0;
}
