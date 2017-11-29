#include <dirent.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <render_pass.h>
#include <debuggl.h>

#include <iostream>
#include <string>
#include <vector>

#include "gui.h"

using namespace std;

const string window_title = "Game";
int window_width = 800, window_height = 600;
const char* vertex_shader =
#include "shaders/red.vert"
;

const char* geometry_shader =
#include "shaders/red.geom"
;

const char* fragment_shader =
#include "shaders/red.frag"
;

void
ErrorCallback(int error, const char* description)
{
	cerr << "GLFW Error: " << description << "\n";
}

GLFWwindow* init_glefw()
{
	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwSetErrorCallback(ErrorCallback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	auto ret = glfwCreateWindow(window_width, window_height, window_title.data(), nullptr, nullptr);
	CHECK_SUCCESS(ret != nullptr);
	glfwMakeContextCurrent(ret);
	glewExperimental = GL_TRUE;
	CHECK_SUCCESS(glewInit() == GLEW_OK);
	glGetError();  // clear GLEW's error for it
	glfwSwapInterval(1);
	const GLubyte* renderer = glGetString(GL_RENDERER);  // get renderer string
	const GLubyte* version = glGetString(GL_VERSION);    // version as a string
	std::cout << "Renderer: " << renderer << "\n";
	std::cout << "OpenGL version supported:" << version << "\n";

	return ret;
}

int main(int argc, char* argv[])
{

	GLFWwindow *window = init_glefw();
	CHECK_SUCCESS(window != nullptr);
	GUI gui(window);
	glfwMakeContextCurrent(window);

	vector<glm::vec4> triangles;
	triangles.push_back({-1.0f, -1.0f, 0.0f, 1.0f});
	triangles.push_back({-1.0f, 1.0f, 0.0f, 1.0f});
	triangles.push_back({1.0f, -1.0f, 0.0f, 1.0f});
	triangles.push_back({1.0f, 1.0f, 0.0f, 1.0f});

	vector<glm::uvec3> faces;
	faces.push_back({0, 1, 2});
	faces.push_back({1, 2, 3});

	MatrixPointers mats = gui.getMatrixPointers();

	auto matrix_binder = [](int loc, const void* data) {
		glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)data);
	};

	auto vector3_binder = [](int loc, const void* data) {
		glUniform3fv(loc, 1, (const GLfloat*)data);
	};

	auto std_view_data = [&mats]() -> const void* {
		return mats.view;
	};
	auto std_camera_data  = [&gui]() -> const void* {
		return &gui.getCamera()[0];
	};
	auto std_proj_data = [&mats]() -> const void* {
		return mats.projection;
	};

	ShaderUniform std_view = { "view", matrix_binder, std_view_data };
	ShaderUniform std_camera = { "camera_position", vector3_binder, std_camera_data };
	ShaderUniform std_proj = { "projection", matrix_binder, std_proj_data };

	RenderDataInput object_pass_input;
	object_pass_input.assign(0, "vertex_position", triangles.data(), triangles.size(), 4, GL_FLOAT);
	//object_pass_input.assign(1, "normal", mesh.vertex_normals.data(), mesh.vertex_normals.size(), 4, GL_FLOAT);
	//object_pass_input.assign(2, "uv", uv_coordinates.data(), uv_coordinates.size(), 2, GL_FLOAT);
	object_pass_input.assign_index(faces.data(), faces.size(), 3);
	//object_pass_input.useMaterials(mesh.materials);
	RenderPass object_pass(-1,
			object_pass_input,
			{
			  vertex_shader,
			  geometry_shader,
			  fragment_shader
			},
			{ std_view, std_proj,
			  std_camera },
			{ "fragment_color" }
			);

	while (!glfwWindowShouldClose(window)) {
		// Setup some basic window stuff.
		glfwGetFramebufferSize(window, &window_width, &window_height);
		glViewport(0, 0, window_width, window_height);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthFunc(GL_LESS);

		gui.updateMatrices();

		object_pass.setup();
		CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0));
		
		// Poll and swap.
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
