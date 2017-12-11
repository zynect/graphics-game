#include <dirent.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <debuggl.h>

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>

#include "gui.h"
#include "gameObject.h"
#include "sprites.h"
#include "level.h"

using namespace std;

const string window_title = "Game";
const int initial_window_width = 800, initial_window_height = 600;

const char* vertex_shader =
#include "shaders/texture.vert"
;

const char* fragment_shader =
#include "shaders/texture.frag"
;

// VBO and VAO descriptors.
enum { kVertexBuffer, kIndexBuffer, kUVBuffer, kNumVbos };

// These are our VAOs.
enum { kGeometryVao, kFloorVao, kNumVaos };

GLuint g_array_objects[kNumVaos];  // This will store the VAO descriptors.
GLuint g_buffer_objects[kNumVaos][kNumVbos];  // These will store VBO descriptors.

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
	auto ret = glfwCreateWindow(initial_window_width, initial_window_height, window_title.data(), nullptr, nullptr);
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

vector<shared_ptr<GameObject>> objects;

int main(int argc, char* argv[])
{
	GLFWwindow *window = init_glefw();
	CHECK_SUCCESS(window != nullptr);
	GUI gui(window);
	glfwMakeContextCurrent(window);

	vector<Sprite> sprites;
    std::vector<glm::vec2> frames;
	Image gameTextures;
	const char* textureFile = "../assets/game.bmp";
	assert(readBMP(textureFile, gameTextures) > -1);
	readSprites(sprites, frames, gameTextures.width, gameTextures.height);
	//frames.push_back({{0, 1}, {1, 1}, {0, 0}, {1, 0}});

	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gameTextures.width, gameTextures.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &gameTextures.bytes[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	vector<glm::vec4> triangles;
	triangles.push_back({0.0f, 0.0f, 0.0f, 1.0f});
	triangles.push_back({1.0f, 0.0f, 0.0f, 1.0f});
	triangles.push_back({0.0f, 1.0f, 0.0f, 1.0f});
	triangles.push_back({1.0f, 1.0f, 0.0f, 1.0f});

	vector<glm::uvec3> faces;
	faces.push_back({0, 1, 2});
	faces.push_back({2, 1, 3});

	// Setup our VAO array.
	CHECK_GL_ERROR(glGenVertexArrays(kNumVaos, &g_array_objects[0]));

	// Switch to the VAO for Geometry.
	CHECK_GL_ERROR(glBindVertexArray(g_array_objects[kGeometryVao]));

	// Generate buffer objects
	CHECK_GL_ERROR(glGenBuffers(kNumVbos, &g_buffer_objects[kGeometryVao][0]));

	// Setup vertex data in a VBO.
	CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, g_buffer_objects[kGeometryVao][kVertexBuffer]));
	CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
				sizeof(float) * triangles.size() * 4, &triangles[0],
				GL_STATIC_DRAW));
	CHECK_GL_ERROR(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0));
	CHECK_GL_ERROR(glEnableVertexAttribArray(0));

	// Setup element array buffer.
	CHECK_GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_buffer_objects[kGeometryVao][kIndexBuffer]));
	CHECK_GL_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				sizeof(uint32_t) * faces.size() * 3,
				&faces[0], GL_STATIC_DRAW));

	// Setup UV data.
	CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, g_buffer_objects[kGeometryVao][kUVBuffer]));
	CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
				sizeof(float) * triangles.size() * 2, nullptr,
				GL_STATIC_DRAW));
	CHECK_GL_ERROR(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0));
	CHECK_GL_ERROR(glEnableVertexAttribArray(1));

	// Setup vertex shader.
	GLuint vertex_shader_id = 0;
	const char* vertex_source_pointer = vertex_shader;
	CHECK_GL_ERROR(vertex_shader_id = glCreateShader(GL_VERTEX_SHADER));
	CHECK_GL_ERROR(glShaderSource(vertex_shader_id, 1, &vertex_source_pointer, nullptr));
	glCompileShader(vertex_shader_id);
	CHECK_GL_SHADER_ERROR(vertex_shader_id);

	// Setup fragment shader.
	GLuint fragment_shader_id = 0;
	const char* fragment_source_pointer = fragment_shader;
	CHECK_GL_ERROR(fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER));
	CHECK_GL_ERROR(glShaderSource(fragment_shader_id, 1, &fragment_source_pointer, nullptr));
	glCompileShader(fragment_shader_id);
	CHECK_GL_SHADER_ERROR(fragment_shader_id);

	// Let's create our programs.
	GLuint program_id = 0;
	CHECK_GL_ERROR(program_id = glCreateProgram());
	CHECK_GL_ERROR(glAttachShader(program_id, vertex_shader_id));
	CHECK_GL_ERROR(glAttachShader(program_id, fragment_shader_id));

	// Bind attributes.
	CHECK_GL_ERROR(glBindAttribLocation(program_id, 0, "vertex_position"));
	CHECK_GL_ERROR(glBindAttribLocation(program_id, 1, "vertexUV"));
	CHECK_GL_ERROR(glBindFragDataLocation(program_id, 0, "fragment_color"));
	glLinkProgram(program_id);
	CHECK_GL_PROGRAM_ERROR(program_id);

	// Get the uniform locations.
	GLint projection_matrix_location = 0;
	CHECK_GL_ERROR(projection_matrix_location =
			glGetUniformLocation(program_id, "projection"));
	GLint view_matrix_location = 0;
	CHECK_GL_ERROR(view_matrix_location =
			glGetUniformLocation(program_id, "view"));
	GLint model_matrix_location = 0;
	CHECK_GL_ERROR(model_matrix_location =
			glGetUniformLocation(program_id, "model"));

	MatrixPointers mats = gui.getMatrixPointers();

	double lastTime = glfwGetTime();
	double deltaTime = 0;

	readInLevel("../assets/level1.txt");

	/*objects.push_back(make_shared<Player>(glm::vec2(400, 300), 0, glm::vec2(100, 200), 0));

	objects.push_back(make_shared<Enemy>(glm::vec2(100, 200), 0, glm::vec2(100, 100), 1));
	objects.push_back(make_shared<Enemy>(glm::vec2(100, 50), 0, glm::vec2(100, 100), 1));*/
	/*for (int i = 0; i < 1000; i++)
	{
		objects.push_back(make_shared<Enemy>(glm::vec2((i * 20) % 800, ((i * 20) / 800) * 20), 0, glm::vec2(16, 32), 0));
	}*/

	while (!glfwWindowShouldClose(window)) {
		// Measure speed
		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		gui.updateLoop();

		// Use our program.
		CHECK_GL_ERROR(glUseProgram(program_id));

		// Pass uniforms in.
		CHECK_GL_ERROR(glUniformMatrix4fv(projection_matrix_location, 1, GL_FALSE,
					mats.projection));
		CHECK_GL_ERROR(glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE,
					mats.view));

		for(shared_ptr<GameObject> obj : objects)
		{
			obj->run(deltaTime);
		}

		sort(objects.begin(), objects.end());
		for(shared_ptr<GameObject>& obj : objects)
		{
			glm::mat4 model = *reinterpret_cast<const glm::mat4*>(mats.model) * obj->modelMatrix();

			CHECK_GL_ERROR(glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE,
						&model[0][0]));

			CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, g_buffer_objects[kGeometryVao][kUVBuffer]));
			unsigned int textureId;
			unsigned int frameId;
			obj->getCurrentSprite(textureId, frameId);

			CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
						sizeof(float) * triangles.size() * 2, &frames[sprites[textureId][frameId]][0],
						GL_STATIC_DRAW));

			CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0));
		}

		// Poll and swap.
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
