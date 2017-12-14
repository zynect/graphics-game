#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include "gameObject.h"

struct Actions pressed;
float flashTimer = 0;

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
	unsigned int thisKey = (zIndex << 28) | (textureId & 0xFFFFFFF);
	unsigned int objKey = (obj.zIndex << 28) | (obj.textureId & 0xFFFFFFF);
	return thisKey < objKey;
}

bool GameObject::checkCollision(const std::shared_ptr<GameObject>& obj)
{
	return (this->position.x <= obj->position.x + obj->size.x &&
   	 this->position.x + this->size.x >= obj->position.x &&
   	 this->position.y <= obj->position.y + obj->size.y &&
   	 this->position.y + this->size.y >= obj->position.y + 1);
}

actions GameObject::testCollision(const std::shared_ptr<GameObject>& obj, glm::vec2 velocity)
{
	int thisAbove = (this->position.y + this->size.y) - (obj->position.y + 1);
	int thisBelow = (obj->position.y + obj->size.y) - this->position.y;
	int thisLeft = (this->position.x + this->size.x) - obj->position.x;
	int thisRight = (obj->position.x + obj->size.x) - this->position.x;

	try {
		Coin& c = dynamic_cast<Coin&>(*obj);
		return COIN;
	}
	catch(const std::bad_cast& e) {}
	try {
		Mushroom& m = dynamic_cast<Mushroom&>(*obj);
		return MUSHROOM;
	}
	catch(const std::bad_cast& e) {}

	if(thisAbove < thisBelow && thisAbove < thisRight && thisAbove < thisLeft) {
		return UP;
	} else if (thisBelow < thisAbove && thisBelow < thisRight && thisBelow < thisLeft) {
		return DOWN;
	} else if(thisLeft < thisAbove && thisLeft < thisBelow && thisLeft < thisRight && velocity.x >= 0) {
		return LEFT;
	} else if(thisRight < thisAbove && thisRight < thisBelow && thisRight < thisLeft && velocity.x <= 0) {
		return RIGHT;
	} else {
		return NONE;
	}
}

