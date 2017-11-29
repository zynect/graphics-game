#include <dirent.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <render_pass.h>
#include <debuggl.h>

#include <iostream>
#include <string>

using namespace std;

int window_width = 800, window_height = 600;

void
ErrorCallback(int error, const char* description)
{
	cerr << "GLFW Error: " << description << "\n";
}

int main(int argc, char* argv[])
{
	string window_title = "Game";
	if (!glfwInit()) exit(EXIT_FAILURE);
	glfwSetErrorCallback(ErrorCallback);

	// Ask an OpenGL 3.3 core profile context
	// It is required on OSX and non-NVIDIA Linux
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(window_width, window_height,
			&window_title[0], nullptr, nullptr);
	CHECK_SUCCESS(window != nullptr);
	glfwMakeContextCurrent(window);

	while(1);
}
