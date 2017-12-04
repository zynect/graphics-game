#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include "gameObject.h"

int activeAction = NONE;
bool isRunning = false;
bool isJumping = false;

glm::mat4 GameObject::modelMatrix()
{
	glm::vec2 center = size / 2.0f;

	// read these comments bottom to top

	// and finally, translate it to the proper place
	glm::mat4 model = glm::translate(glm::vec3(position + center, 0.0f));
	// rotate the object
	model *= glm::rotate(angle, glm::vec3(0.0f, 0.0f, 1.0f));
	// move the center of the object to (0,0)
	model *= glm::translate(glm::vec3(-center, 0.0f));
	// scale the object to the right size
	model *= glm::scale(glm::vec3(size, 1.0f));

	return model;
}

bool GameObject::operator < (const GameObject& obj) const
{
	return (*reinterpret_cast<const unsigned int*>(&key) < *reinterpret_cast<const unsigned int*>(&obj.key));
}

bool GameObject::checkCollision(const std::shared_ptr<GameObject>& obj)
{
	return (this->position.x <= obj->position.x + obj->size.x &&
   	 this->position.x + this->size.x >= obj->position.x &&
   	 this->position.y <= obj->position.y + obj->size.y &&
   	 this->position.y + this->size.y >= obj->position.y);
}

actions GameObject::repelFrom(const std::shared_ptr<GameObject>& obj)
{
	float thisAbove = (this->position.y + this->size.y) - obj->position.y;
	float thisBelow = (obj->position.y + obj->size.y) - this->position.y;
	float thisLeft = (this->position.x + this->size.x) - obj->position.x;
	float thisRight = (obj->position.x + obj->size.x) - this->position.x;
	if(thisAbove <= thisBelow && thisAbove <= thisRight && thisAbove <= thisLeft)	{
		this->position.y = obj->position.y - this->size.y;
		return UP;
	}	else if (thisBelow <= thisAbove && thisBelow <= thisRight && thisBelow <= thisLeft) {
		this->position.y = obj->position.y + obj->size.y;
		return DOWN;
	}	else if(thisLeft <= thisAbove && thisLeft <= thisBelow && thisLeft <= thisRight)	{
		this->position.x = obj->position.x - this->size.x;
		return LEFT;
	}	else {
		this->position.x = obj->position.x + obj->size.x;
		return RIGHT;
	}
}

void Entity::collide(const std::shared_ptr<GameObject>& obj)
{
	if(obj == nullptr)
	{
		isResting = false;
	}
	else
	{
		int action = repelFrom(obj);
		if(action == UP || action == DOWN) {
			velocity.y = 0;
		} else {
			velocity.x = 0;
		}

		isResting = true;
	}
}

void Entity::calcGravity(double deltaTime)
{
	float dt = static_cast<float>(deltaTime);
	if(!isResting && velocity.y < maxYVelocity)
	{
		velocity.y += gravity * dt;
	}
}

void Entity::checkForCollisions()
{
	bool hasCollided = false;
	// temp floor
	if (position.y + size.y >= 600)
	{
		position.y = 600 - size.y;
		velocity.y = 0;
		isResting = true;
		hasCollided = true;
	}

	for(const std::shared_ptr<GameObject> &g : objects)
	{
		if(g.get() != this && GameObject::checkCollision(g))
		{
			collide(g);
			hasCollided = true;
		}
	}
	if(!hasCollided)
		collide(nullptr);
}

void Player::run(double deltaTime)
{
	updatePosition(deltaTime, activeAction);
	animate(deltaTime);
}

void Player::animate(double deltaTime)
{
	if (!isResting)
	{
		frameId = 5;
	}
	else if (velocity.x < 3.f)
	{
		frameId = 0;
	}
	else
	{
		if (frameId < 1.0f)
			frameId = 1.0f;
		
		frameId = (frameId + deltaTime * 10.f);

		if (frameId > 4.0f)
			frameId = 1.0f;
	}
}

void Enemy::animate(double deltaTime)
{
	frameId = (frameId + deltaTime * 10.0f);
	if (frameId > 2)
		frameId = 0;
}

void Player::updatePosition(double deltaTime, int move)
{
	float dt = static_cast<float>(deltaTime);

	if (move == LEFT)
	{
		if((isRunning && velocity.x > -maxXRunVelocity) || (!isRunning && velocity.x > -maxXVelocity))
		{
			if(velocity.x > 0){
				velocity.x /= friction;
			}
			velocity.x -= horizAccel * dt;
		}
	}
	else if (move == RIGHT)
	{
		if((isRunning && velocity.x < maxXRunVelocity) || (!isRunning && velocity.x < maxXVelocity))
		{
			if(velocity.x < 0){
				velocity.x /= friction;
			}
			velocity.x += horizAccel * dt;
		}
	}
	else //No movement
	{
		velocity.x /= friction;
	}

	//Jumping
	if (!isJumping){
		firstJump = false;
	}
	else {
		if(isResting) {
			firstJump = true;
			isResting = false;
			velocity.y = -jumpVelocity;
		}
		else if (firstJump && velocity.y > -maxJumpVelocity) {
			velocity.y -= jumpHoldBoost;
		}
		else {
			isJumping = false;
		}
	}

	calcGravity(deltaTime);
	position += velocity * dt;
	checkForCollisions();
}

void Enemy::run(double deltaTime)
{
	updatePosition(deltaTime, NONE);
	animate(deltaTime);
}

void Enemy::updatePosition(double deltaTime, int move)
{
	float dt = static_cast<float>(deltaTime);

	if (move == LEFT)
	{
		velocity.x = -100;
	}
	else if (move == RIGHT)
	{
		velocity.x = 100;
	}
	else //No movement
	{

	}

	calcGravity(deltaTime);
	position += velocity * dt;
	checkForCollisions();
}
