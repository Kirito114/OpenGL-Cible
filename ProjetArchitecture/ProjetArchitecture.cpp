#include <iostream>
#include <ctime>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stddef.h>
#include <vector>

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

//Vibration de la cible
unsigned nbFramesVibrate = 0;

//Deformations
float xDeformations[50];
float yDeformations[50];
unsigned nbDeformations = 0;
bool wasTouched = false;

//Vision nocturne
bool nightVision = false;
enum VisionMode {red, green, normal};
VisionMode visionMode = VisionMode::normal;

//Projectile movement
float last_frame_time = 0;
std::vector<glm::vec3> projectiles;

glm::vec3 getWorldPosition()
{
	float mouseX = MOUSE_PRESSED_X / (WIDTH * 0.5f) - 1.0f;
	float mouseY = MOUSE_PRESSED_Y / (HEIGHT * 0.5f) - 1.0f;

	std::cerr << mouseX << " " << mouseY << std::endl;

	glm::mat4 proj = glm::perspective(45.0f, GLfloat(WIDTH) / GLfloat(HEIGHT), 0.1f, 300.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 250.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 invVP = glm::inverse(proj * view);
	glm::vec4 screenPos = glm::vec4(mouseX, -mouseY, 1.0f, 1.0f);
	glm::vec4 worldPos = invVP * screenPos;

	glm::vec3 dir = glm::normalize(glm::vec3(worldPos));

	return dir;
}

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
	MOUSE_X = xpos;
	MOUSE_Y = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		//std::cerr << "GLFW::MOUSE::POSITION " << MOUSE_X << " " << MOUSE_Y << std::endl;
		MOUSE_PRESSED_X = MOUSE_X;
		MOUSE_PRESSED_Y = MOUSE_Y;
		glm::vec3 world_pos = getWorldPosition();
		std::cerr << world_pos.x << " " << world_pos.y << " " << world_pos.z << std::endl;
		projectiles.push_back(glm::vec3(world_pos.x,world_pos.y, 200));
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

	int vision_mode;

	if (visionMode == VisionMode::green)
	{
		//shader = nightGreenShader;
		vision_mode = 2;
	}
	else if (visionMode == VisionMode::red)
	{
		//shader = nightRedShader;
		vision_mode = 1;
	}
	else
	{
		//shader = normalShader;
		vision_mode = 0;
	}
	shader = normalShader;
	shader->use();

	glActiveTexture(GL_TEXTURE0);
	textureCible.use();
	glUniform1i(glGetUniformLocation(shader->Program, "textureCible"), 0);


	GLint visionModeLoc = glGetUniformLocation(shader->Program, "vision_mode");
	glUniform1i(visionModeLoc, vision_mode);

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

	GLint xVibrateLoc = glGetUniformLocation(shader->Program, "xVibrate");
	GLint yVibrateLoc = glGetUniformLocation(shader->Program, "yVibrate");

	if (nbFramesVibrate > 0)
	{
		glUniform1f(xVibrateLoc, (rand() % 1000) / 500.0f);
		glUniform1f(yVibrateLoc, (rand() % 1000) / 500.0f);
		nbFramesVibrate--;
	}
	else
	{
		glUniform1f(xVibrateLoc, 0);
		glUniform1f(yVibrateLoc, 0);
	}

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform1i(glGetUniformLocation(shader->Program, "nbDeformations"), nbDeformations);
	glUniform1fv(glGetUniformLocation(shader->Program, "xDeformations"), 50, xDeformations);
	glUniform1fv(glGetUniformLocation(shader->Program, "yDeformations"), 50, yDeformations);
	glUniform1i(glGetUniformLocation(shader->Program, "wasTouched"), wasTouched);

	//Render target
	glBindVertexArray(vao_target);

	glDrawElements(GL_TRIANGLES, cible.nbfaces * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);


	float elapsed_time = glfwGetTime() - last_frame_time;
	last_frame_time = glfwGetTime();

	shader = shaderUVLess;
	glm::vec3 color(1.0f, 0.0f, 0.0f);
	shader->use();

	glUniformMatrix4fv(glGetUniformLocation(shader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3f(glGetUniformLocation(shader->Program, "color"), color.x, color.y, color.z);

	glBindVertexArray(vao_projectile);
	for (unsigned i = 0; i < projectiles.size(); i++)
	{
		model = glm::mat4();
		model = glm::translate(model, projectiles.at(i));

		projectiles.at(i).x += projectiles.at(i).x*elapsed_time;
		projectiles.at(i).y += projectiles.at(i).y*elapsed_time;
		projectiles.at(i).z += -70 * elapsed_time;


		glUniformMatrix4fv(glGetUniformLocation(shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		//Render projectile
		glDrawElements(GL_TRIANGLES, projectile.nbfaces * 3, GL_UNSIGNED_INT, 0);

		if (projectiles.at(i).z <= 0)
		{
			if ((projectiles.at(i).x > (cible.min.x + xOffset + ((elapsedTime - timeBeginMove) / moveTime) * xOffsetTemp)) && (projectiles.at(i).x < (cible.max.x + xOffset + ((elapsedTime - timeBeginMove) / moveTime) * xOffsetTemp)) && (projectiles.at(i).y > (cible.min.y + yOffset + ((elapsedTime - timeBeginMove) / moveTime) * yOffsetTemp)) && (projectiles.at(i).y < (cible.max.y + yOffset + ((elapsedTime - timeBeginMove) / moveTime) * yOffsetTemp)) && (nbDeformations < 49))
			{
				xDeformations[nbDeformations] = projectiles.at(i).x - xOffset - ((elapsedTime - timeBeginMove) / moveTime) * xOffsetTemp;
				yDeformations[nbDeformations] = projectiles.at(i).y - yOffset - ((elapsedTime - timeBeginMove) / moveTime) * yOffsetTemp;
				nbDeformations++;
				wasTouched = true;
				nbFramesVibrate = 300;
			}
			projectiles.erase(projectiles.begin()+i);
		}
	}
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
