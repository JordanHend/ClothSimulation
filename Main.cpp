#include "Model/Model.h"
#include "../Utility/Shader.h"
#include "../Utility/Timer.h"
#include "../Utility/Common.h"
#include <glfw3.h>
GLFWwindow * window;

int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 720;

//Shaders
 Shader compute;
 Shader clothShader;

 //Members used for mouse input.
 double lastX = 0, lastY = 0;
 bool firstMouse = true;

 void framebuffer_size_callback(GLFWwindow * window, int width, int height);
 void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
 void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

 //Force uniform for manipulating cloth. 
 glm::vec3 force;
 void APIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
 {
	 fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n\n",
		 (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		 type, severity, message);
 }

int initOpenGL()
{
	//Init glfw
	glfwInit();
	//Set version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	//Creating window
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window";
		return -1;
	}

	//Set context to current window
	glfwMakeContextCurrent(window);



	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetKeyCallback(window, keyboardCallback);
	glfwSwapInterval(0);


	//initialize GLAD function pointers.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	return 1;


}

Shader modelShader;
GLCamera camera;
float fov;
int main()
{

	//Initializing everything...
	initOpenGL();
	glEnable(GL_DEPTH_TEST);

	//Only enabled for debug.
	//glEnable(GL_DEBUG_OUTPUT);
	//glDebugMessageCallback(MessageCallback, 0);

	
	force = glm::vec3(0, 0, 0);

	//Hide cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Initialize shaders
	modelShader.init("Shaders/animated.vsh", "Shaders/animated.fsh");
	compute.init("Shaders/compute.shader");
	clothShader.init("Shaders/cloth.vsh", "Shaders/cloth.fsh");
	clothShader.use();
	clothShader.setBool("renderNormal", false);

	//Model of cloth.
	/*
	Basically, in model.cpp, it loads the model using assimp and parses the mesh names to see if it should use cloth physics or not (whether the mesh name has 'cloth' in the name'.

	See @ Mesh.cpp for processing of mesh data to be suitable for cloth physics.
	
	On the model's draw, it draws both normal meshes and cloth meshes that are a part of it.
	*/

	Model m;
	m.Init("data/cloth.fbx");

	fov = 40;
	camera.Position = glm::vec3(0, -2, 15);

	float deltatime = 0;
	Timer timer;
	float ticks = 0;
	timer.start();
	timer.unpause();
	while (!glfwWindowShouldClose(window))
	{
		float t = timer.getTicks();
		deltatime = (timer.getTicks() - ticks) / 1000.0f;
		ticks = timer.getTicks();


		//Camera movement 
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltatime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltatime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltatime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltatime);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			float velocity = camera.MovementSpeed * deltatime;
			camera.Position += glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			float velocity = camera.MovementSpeed * deltatime;
			camera.Position += glm::vec3(0.0f, -1.0f, 0.0f) * velocity;
		}




		//Clear buffer bits...
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model = glm::mat4(1);
		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 5000.0f);
		glm::mat4 view = camera.GetViewMatrix();

		//Set uniforms...
		modelShader.use();
		modelShader.setMat4("model", model);
		modelShader.setMat4("view", view);
		modelShader.setMat4("projection", projection);

		clothShader.use();
		glm::mat4 MVP = projection * view * model;
		clothShader.setMat4("MVPMatrix", MVP);
		clothShader.setMat4("model", model);
		clothShader.setVec3("viewPos", camera.Position);
		modelShader.use();
		m.Draw(modelShader);

		//Swap Buffers.
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	}

	return 0;
}


void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	glViewport(0, 0, width, height);
}



void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);

}


bool renderNormal = false;
bool wireframe = false;
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		renderNormal = !renderNormal;
		clothShader.setBool("renderNormal", renderNormal);
	}

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		if (wireframe)
		{
			wireframe = false;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else
		{
			wireframe = true;
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
			
	}
	if (key == GLFW_KEY_ESCAPE)
	{
		//Close
		glfwSetWindowShouldClose(window, true);
	}
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_I)
		{
			force.y += .1;
		}
		if (key == GLFW_KEY_K)
		{
			force.y -= .1;
		}
		if (key == GLFW_KEY_J)
		{
			force.x -= .1;
		}
		if (key == GLFW_KEY_L)
		{
			force.x += .1;
		}

		if (key == GLFW_KEY_U)
		{
			force.z += .1;
		}
		if (key == GLFW_KEY_O)
		{
			force.z -= .1;
		}

	}

}