#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include "gameObject.h"

struct Actions pressed;

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
	}	else if(thisRight <= thisAbove && thisRight <= thisBelow && thisRight <= thisLeft)	{
		this->position.x = obj->position.x + obj->size.x;
		return RIGHT;
	} else {
		return NONE;
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
	if (position.y + size.y >= 240)
	{
		position.y = 240 - size.y;
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

void Player::collide(const std::shared_ptr<GameObject>& obj)
{
	if(obj == nullptr)
	{
		isResting = false;
	}
	else
	{
		int action = repelFrom(obj);
		try {
			Enemy& en = dynamic_cast<Enemy&>(*obj);
			if(action == UP) {
				velocity.y = -enemyBounce;
			} else {
				//die
			}
		}
		catch(const std::bad_cast& e) {
			if(action == UP) {
				isResting = true;
				velocity.y = 0;
			} else if(action == DOWN) {
				velocity.y = 0;
			} else {
				velocity.x = 0;
			}
		}
	}
}

void Player::run(double deltaTime)
{
	int move = NONE;
	if (pressed.right)
		move = RIGHT;
	else if (pressed.left)
		move = LEFT;
	updatePosition(deltaTime, move);
	animate(deltaTime);
}

void Player::animate(double deltaTime)
{
	if (frameId < 0)
		frameId = -frameId - 1;
	
	if (isJumping)
	{
		frameId = 5;
	}
	else if (fabs(velocity.x) < 3.f)
	{
		frameId = 0;
	}
	else
	{
		timer += deltaTime * fabs(velocity.x);

		if (timer > 5.0f)
		{
			timer -= 5.0f;
			frameId++;
		}

		if (frameId < 1 || frameId > 3)
			frameId = 1;
	}

	if (facingLeft)
		frameId = -frameId - 1;
}

void Enemy::animate(double deltaTime)
{
	timer += deltaTime * 400.f;

	if (timer > 50.0f)
	{
		timer -= 50.0f;
		frameId++;
	}

	if (frameId > 1)
		frameId = 0;
}

void Player::updatePosition(double deltaTime, int move)
{
	float dt = static_cast<float>(deltaTime);

	if (move == LEFT)
	{
		if((pressed.run && velocity.x > -maxXRunVelocity) || (!pressed.run && velocity.x > -maxXVelocity))
		{
			if(velocity.x > 0){
				velocity.x /= friction;
			}
			velocity.x -= horizAccel * dt;
		}
	}
	else if (move == RIGHT)
	{
		if((pressed.run && velocity.x < maxXRunVelocity) || (!pressed.run && velocity.x < maxXVelocity))
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

	if (!isJumping)
	{
		if (velocity.x > 0.1f)
			facingLeft = false;
		else if (velocity.x < -0.1f)
			facingLeft = true;
	}

	//Jumping
	if (!pressed.jump){
		heldJump = false;
	}
	else {
		if(isResting) {
			heldJump = true;
			isResting = false;
			isJumping = true;
			velocity.y = -jumpVelocity;
		}
		else if (heldJump && -velocity.y < maxJumpVelocity) {
			//std::cout << velocity.y << std::endl;
			velocity.y -= jumpHoldBoost;
		}
		else {
			heldJump = false;
		}
	}

	calcGravity(deltaTime);
	position += velocity * dt;
	checkForCollisions();

	if (isResting)
		isJumping = false;
}

void Enemy::run(double deltaTime)
{
	updatePosition(deltaTime, direction);
	animate(deltaTime);
}

void Enemy::collide(const std::shared_ptr<GameObject>& obj)
{
	if(obj == nullptr)
	{
		isResting = false;
	}
	else
	{
		int action = repelFrom(obj);
		if(action == UP) {
			isResting = true;
			velocity.y = 0;
		} else if (action == DOWN) {
			velocity.y = 0;
		} else {
			//velocity.x = -velocity.x;
			direction = (direction == LEFT) ? RIGHT : LEFT;
		}
	}
}

void Enemy::updatePosition(double deltaTime, int move)
{
	float dt = static_cast<float>(deltaTime);

	if (move == LEFT)
	{
		velocity.x = -movespeed;
	}
	else if (move == RIGHT)
	{
		velocity.x = movespeed;
	}

	calcGravity(deltaTime);
	position += velocity * dt;
	checkForCollisions();
}
