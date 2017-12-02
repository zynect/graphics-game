#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
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

void Entity::calcGravity(double deltaTime)
{
	float dt = static_cast<float>(deltaTime);
	if(!isResting && velocity.y < maxYVelocity)
	{
		velocity.y += gravity * dt;
	}
}

void Entity::checkCollision()
{
	// temp floor
	if (position.y + size.y > 600)
	{
		position.y = 600 - size.y;
		velocity.y = 0;
		isResting = true;
	}
}

void Player::run(double deltaTime)
{
	updatePosition(deltaTime, activeAction);
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
	checkCollision();
}

void Enemy::run(double deltaTime)
{
	updatePosition(deltaTime, NONE);
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
	checkCollision();
}
