#include "gameObject.h"

void Player::run(double deltaTime)
{
	updateVelocity(deltaTime, NONE);
}

void Player::checkCollision()
{

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
	if(!isResting)
	{
		if(velocity.y < maxYVelocity)
		{
			//position.y += (velocity.y * deltaTime) + (.5 * gravity * deltaTime * deltaTime);
			velocity.y += gravity * dt;
		}
		/*else
		{
			position.y += velocity.y * deltaTime;
		}*/
	}

	position += velocity * dt;
}