actions GameObject::repelFrom(const std::shared_ptr<GameObject>& obj, glm::vec2 velocity)
{
	actions action = testCollision(obj, velocity);
	
	if(action == UP) {
		this->position.y = obj->position.y - this->size.y + 1;
	} else if (action == DOWN) {
		this->position.y = obj->position.y + obj->size.y;
	} else if(action == LEFT) {
		this->position.x = obj->position.x - this->size.x;
	} else if(action == RIGHT) {
		this->position.x = obj->position.x + obj->size.x;
	}

	return action;
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
	/*if (position.y + size.y >= 240)
	{
		position.y = 240 - size.y;
		velocity.y = 0;
		isResting = true;
		hasCollided = true;
	}*/

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

void Player::changePowerUpState(int change)
{
	int oldState = powerUpState;

	if (change == 0)
		powerUpState = 0;
	else
		powerUpState += change;

	if (powerUpState < 0)
	{
		die();
		return;
	}

	if (change < 0)
	{
		invincibleTimer = 3;
	}

	if (powerUpState == 0)
	{
		size.y = 16;
		position.y += 16;
		textureId = 1;
		return;
	}

	if (oldState < 1)
	{
		size.y = 32;
		position.y -= 16;
	}

	if (powerUpState == 1)
	{
		textureId = 0;
	}
}

void Player::die()
{
	timer = 0;
	velocity.x = 0;
	velocity.y -= 400;
}

void Player::collide(const std::shared_ptr<GameObject>& obj)
{
	if(obj == nullptr)
	{
		isResting = false;
	}
	else if (powerUpState >= 0)
	{
		try {
			Background& b = dynamic_cast<Background&>(*obj);
			isResting = false;
		}
		catch(const std::bad_cast& e) { 
			try {
				Enemy& en = dynamic_cast<Enemy&>(*obj);
				int action = testCollision(obj, velocity);
				if(action == UP) {
					isJumping = true;
					velocity.y = -enemyBounce;
					en.die();
				} else if (invincibleTimer <= 0) {
					changePowerUpState(-1);
				}
			}
			catch(const std::bad_cast& e) {
				int action = repelFrom(obj, velocity);
				if(action == UP) {
					isResting = true;
					velocity.y = 0;
				} else if(action == DOWN) {
					velocity.y = 0;
					heldJump = false;
				} else if(action == LEFT || action == RIGHT){
					velocity.x = 0;
				} else if(action == COIN){
					Coin& c = dynamic_cast<Coin&>(*obj);
					c.die();
				} else if(action == MUSHROOM){
					Mushroom& m = dynamic_cast<Mushroom&>(*obj);
					m.die();
					changePowerUpState(1);
				}
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
	
	if (position.y >= 240)
	{
		
	}
}

void Player::animate(double deltaTime)
{
	if (powerUpState < 0)
	{
		frameId = 14;
		return;
	}

	if (invincibleTimer > 0)
	{
		invincibleTimer -= deltaTime;
		if (frameId != 0)
		{
			frameId = 0;
			return;
		}
	}

	if (frameId < 0)
		frameId = -frameId;

	if (isJumping)
	{
		frameId = 6;
	}
	else if (fabs(velocity.x) < 3.f)
	{
		frameId = 1;
	}
	else
	{
		timer += deltaTime * fabs(velocity.x);

		if (timer > 5.0f)
		{
			timer -= 5.0f;
			frameId++;
		}

		if (frameId < 2 || frameId > 4)
			frameId = 2;
	}

	if (facingLeft)
		frameId = -frameId;
}

void Enemy::animate(double deltaTime)
{
	timer += deltaTime * 400.f;

	if(isDead){
		frameId = 2;
		return;
	}

	if (timer > 50.0f)
	{
		timer -= 50.0f;
		frameId = -frameId;
	}
}

void Player::updatePosition(double deltaTime, int move)
{
	float dt = static_cast<float>(deltaTime);

	if (powerUpState >= 0)
	{
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
	}

	calcGravity(deltaTime);
	position += velocity * dt;
	checkForCollisions();

	if (isResting)
		isJumping = false;
}

void Enemy::run(double deltaTime)
{
	if(position.y >= 240 || (isDead && timer > 80.0f)) {
		//remove the enemy from objects
		for (unsigned int i = 0; i < objects.size(); i++) {
			if(objects[i].get() == this) {
				objects.erase(objects.begin() + i);
				break;
			}
		}
	}
	else if (!isDead) {
		updatePosition(deltaTime, direction);
	}
	animate(deltaTime);
}

void Enemy::die()
{
	if(!isDead){
		timer = 0.0f;
		velocity = {0, 0};
		isDead = true;
	}
}

void Enemy::collide(const std::shared_ptr<GameObject>& obj)
{
	if(obj == nullptr)
	{
		isResting = false;
	}
	else
	{
		try {
			Player& ply = dynamic_cast<Player&>(*obj);
		}
		catch(const std::bad_cast& e) {
			int action = repelFrom(obj, velocity);
			if(action == UP) {
				isResting = true;
				velocity.y = 0;
			} else if (action == DOWN) {
				velocity.y = 0;
			} else if (action == LEFT || action == RIGHT) {
				direction = (direction == LEFT) ? RIGHT : LEFT;
			}
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

void Coin::run(double deltaTime)
{
	if(isDead)
	{
		for (unsigned int i = 0; i < objects.size(); i++) {
			if(objects[i].get() == this) {
				objects.erase(objects.begin() + i);
				break;
			}
		}
	}
	animate(deltaTime);
}

void Coin::die()
{
	if(!isDead)
		isDead = true;
}

void Coin::animate(double deltaTime)
{
	frameId = std::floor(flashTimer) + 1;

	if (frameId > 3)
	{
		frameId = -frameId + 6;
	}
}

void Mushroom::run(double deltaTime)
{
	if(position.y >= 240 || isDead) {
		//remove the enemy from objects
		for (unsigned int i = 0; i < objects.size(); i++) {
			if(objects[i].get() == this) {
				objects.erase(objects.begin() + i);
				break;
			}
		}
	}
	else if (!isDead) {
		updatePosition(deltaTime, direction);
	}
}

void Mushroom::collide(const std::shared_ptr<GameObject>& obj)
{
	if(obj == nullptr)
	{
		isResting = false;
	}
	else
	{
		int action = repelFrom(obj, velocity);
		if(action == UP) {
			isResting = true;
			velocity.y = 0;
		} else if (action == DOWN) {
			velocity.y = 0;
		} else if (action == LEFT || action == RIGHT) {
			direction = (direction == LEFT) ? RIGHT : LEFT;
		}
	}
}

void Mushroom::die()
{
	if(!isDead)
		isDead = true;	
}

void Mushroom::animate(double deltaTime)
{
	
}

void Mushroom::updatePosition(double deltaTime, int move)
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
