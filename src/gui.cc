#include "gui.h"
#include <iostream>
#include <debuggl.h>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

using namespace std;

GUI::GUI(GLFWwindow* window)
	:window_(window)
{
	glfwSetWindowUserPointer(window_, this);
	glfwSetKeyCallback(window_, KeyCallback);
	glfwSetCursorPosCallback(window_, MousePosCallback);
	glfwSetMouseButtonCallback(window_, MouseButtonCallback);

	glfwGetWindowSize(window_, &window_width_, &window_height_);
	float aspect_ = static_cast<float>(window_width_) / window_height_;
	projection_matrix_ = glm::ortho(0, window_width_, window_height_, 0);
}

GUI::~GUI()
{
}

void GUI::updateLoop()
{
	glfwGetFramebufferSize(window_, &window_width_, &window_height_);
	glViewport(0, 0, window_width_, window_height_);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	updateMatrices();
}

void GUI::keyCallback(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window_, GL_TRUE);
		return;
	}

	//cout << key << ' ' << action << endl;

	/*if (key == GLFW_KEY_W) {
	} else if (key == GLFW_KEY_S) {
	} else*/
	if (key == GLFW_KEY_A || key == GLFW_KEY_LEFT) {
		if (action == GLFW_RELEASE) {
			activeAction = NONE;
		} else {
			activeAction = LEFT;
		}
	}
	else if (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) {
		if (action == GLFW_RELEASE) {
			activeAction = NONE;
		} else {
			activeAction = RIGHT;
		}
	}

	if(key == GLFW_KEY_SPACE){
		if(action == GLFW_PRESS){
			isJumping = true;
		} else if(action == GLFW_RELEASE) {
			isJumping = false;
		}
	}

	if(key == GLFW_KEY_LEFT_SHIFT){
		if(action == GLFW_RELEASE){
			isRunning = false;
		} else {
			isRunning = true;
		}
	}
}

void GUI::mousePosCallback(double mouse_x, double mouse_y)
{
	last_x_ = current_x_;
	last_y_ = current_y_;
	current_x_ = mouse_x;
	current_y_ = window_height_ - mouse_y;
	float delta_x = current_x_ - last_x_;
	float delta_y = current_y_ - last_y_;
	if (sqrt(delta_x * delta_x + delta_y * delta_y) < 1e-15)
		return;
	glm::vec3 mouse_direction = glm::normalize(glm::vec3(delta_x, delta_y, 0.0f));
	glm::vec2 mouse_start = glm::vec2(last_x_, last_y_);
	glm::vec2 mouse_end = glm::vec2(current_x_, current_y_);
}

void GUI::mouseButtonCallback(int button, int action, int mods)
{
	current_button_ = button;
}

void GUI::updateMatrices()
{
	// Compute our view, and projection matrices.
	/*if (fps_mode_)
		center_ = eye_ + camera_distance_ * look_;
	else*/
	//eye_ = center_ - camera_distance_ * look_;

	view_matrix_ = glm::mat4(1.0f);
	view_matrix_[3].x = camera_position_.x;
	view_matrix_[3].y = camera_position_.y;
	view_matrix_[3].z = camera_distance_;
	//light_position_ = glm::vec4(eye_, 1.0f);

	aspect_ = static_cast<float>(window_width_) / window_height_;
	projection_matrix_ = glm::ortho(0.0f, static_cast<float>(window_width_), static_cast<float>(window_height_), 0.0f);
	//projection_matrix_ = glm::perspective(60.0f, (float)window_width_ / (float)window_height_, 0.1f, 100.0f);
	model_matrix_ = glm::mat4(1.0f);
}

MatrixPointers GUI::getMatrixPointers() const
{
	MatrixPointers ret;
	ret.projection = &projection_matrix_[0][0];
	ret.model= &model_matrix_[0][0];
	ret.view = &view_matrix_[0][0];
	return ret;
}


// Delegrate to the actual GUI object.
void GUI::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	GUI* gui = (GUI*)glfwGetWindowUserPointer(window);
	gui->keyCallback(key, scancode, action, mods);
}

void GUI::MousePosCallback(GLFWwindow* window, double mouse_x, double mouse_y)
{
	GUI* gui = (GUI*)glfwGetWindowUserPointer(window);
	gui->mousePosCallback(mouse_x, mouse_y);
}

void GUI::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	GUI* gui = (GUI*)glfwGetWindowUserPointer(window);
	gui->mouseButtonCallback(button, action, mods);
}
