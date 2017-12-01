#include "gameObject.h"

void GameObject::checkCollision()
{

}

void Player::run(double deltaTime)
{
	//updateVelocity(deltaTime, NONE);
	angle += deltaTime * 10.0f;
}

void Player::updateVelocity(double deltaTime, int move)
{
	float dt = static_cast<float>(deltaTime);
	if (move == LEFT)
	{
		velocity.x = -1;
	}
	else if (move == RIGHT)
	{
		velocity.x = 1;
	}
	else //No movement
	{

	}

	//Gravity
	if(!isResting && velocity.y < maxYVelocity)
	{
		velocity.y += gravity * dt;
	}

	position += velocity * dt;
}
