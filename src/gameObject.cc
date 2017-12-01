#include "gameObject.h"

int activeAction = NONE;
bool isRunning = false;
bool isJumping = false;

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
	if (position.y + size.y > 500)
	{
		position.y = 500 - size.y;
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
		velocity.x = -500;
	}
	else if (move == RIGHT)
	{
		velocity.x = 500;
	}
	else //No movement
	{
		velocity.x = 0;
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
